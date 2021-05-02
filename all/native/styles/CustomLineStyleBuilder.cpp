#include "CustomLineStyleBuilder.h"
#include "assets/DefaultLinePNG.h"
#include "components/Exceptions.h"
#include "graphics/Bitmap.h"

namespace carto {

    CustomLineStyleBuilder::CustomLineStyleBuilder() :
        StyleBuilder(),
        _beforeBitmap(GetDefaultBitmap()),
        _afterBitmap(GetDefaultBitmap()),
        _beforeColor(0xFFFFFFFF),
        _afterColor(0xFFFFFFFF),
        _lightTrafficColor(0xFFFF992B),
        _casualTrafficColor(0xFFFF001D),
        _heavyTrafficColor(0xFFBF000D),
        _isNight(false),
        _clickWidth(-1),
        _lineEndType(CustomLineEndType::LINE_END_TYPE_ROUND),
        _lineJoinType(CustomLineJoinType::LINE_JOIN_TYPE_MITER),
        _stretchFactor(1),
        _width(12),
        _gradientWidth(0)
    {
    }
    
    CustomLineStyleBuilder::~CustomLineStyleBuilder() {
    }
        
    std::shared_ptr<Bitmap> CustomLineStyleBuilder::getBeforeBitmap() const {
        std::lock_guard<std::mutex> lock(_mutex);
        return _beforeBitmap;
    }
    
    void CustomLineStyleBuilder::setBeforeBitmap(const std::shared_ptr<Bitmap>& bitmap) {
        if (!bitmap) {
            throw NullArgumentException("Null bitmap");
        }

        std::lock_guard<std::mutex> lock(_mutex);
        _beforeBitmap = bitmap;
    }
        
    std::shared_ptr<Bitmap> CustomLineStyleBuilder::getAfterBitmap() const {
        std::lock_guard<std::mutex> lock(_mutex);
        return _afterBitmap;
    }
    
    void CustomLineStyleBuilder::setAfterBitmap(const std::shared_ptr<Bitmap>& bitmap) {
        if (!bitmap) {
            throw NullArgumentException("Null bitmap");
        }

        std::lock_guard<std::mutex> lock(_mutex);
        _afterBitmap = bitmap;
    }
    
    Color CustomLineStyleBuilder::getBeforeColor() const {
        std::lock_guard<std::mutex> lock(_mutex);
        return _beforeColor;
    }
    
    void CustomLineStyleBuilder::setBeforeColor(const Color& color) {
        std::lock_guard<std::mutex> lock(_mutex);
        _beforeColor = color;
    }
    
    Color CustomLineStyleBuilder::getAfterColor() const {
        std::lock_guard<std::mutex> lock(_mutex);
        return _afterColor;
    }
    
    void CustomLineStyleBuilder::setAfterColor(const Color& color) {
        std::lock_guard<std::mutex> lock(_mutex);
        _afterColor = color;
    }

    Color CustomLineStyleBuilder::getLightTrafficColor() const {
        std::lock_guard<std::mutex> lock(_mutex);
        return _lightTrafficColor;
    }

    void CustomLineStyleBuilder::setLightTrafficColor(const Color& color) {
        std::lock_guard<std::mutex> lock(_mutex);
        _lightTrafficColor = color;
    }

    Color CustomLineStyleBuilder::getCasualTrafficColor() const {
        std::lock_guard<std::mutex> lock(_mutex);
        return _casualTrafficColor;
    }

    void CustomLineStyleBuilder::setCasualTrafficColor(const Color& color) {
        std::lock_guard<std::mutex> lock(_mutex);
        _casualTrafficColor = color;
    }

    Color CustomLineStyleBuilder::getHeavyTrafficColor() const {
        std::lock_guard<std::mutex> lock(_mutex);
        return _heavyTrafficColor;
    }

    void CustomLineStyleBuilder::setHeavyTrafficColor(const Color& color) {
        std::lock_guard<std::mutex> lock(_mutex);
        _heavyTrafficColor = color;
    }
        
    bool CustomLineStyleBuilder::isNight() const {
        std::lock_guard<std::mutex> lock(_mutex);
        return _isNight;
    }
    
    void CustomLineStyleBuilder::setNight(bool isNight) {
        std::lock_guard<std::mutex> lock(_mutex);
        _isNight = isNight;
    }

    float CustomLineStyleBuilder::getClickWidth() const {
        std::lock_guard<std::mutex> lock(_mutex);
        return _clickWidth;
    }

    void CustomLineStyleBuilder::setClickWidth(float clickWidth) {
        std::lock_guard<std::mutex> lock(_mutex);
        _clickWidth = clickWidth;
    }
        
    CustomLineEndType::CustomLineEndType CustomLineStyleBuilder::getLineEndType() {
        std::lock_guard<std::mutex> lock(_mutex);
        return _lineEndType;
    }
    
    void CustomLineStyleBuilder::setLineEndType(CustomLineEndType::CustomLineEndType lineEndType) {
        std::lock_guard<std::mutex> lock(_mutex);
        _lineEndType = lineEndType;
    }
        
    CustomLineJoinType::CustomLineJoinType CustomLineStyleBuilder::getLineJoinType() {
        std::lock_guard<std::mutex> lock(_mutex);
        return _lineJoinType;
    }
    
    void CustomLineStyleBuilder::setLineJoinType(CustomLineJoinType::CustomLineJoinType lineJoinType) {
        std::lock_guard<std::mutex> lock(_mutex);
        _lineJoinType = lineJoinType;
    }
        
    float CustomLineStyleBuilder::getStretchFactor() {
        std::lock_guard<std::mutex> lock(_mutex);
        return _stretchFactor;
    }
    
    void CustomLineStyleBuilder::setStretchFactor(float stretch) {
        std::lock_guard<std::mutex> lock(_mutex);
        _stretchFactor = stretch;
    }
        
    float CustomLineStyleBuilder::getWidth() {
        std::lock_guard<std::mutex> lock(_mutex);
        return _width;
    }
    
    void CustomLineStyleBuilder::setWidth(float width) {
        std::lock_guard<std::mutex> lock(_mutex);
        _width = width;
    }  

    float CustomLineStyleBuilder::getGradientWidth() {
        std::lock_guard<std::mutex> lock(_mutex);
        return _gradientWidth;
    }
    
    void CustomLineStyleBuilder::setGradientWidth(float gradientWidth) {
        std::lock_guard<std::mutex> lock(_mutex);
        _gradientWidth = gradientWidth;
    }
    
    std::shared_ptr<CustomLineStyle> CustomLineStyleBuilder::buildStyle() const {
        std::lock_guard<std::mutex> lock(_mutex);
        return std::make_shared<CustomLineStyle>(_isNight, _color, _beforeBitmap, _afterBitmap, _beforeColor, _afterColor, _lightTrafficColor, _casualTrafficColor, _heavyTrafficColor, _clickWidth, _lineEndType, _lineJoinType,
                _stretchFactor, _width, _gradientWidth);
    }
    
    std::shared_ptr<Bitmap> CustomLineStyleBuilder::GetDefaultBitmap() {
        std::lock_guard<std::mutex> lock(_DefaultBitmapMutex);
        if (!_DefaultBitmap) {
            _DefaultBitmap = Bitmap::CreateFromCompressed(default_line_png, default_line_png_len);
        }
        return _DefaultBitmap;
    }
    
    std::shared_ptr<Bitmap> CustomLineStyleBuilder::_DefaultBitmap;
    std::mutex CustomLineStyleBuilder::_DefaultBitmapMutex;
    
}
