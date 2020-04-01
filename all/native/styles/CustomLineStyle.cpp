#include "CustomLineStyle.h"

namespace carto {

    CustomLineStyle::CustomLineStyle(const Color& color, const std::shared_ptr<Bitmap>& beforeBitmap, const std::shared_ptr<Bitmap>& afterBitmap,
            const Color& beforeColor, const Color& afterColor, float clickWidth,
            CustomLineEndType::CustomLineEndType lineEndType, CustomLineJoinType::CustomLineJoinType lineJoinType,
            float stretchFactor, float width, float gradientWidth) :
        Style(color),
        _beforeBitmap(beforeBitmap),
        _afterBitmap(afterBitmap),
        _beforeColor(beforeColor),
        _afterColor(afterColor),
        _clickWidth(clickWidth),
        _lineEndType(lineEndType),
        _lineJoinType(lineJoinType),
        _stretchFactor(stretchFactor),
        _width(width),
        _gradientWidth(gradientWidth)
    {
    }
    
    CustomLineStyle::~CustomLineStyle() {
    }
    
    std::shared_ptr<Bitmap> CustomLineStyle::getBeforeBitmap() const {
        return _beforeBitmap;
    }
    
    std::shared_ptr<Bitmap> CustomLineStyle::getAfterBitmap() const {
        return _afterBitmap;
    }

    const Color& CustomLineStyle::getBeforeColor() const {
        return _beforeColor;
    }

    const Color& CustomLineStyle::getAfterColor() const {
        return _afterColor;
    }
        
    float CustomLineStyle::getClickWidth() const {
        return _clickWidth;
    }
    
    CustomLineEndType::CustomLineEndType CustomLineStyle::getLineEndType() const {
        return _lineEndType;
    }
    
    CustomLineJoinType::CustomLineJoinType CustomLineStyle::getLineJoinType() const {
        return _lineJoinType;
    }
    
    float CustomLineStyle::getStretchFactor() const {
        return _stretchFactor;
    }
    
    float CustomLineStyle::getWidth() const {
        return _width;
    }

    float CustomLineStyle::getGradientWidth() const {
        return _gradientWidth;
    }
    
}
