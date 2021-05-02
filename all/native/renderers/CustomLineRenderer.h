/*
 * Copyright (c) 2016 CartoDB. All rights reserved.
 * Copying and using this code is allowed only according
 * to license terms, as given in https://cartodb.com/terms/
 */

#ifndef _CARTO_CUSTOMLINERENDERER_H_
#define _CARTO_CUSTOMLINERENDERER_H_

#include "renderers/utils/GLContext.h"
#include "renderers/utils/BitmapTextureCache.h"

#include <deque>
#include <memory>
#include <mutex>
#include <vector>

#include <cglib/ray.h>

namespace carto {
    class Bitmap;
    class CustomLine;
    class CustomLineDrawData;
    class Options;
    class MapRenderer;
    class Shader;
    class RayIntersectedElement;
    class VectorLayer;
    class VectorElement;
    class ViewState;

    class CustomLineRenderer {
    public:
        CustomLineRenderer();
        virtual ~CustomLineRenderer();

        void setComponents(const std::weak_ptr<Options>& options, const std::weak_ptr<MapRenderer>& mapRenderer);

        void offsetLayerHorizontally(double offset);

        void onDrawFrame(float deltaSeconds, const ViewState& viewState);

        void addElement(const std::shared_ptr<CustomLine>& element);
        void refreshElements();
        void updateElement(const std::shared_ptr<CustomLine>& element);
        void removeElement(const std::shared_ptr<CustomLine>& element);

        void calculateRayIntersectedElements(const std::shared_ptr<VectorLayer>& layer, const cglib::ray3<double>& ray, const ViewState& viewState, std::vector<RayIntersectedElement>& results) const;

    protected:
        friend class PolygonRenderer;
        friend class GeometryCollectionRenderer;

    private:
        static void BuildAndDrawBuffers(GLuint a_color,
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
                                        const ViewState& viewState);

        static bool FindElementRayIntersection(const std::shared_ptr<VectorElement>& element,
                                               const std::shared_ptr<CustomLineDrawData>& drawData,
                                               const std::shared_ptr<VectorLayer>& layer,
                                               const cglib::ray3<double>& ray,
                                               const ViewState& viewState,
                                               std::vector<RayIntersectedElement>& results);

        bool initializeRenderer();
        void bind(const ViewState& viewState);
        void unbind();

        bool isEmptyBatch() const;
        void addToBatch(const std::shared_ptr<CustomLine>& customLine, const ViewState& viewState);
        void drawBatch(const ViewState& viewState);

        static const std::string LINE_VERTEX_SHADER;
        static const std::string LINE_FRAGMENT_SHADER;

        static const unsigned int TEXTURE_CACHE_SIZE;

        std::weak_ptr<MapRenderer> _mapRenderer;

        std::vector<std::shared_ptr<CustomLine> > _elements;
        std::vector<std::shared_ptr<CustomLine> > _tempElements;

        std::vector<std::shared_ptr<CustomLineDrawData> > _drawDataBuffer; // this buffer is used to keep objects alive
        std::vector<const CustomLineDrawData*> _lineDrawDataBuffer;
        float _progress;

        std::vector<unsigned char> _colorBuf;
        std::vector<float> _coordBuf;
        std::vector<float> _normalBuf;
        std::vector<float> _texCoordBuf;
        std::vector<unsigned short> _indexBuf;
        std::vector<float> _progressBuf;
        std::vector<float> _trafficBuf;

        std::shared_ptr<BitmapTextureCache> _textureCache;
        std::shared_ptr<Shader> _shader;
        GLuint _a_color;
        GLuint _a_coord;
        GLuint _a_normal;
        GLuint _a_texCoord;
        GLuint _a_progress;
        GLuint _a_traffic;
        GLuint _u_gamma;
        GLuint _u_dpToPX;
        GLuint _u_unitToDP;
        GLuint _u_mvpMat;
        GLuint _u_tex_before;
        GLuint _u_progress;
        GLuint _u_gradientDistance;
        GLuint _u_beforeColor;
        GLuint _u_afterColor;
        GLuint _u_night;
        GLuint _u_traffic_color;

        mutable std::mutex _mutex;
    };

}

#endif
