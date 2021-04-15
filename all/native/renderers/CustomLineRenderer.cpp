#include "CustomLineRenderer.h"
#include "graphics/Bitmap.h"
#include "graphics/ViewState.h"
#include "layers/VectorLayer.h"
#include "renderers/MapRenderer.h"
#include "renderers/utils/GLResourceManager.h"
#include "renderers/utils/Shader.h"
#include "renderers/utils/Texture.h"
#include "renderers/drawdatas/CustomLineDrawData.h"
#include "styles/CustomLineStyle.h"
#include "renderers/components/RayIntersectedElement.h"
#include "utils/Const.h"
#include "utils/Log.h"
#include "vectorelements/CustomLine.h"


#include <cglib/mat.h>
#include <cglib/vec.h>

namespace carto {

    CustomLineRenderer::CustomLineRenderer() :
            _mapRenderer(),
            _elements(),
            _tempElements(),
            _drawDataBuffer(),
            _lineDrawDataBuffer(),
            _colorBuf(),
            _coordBuf(),
            _normalBuf(),
            _texCoordBuf(),
            _indexBuf(),
            _textureCache(),
            _progressBuf(),
            _trafficBuf(),
            _shader(),
            _a_color(0),
            _a_coord(0),
            _a_normal(0),
            _a_texCoord(0),
            _a_progress(0),
            _a_traffic(0),
            _u_gamma(0),
            _u_dpToPX(0),
            _u_unitToDP(0),
            _u_mvpMat(0),
            _u_tex_before(0),
            _u_tex_after(0),
            _u_progress(0),
            _u_gradientDistance(0),
            _u_beforeColor(0),
            _u_afterColor(0),
            _u_traffic_color_0(0),
            _progress(-1),
            _mutex()
    {
    }

    CustomLineRenderer::~CustomLineRenderer() {
    }

    void CustomLineRenderer::setComponents(const std::weak_ptr<Options>& options, const std::weak_ptr<MapRenderer>& mapRenderer) {
        std::lock_guard<std::mutex> lock(_mutex);

        _mapRenderer = mapRenderer;
        _textureCache.reset();
        _shader.reset();
    }

    void CustomLineRenderer::offsetLayerHorizontally(double offset) {
        // Offset current draw data batch horizontally by the required amount
        std::lock_guard<std::mutex> lock(_mutex);

        for (const std::shared_ptr<CustomLine>& element : _elements) {
            element->getDrawData()->offsetHorizontally(offset);
        }
    }

    void CustomLineRenderer::onDrawFrame(float deltaSeconds, const ViewState& viewState) {
        std::lock_guard<std::mutex> lock(_mutex);

        if (_elements.empty()) {
            // Early return, to avoid calling glUseProgram etc.
            return;
        }

        if (!initializeRenderer()) {
            return;
        }

        glDisable(GL_CULL_FACE);

        bind(viewState);

        // Draw, batch by bitmap
        for (const std::shared_ptr<CustomLine>& element : _elements) {
            addToBatch(element, viewState);
        }
        drawBatch(viewState);

        unbind();

        glEnable(GL_CULL_FACE);

        GLContext::CheckGLError("CustomLineRenderer::onDrawFrame");
    }

    void CustomLineRenderer::addElement(const std::shared_ptr<CustomLine>& element) {
        if (element->getDrawData()) {
            _tempElements.push_back(element);
        }
    }

    void CustomLineRenderer::refreshElements() {
        std::lock_guard<std::mutex> lock(_mutex);
        _elements.clear();
        _elements.swap(_tempElements);
    }

    void CustomLineRenderer::updateElement(const std::shared_ptr<CustomLine>& element) {
        std::lock_guard<std::mutex> lock(_mutex);
        if (std::find(_elements.begin(), _elements.end(), element) == _elements.end()) {
            if (element->getDrawData()) {
                _elements.push_back(element);
            }
        }
    }

    void CustomLineRenderer::removeElement(const std::shared_ptr<CustomLine>& element) {
        std::lock_guard<std::mutex> lock(_mutex);
        _elements.erase(std::remove(_elements.begin(), _elements.end(), element), _elements.end());
    }

    void CustomLineRenderer::calculateRayIntersectedElements(const std::shared_ptr<VectorLayer>& layer, const cglib::ray3<double>& ray, const ViewState& viewState, std::vector<RayIntersectedElement>& results) const {
        std::lock_guard<std::mutex> lock(_mutex);

        std::vector<MapPos> worldCoords;
        for (const std::shared_ptr<CustomLine>& element : _elements) {
            FindElementRayIntersection(element, element->getDrawData(), layer, ray, viewState, results);
        }
    }

    void CustomLineRenderer::BuildAndDrawBuffers(GLuint a_color,
                                                 GLuint a_coord,
                                                 GLuint a_normal,
                                                 GLuint a_texCoord,
                                                 GLuint a_progress,
                                                 GLuint a_traffic,
                                                 std::vector<unsigned char>& colorBuf,
                                                 std::vector<float>& coordBuf,
                                                 std::vector<float>& normalBuf,
                                                 std::vector<float>& texCoordBuf,
                                                 std::vector<unsigned short>& indexBuf,
                                                 std::vector<float>& progressBuf,
                                                 std::vector<float>& trafficBuf,
                                                 std::vector<const CustomLineDrawData*>& drawDataBuffer,
                                                 const ViewState& viewState)
    {
        // Get bitmap
        std::shared_ptr<Bitmap> bitmap = drawDataBuffer.front()->getBeforeBitmap();

        // Calculate buffer size
        std::size_t totalCoordCount = 0;
        std::size_t totalIndexCount = 0;
        for (const CustomLineDrawData* drawData : drawDataBuffer) {
            for (std::size_t i = 0; i < drawData->getCoords().size(); i++) {
                const std::vector<cglib::vec3<double>*>& coords = drawData->getCoords()[i];
                const std::vector<unsigned int>& indices = drawData->getIndices()[i];

                totalCoordCount += coords.size();
                totalIndexCount += indices.size();
            }
        }

        // Resize the buffers, if necessary
        if (coordBuf.size() < totalCoordCount * 3) {
            colorBuf.resize(std::min(totalCoordCount * 4, GLContext::MAX_VERTEXBUFFER_SIZE * 4));
            coordBuf.resize(std::min(totalCoordCount * 3, GLContext::MAX_VERTEXBUFFER_SIZE * 3));
            normalBuf.resize(std::min(totalCoordCount * 4, GLContext::MAX_VERTEXBUFFER_SIZE * 4));
            texCoordBuf.resize(std::min(totalCoordCount * 2, GLContext::MAX_VERTEXBUFFER_SIZE * 2));
            progressBuf.resize(std::min(totalCoordCount * 1, GLContext::MAX_VERTEXBUFFER_SIZE * 1));
            trafficBuf.resize(std::min(totalCoordCount * 1, GLContext::MAX_VERTEXBUFFER_SIZE * 1));
        }

        if (indexBuf.size() < totalIndexCount) {
            indexBuf.resize(std::min(totalIndexCount, GLContext::MAX_VERTEXBUFFER_SIZE));
        }

        // Calculate and draw buffers
        cglib::vec3<double> cameraPos = viewState.getCameraPos();
        std::size_t colorIndex = 0;
        std::size_t coordIndex = 0;
        std::size_t normalIndex = 0;
        std::size_t texCoordIndex = 0;
        std::size_t indexIndex = 0;
        std::size_t progressIndex = 0;
        std::size_t trafficIndex = 0;
        float texCoordYScale = (bitmap->getHeight() > 1 ? 1.0f / viewState.getUnitToDPCoef() : 1.0f);
        for (const CustomLineDrawData* drawData : drawDataBuffer) {
            // Draw data vertex info may be split into multiple buffers, draw each one
            for (std::size_t i = 0; i < drawData->getCoords().size(); i++) {

                // Check for possible overflow in the buffer
                const std::vector<unsigned int>& indices = drawData->getIndices()[i];
                if (indexIndex + indices.size() > GLContext::MAX_VERTEXBUFFER_SIZE) {
                    // If it doesn't fit, stop and draw the buffers
                    glVertexAttribPointer(a_color, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, colorBuf.data());
                    glVertexAttribPointer(a_coord, 3, GL_FLOAT, GL_FALSE, 0, coordBuf.data());
                    glVertexAttribPointer(a_normal, 4, GL_FLOAT, GL_FALSE, 0, normalBuf.data());
                    glVertexAttribPointer(a_texCoord, 2, GL_FLOAT, GL_FALSE, 0, texCoordBuf.data());
                    glVertexAttribPointer(a_progress, 1, GL_FLOAT, GL_FALSE, 0, progressBuf.data());
                    glVertexAttribPointer(a_traffic, 1, GL_FLOAT, GL_FALSE, 0, trafficBuf.data());
                    glDrawElements(GL_TRIANGLES, indexIndex, GL_UNSIGNED_SHORT, indexBuf.data());
                    // Start filling buffers from the beginning
                    colorIndex = 0;
                    coordIndex = 0;
                    normalIndex = 0;
                    texCoordIndex = 0;
                    indexIndex = 0;
                    progressIndex = 0;
                    trafficIndex = 0;
                }

                // Indices
                std::size_t indexOffset = coordIndex / 3;
                std::vector<unsigned int>::const_iterator iit;
                for (iit = indices.begin(); iit != indices.end(); ++iit) {
                    indexBuf[indexIndex] = static_cast<unsigned short>(indexOffset + *iit);
                    indexIndex++;
                }

                // Coords, tex coords and colors
                Color color = drawData->getColor();
                float normalScale = drawData->getNormalScale();

                // If subpixel width is requested, adjust normal scale and fade color
                if (normalScale < 0.5f) {
                    float c = normalScale / 0.5f;
                    color = Color(
                            static_cast<unsigned char>(color.getR() * c),
                            static_cast<unsigned char>(color.getG() * c),
                            static_cast<unsigned char>(color.getB() * c),
                            static_cast<unsigned char>(color.getA() * c)
                    );
                    normalScale = 0.5f;
                }
                const std::vector<cglib::vec3<double>*>& coords = drawData->getCoords()[i];
                const std::vector<cglib::vec4<float> >& normals = drawData->getNormals()[i];
                const std::vector<cglib::vec2<float> >& texCoords = drawData->getTexCoords()[i];
                const std::vector<float>& progresses = drawData->getProgresses()[i];
                const std::vector<int>& traffics = drawData->getTraffics()[i];
                auto cit = coords.begin();
                auto nit = normals.begin();
                auto tit = texCoords.begin();
                auto pit = progresses.begin();
                auto trit = traffics.begin();
                for ( ; cit != coords.end(); ++cit, ++nit, ++tit, ++pit, ++trit) {
                    // Colors
                    colorBuf[colorIndex + 0] = color.getR();
                    colorBuf[colorIndex + 1] = color.getG();
                    colorBuf[colorIndex + 2] = color.getB();
                    colorBuf[colorIndex + 3] = color.getA();
                    colorIndex += 4;

                    // Coords
                    const cglib::vec3<double>& pos = **cit;
                    coordBuf[coordIndex + 0] = static_cast<float>(pos(0) - cameraPos(0));
                    coordBuf[coordIndex + 1] = static_cast<float>(pos(1) - cameraPos(1));
                    coordBuf[coordIndex + 2] = static_cast<float>(pos(2) - cameraPos(2));
                    coordIndex += 3;

                    // Normals
                    const cglib::vec4<float>& normal = *nit;
                    normalBuf[normalIndex + 0] = normal(0) * normalScale;
                    normalBuf[normalIndex + 1] = normal(1) * normalScale;
                    normalBuf[normalIndex + 2] = normal(2) * normalScale;
                    normalBuf[normalIndex + 3] = normal(3);
                    normalIndex += 4;

                    // Tex coords
                    const cglib::vec2<float>& texCoord = *tit;
                    texCoordBuf[texCoordIndex + 0] = texCoord(0);
                    texCoordBuf[texCoordIndex + 1] = texCoord(1) * texCoordYScale;
                    texCoordIndex += 2;

                    // Progresses
                    progressBuf[progressIndex] = *pit;
                    progressIndex += 1;

                    // Traffic
                    trafficBuf[trafficIndex] = (float) *trit;
//                    Log::Errorf("Traffic %d: %f", trafficIndex, trafficBuf[trafficIndex]);
                    trafficIndex += 1;
                }
            }
        }

        // Draw the final batch
        if (indexIndex > 0) {
            glVertexAttribPointer(a_color, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, colorBuf.data());
            glVertexAttribPointer(a_coord, 3, GL_FLOAT, GL_FALSE, 0, coordBuf.data());
            glVertexAttribPointer(a_normal, 4, GL_FLOAT, GL_FALSE, 0, normalBuf.data());
            glVertexAttribPointer(a_texCoord, 2, GL_FLOAT, GL_FALSE, 0, texCoordBuf.data());
            glVertexAttribPointer(a_progress, 1, GL_FLOAT, GL_FALSE, 0, progressBuf.data());
            glVertexAttribPointer(a_traffic, 1, GL_FLOAT, GL_FALSE, 0, trafficBuf.data());
            glDrawElements(GL_TRIANGLES, indexIndex, GL_UNSIGNED_SHORT, indexBuf.data());
        }
    }

    bool CustomLineRenderer::FindElementRayIntersection(const std::shared_ptr<VectorElement>& element,
                                                        const std::shared_ptr<CustomLineDrawData>& drawData,
                                                        const std::shared_ptr<VectorLayer>& layer,
                                                        const cglib::ray3<double>& ray,
                                                        const ViewState& viewState,
                                                        std::vector<RayIntersectedElement>& results)
    {
        std::vector<cglib::vec3<double> > worldCoords;

        for (std::size_t i = 0; i < drawData->getCoords().size(); i++) {
            // Resize the buffer for calculated world coordinates
            const std::vector<cglib::vec3<double>*>& coords = drawData->getCoords()[i];
            worldCoords.clear();
            worldCoords.reserve(coords.size());

            // Calculate world coordinates and bounding box
            cglib::bbox3<double> bounds = cglib::bbox3<double>::smallest();
            const std::vector<cglib::vec4<float> >& normals = drawData->getNormals()[i];
            auto cit = coords.begin();
            auto nit = normals.begin();
            for ( ; cit != coords.end() && nit != normals.end(); ++cit, ++nit) {
                const cglib::vec3<double>& pos = **cit;
                const cglib::vec4<float>& normal = *nit;
                cglib::vec3<double> worldCoord = pos + cglib::vec3<double>(normal(0) * normal(3), normal(1) * normal(3), normal(2) * normal(3)) * static_cast<double>(viewState.getUnitToDPCoef() * drawData->getClickScale());
                bounds.add(worldCoord);
                worldCoords.push_back(worldCoord);
            }

            // Bounding box check
            if (!cglib::intersect_bbox(bounds, ray)) {
                continue;
            }

            // Click test
            const std::vector<unsigned int>& indices = drawData->getIndices()[i];
            const cglib::vec3<double>* prevPos = nullptr;
            for (std::size_t i = 0; i < indices.size(); i += 3) {
                // Figure out the start and end point of the current line segment
                const cglib::vec3<double>* pos = prevPos;
                for (std::size_t j = 0; j < 3; j++) {
                    const cglib::vec3<double>* nextPos = coords[indices[i + j]];
                    if (nextPos != pos) {
                        prevPos = pos;
                        pos = nextPos;
                    }
                }
                if (!pos || !prevPos) {
                    continue;
                }

                // Test a line triangle against the click position
                double t = 0;
                if (cglib::intersect_triangle(worldCoords[indices[i + 0]], worldCoords[indices[i + 1]], worldCoords[indices[i + 2]], ray, &t)) {
                    cglib::vec3<double> dp = ray(t) - *prevPos;
                    cglib::vec3<double> ds = *pos - *prevPos;
                    cglib::vec3<double> pos = *prevPos + ds * std::max(0.0, std::min(1.0, cglib::dot_product(dp, ds) / cglib::norm(ds)));
                    results.push_back(RayIntersectedElement(std::static_pointer_cast<VectorElement>(element), layer, ray(t), pos, layer->isZBuffering()));
                    return true;
                }
            }
        }
        return false;
    }

    bool CustomLineRenderer::initializeRenderer() {
        if (_shader && _shader->isValid() && _textureCache && _textureCache->isValid()) {
            return true;
        }

        if (auto mapRenderer = _mapRenderer.lock()) {
            _textureCache = mapRenderer->getGLResourceManager()->create<BitmapTextureCache>(TEXTURE_CACHE_SIZE);

            _shader = mapRenderer->getGLResourceManager()->create<Shader>("line", LINE_VERTEX_SHADER, LINE_FRAGMENT_SHADER);

            // Get shader variables locations
            _a_color = _shader->getAttribLoc("a_color");
            _a_coord = _shader->getAttribLoc("a_coord");
            _a_normal = _shader->getAttribLoc("a_normal");
            _a_texCoord = _shader->getAttribLoc("a_texCoord");
            _a_progress = _shader->getAttribLoc("a_progress");
            _a_traffic = _shader->getAttribLoc("a_traffic");
            _u_gamma = _shader->getUniformLoc("u_gamma");
            _u_dpToPX = _shader->getUniformLoc("u_dpToPX");
            _u_unitToDP = _shader->getUniformLoc("u_unitToDP");
            _u_mvpMat = _shader->getUniformLoc("u_mvpMat");
            _u_tex_before = _shader->getUniformLoc("u_tex_before");
            _u_tex_after = _shader->getUniformLoc("u_tex_after");
            _u_progress = _shader->getUniformLoc("u_progress");
            _u_traffic_color_0 = _shader->getUniformLoc("u_traffic_colors[0]");
            _u_gradientDistance = _shader->getUniformLoc("u_gradientDistance");
            _u_beforeColor = _shader->getUniformLoc("u_beforeColor");
            _u_afterColor = _shader->getUniformLoc("u_afterColor");
        }

        return _shader && _shader->isValid() && _textureCache && _textureCache->isValid();
    }

    void CustomLineRenderer::bind(const ViewState& viewState) {
        // Prepare for drawing
        glUseProgram(_shader->getProgId());
        // Coords, texCoords, colors
        glEnableVertexAttribArray(_a_color);
        glEnableVertexAttribArray(_a_coord);
        glEnableVertexAttribArray(_a_normal);
        glEnableVertexAttribArray(_a_texCoord);
        glEnableVertexAttribArray(_a_progress);
        glEnableVertexAttribArray(_a_traffic);
        // Scale, gamma
        glUniform1f(_u_gamma, 0.5f);
        glUniform1f(_u_dpToPX, viewState.getDPToPX());
        glUniform1f(_u_unitToDP, viewState.getUnitToDPCoef());
        glUniform1f(_u_progress, 0.0f);
        glUniform1f(_u_gradientDistance, 0.0f);

        float color1[4] = {0.2, 0, 0, 1};
        float color2[4] = {0.0, 0.7, 0, 1};
        float color3[4] = {0.0, 0, 0.8, 1};

        glUniform4fv(_u_traffic_color_0, 0, color1);
        glUniform4fv(_u_traffic_color_0, 1, color2);
        glUniform4fv(_u_traffic_color_0, 2, color3);
        // Matrix
        const cglib::mat4x4<float>& mvpMat = viewState.getRTEModelviewProjectionMat();
        glUniformMatrix4fv(_u_mvpMat, 1, GL_FALSE, mvpMat.data());
        // Texture
        glUniform1i(_u_tex_before, 0);
        glUniform1i(_u_tex_after, 1);
    }

    void CustomLineRenderer::unbind() {
        // Disable bound arrays
        glDisableVertexAttribArray(_a_color);
        glDisableVertexAttribArray(_a_coord);
        glDisableVertexAttribArray(_a_normal);
        glDisableVertexAttribArray(_a_texCoord);
        glDisableVertexAttribArray(_a_progress);
        glDisableVertexAttribArray(_a_traffic);
    }

    bool CustomLineRenderer::isEmptyBatch() const {
        return _drawDataBuffer.empty();
    }

    void CustomLineRenderer::addToBatch(const std::shared_ptr<CustomLine>& customLine, const ViewState& viewState) {
        _lineDrawDataBuffer.push_back(customLine->getDrawData().get());
        _drawDataBuffer.push_back(std::move(customLine->getDrawData()));
        _progress = customLine->getProgress();
        const Color& beforeColor = customLine->getStyle()->getBeforeColor();
        glUniform4f(_u_beforeColor, beforeColor.getR() / 255.0f, beforeColor.getG() / 255.0f, beforeColor.getB() / 255.0f, beforeColor.getA()/ 255.0f);
        const Color& afterColor = customLine->getStyle()->getAfterColor();
        glUniform4f(_u_afterColor, afterColor.getR() / 255.0f, afterColor.getG() / 255.0f, afterColor.getB() / 255.0f, afterColor.getA()/ 255.0f);
        glUniform1f(_u_gradientDistance, customLine->getDrawData()->getGradientWidth());
    }

    void CustomLineRenderer::drawBatch(const ViewState& viewState) {
        if (_lineDrawDataBuffer.empty()) {
            return;
        }

        const float progress = _lineDrawDataBuffer.front()->getCurrentProgress();

        // Bind texture
        const std::shared_ptr<Bitmap>& beforeBitmap = _lineDrawDataBuffer.front()->getBeforeBitmap();
        std::shared_ptr<Texture> beforeTexture = _textureCache->get(beforeBitmap);
        if (!beforeTexture) {
            beforeTexture = _textureCache->create(beforeBitmap, true, true);
        }
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, beforeTexture->getTexId());

        // Bind texture
        const std::shared_ptr<Bitmap>& afterBitmap = _lineDrawDataBuffer.front()->getAfterBitmap();
        std::shared_ptr<Texture> afterTexture = _textureCache->get(afterBitmap);
        if (!afterTexture) {
            afterTexture = _textureCache->create(afterBitmap, true, true);
        }
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, afterTexture->getTexId());
        glActiveTexture(GL_TEXTURE0);

        glUniform1f(_u_progress, _progress);

        BuildAndDrawBuffers(_a_color, _a_coord, _a_normal, _a_texCoord, _a_progress, _a_traffic, _colorBuf, _coordBuf, _normalBuf,_texCoordBuf, _indexBuf, _progressBuf, _trafficBuf, _lineDrawDataBuffer, viewState);

        _lineDrawDataBuffer.clear();
        _drawDataBuffer.clear();
        _progress = -1;
    }


    const std::string CustomLineRenderer::LINE_VERTEX_SHADER = R"GLSL(
        #version 100
        attribute vec3 a_coord;
        attribute vec4 a_normal;
        attribute vec2 a_texCoord;
        attribute vec4 a_color;
        attribute float a_traffic;
        attribute float a_progress;
        uniform float u_gamma;
        uniform float u_dpToPX;
        uniform float u_unitToDP;
        uniform vec4 u_traffic_colors[3];
        uniform mat4 u_mvpMat;
        varying lowp vec4 v_color;
        varying vec2 v_texCoord;
        varying float v_dist;
        varying float v_width;
        varying float v_progress;
        varying float v_traffic;

        void main() {
            float width = length(a_normal.xyz) * u_dpToPX;
            float roundedWidth = width + 1.0;
            vec3 pos = a_coord + u_unitToDP * roundedWidth / width * (a_normal.xyz * a_normal.w);
            v_color = a_color;
            v_texCoord = a_texCoord;
            v_progress = a_progress;
            v_traffic = a_traffic;
            v_dist = a_normal.w * roundedWidth * u_gamma;
            v_width = 1.0 + (width - 1.0) * u_gamma;
            gl_Position = u_mvpMat * vec4(pos, 1.0);
        }
    )GLSL";

    const std::string CustomLineRenderer::LINE_FRAGMENT_SHADER = R"GLSL(
        #version 100
        precision mediump float;
        uniform highp float u_progress;
        uniform highp float u_gradientDistance;
        uniform sampler2D u_tex_before;
        uniform sampler2D u_tex_after;
        uniform vec4 u_beforeColor;
        uniform vec4 u_afterColor;
        uniform vec4 u_traffic_colors[3];
        varying lowp vec4 v_color;
        varying float v_traffic;
        #ifdef GL_FRAGMENT_PRECISION_HIGH
        varying highp vec2 v_texCoord;
        varying highp float v_dist;
        varying highp float v_width;
        varying highp float v_progress;
        #else
        varying mediump vec2 v_texCoord;
        varying mediump float v_dist;
        varying mediump float v_width;
        varying mediump float v_progress;
        #endif
        void main() {
            lowp float a = clamp(v_width - abs(v_dist), 0.0, 1.0);

            vec4 beforeColor = texture2D(u_tex_before, v_texCoord) * u_beforeColor * v_color * a;
            vec4 afterColor = texture2D(u_tex_after, v_texCoord) * u_afterColor * v_color * a;

            if (v_traffic >= 0.0 && int(mod(v_traffic * 10000.0, 10000.0)) == 0) {
                if (v_traffic < 1.0) {
                    afterColor = vec4(1.0, 0.0, 0.0, 1.0);
                } else if (v_traffic < 2.0) {
                    afterColor = vec4(0.0, 1.0, 0.0, 1.0);
                } else if (v_traffic < 3.0) {
                    afterColor = vec4(0.0, 0.0, 1.0, 1.0);
                } else {
                    afterColor = u_traffic_colors[int(v_traffic)];
                }
            }

            float beforeCoef = (u_progress + u_gradientDistance - v_progress) /  (2.0 * u_gradientDistance);
            float afterCoef = (u_gradientDistance - u_progress + v_progress) /  (2.0 * u_gradientDistance);

            beforeCoef = clamp(beforeCoef, 0.0, 1.0);
            afterCoef = clamp(afterCoef, 0.0, 1.0);

            gl_FragColor = (beforeColor * beforeCoef) + (afterColor * afterCoef);
        }
    )GLSL";

    const unsigned int CustomLineRenderer::TEXTURE_CACHE_SIZE = 4 * 1024 * 1024;
}
