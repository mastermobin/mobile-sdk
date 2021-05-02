/*
 * Copyright (c) 2016 CartoDB. All rights reserved.
 * Copying and using this code is allowed only according
 * to license terms, as given in https://cartodb.com/terms/
 */

#ifndef _CARTO_CUSTOMLINESTYLEBUILDER_H_
#define _CARTO_CUSTOMLINESTYLEBUILDER_H_

#include "styles/CustomLineStyle.h"
#include "styles/StyleBuilder.h"

#include <memory>
#include <mutex>

namespace carto {
    class Bitmap;
    class CustomLineStyle;
    
    /**
     * A builder class for LineStyle.
     */
    class CustomLineStyleBuilder : public StyleBuilder {
    public:
        /**
         * Constructs a LineStyleBuilder object with all parameters set to defaults.
         */
        CustomLineStyleBuilder();
        virtual ~CustomLineStyleBuilder();
        
        /**
         * Returns the bitmap of the line in first section of line.
         * @return The bitmap of the line in first section of line.
         */
        std::shared_ptr<Bitmap> getBeforeBitmap() const;
        /**
         * Sets the bitmap that will be used for drawing the line in first section of line. The bitmap will be stretched
         * horizontally to match the width of the line and repeated vertically along the lengtht of the line.
         * The default is "default_line.png".
         * @param bitmap The new bitmap for the line in first section of line.
         */
        void setBeforeBitmap(const std::shared_ptr<Bitmap>& bitmap);

        /**
         * Returns the bitmap of the line in last section of line.
         * @return The bitmap of the line in last section of line.
         */
        std::shared_ptr<Bitmap> getAfterBitmap() const;
        /**
         * Sets the bitmap that will be used for drawing the line in last section of line. The bitmap will be stretched
         * horizontally to match the width of the line and repeated vertically along the lengtht of the line.
         * The default is "default_line.png".
         * @param bitmap The new bitmap for the line in last section of line.
         */
        void setAfterBitmap(const std::shared_ptr<Bitmap>& bitmap);
        
        /**
         * Returns the color of the vector element in first section of line.
         * @return The color of the vector element in first section of line.
         */
        Color getBeforeColor() const;
        /**
         * Sets the color for the vector element in first section of line. Coloring works by multiplying the bitmap
         * of the vector element with the specified color. If the color is set to white, the resulting bitmap
         * will look exactly like the original. The default is white.
         * @param color The new color for the vector element in first section of line.
         */
        void setBeforeColor(const Color& color);
        
        /**
         * Returns the color of the vector element in last section of line.
         * @return The color of the vector element in last section of line.
         */
        Color getAfterColor() const;
        /**
         * Sets the color for the vector element in last section of line. Coloring works by multiplying the bitmap
         * of the vector element with the specified color. If the color is set to white, the resulting bitmap
         * will look exactly like the original. The default is white.
         * @param color The new color for the vector element in last section of line.
         */
        void setAfterColor(const Color& color);

        /**
         * Returns the color of the vector element in last section of line.
         * @return The color of the vector element in last section of line.
         */
        Color getLightTrafficColor() const;
        /**
         * Sets the color for the vector element in last section of line. Coloring works by multiplying the bitmap
         * of the vector element with the specified color. If the color is set to white, the resulting bitmap
         * will look exactly like the original. The default is white.
         * @param color The new color for the vector element in last section of line.
         */
        void setLightTrafficColor(const Color& color);

        /**
         * Returns the color of the vector element in last section of line.
         * @return The color of the vector element in last section of line.
         */
        Color getCasualTrafficColor() const;
        /**
         * Sets the color for the vector element in last section of line. Coloring works by multiplying the bitmap
         * of the vector element with the specified color. If the color is set to white, the resulting bitmap
         * will look exactly like the original. The default is white.
         * @param color The new color for the vector element in last section of line.
         */
        void setCasualTrafficColor(const Color& color);

        /**
         * Returns the color of the vector element in last section of line.
         * @return The color of the vector element in last section of line.
         */
        Color getHeavyTrafficColor() const;
        /**
         * Sets the color for the vector element in last section of line. Coloring works by multiplying the bitmap
         * of the vector element with the specified color. If the color is set to white, the resulting bitmap
         * will look exactly like the original. The default is white.
         * @param color The new color for the vector element in last section of line.
         */
        void setHeavyTrafficColor(const Color& color);
        
        /**
         * Returns the width of the line used for click detection.
         * @return The width of the line used for click detection.
         */
        bool isNight() const;
        /**
         * Sets the width for the line that will be used for click detection.
         * Units are screen density independent pixels (DP or DIP). If set to -1 the click width will be
         * calculated automatically. The default is -1.
         * @param clickWidth The new point size in dp.
         */
        void setNight(bool isNight);

        /**
         * Returns the width of the line used for click detection.
         * @return The width of the line used for click detection.
         */
        float getClickWidth() const;
        /**
         * Sets the width for the line that will be used for click detection.
         * Units are screen density independent pixels (DP or DIP). If set to -1 the click width will be
         * calculated automatically. The default is -1.
         * @param clickWidth The new point size in dp.
         */
        void setClickWidth(float clickWidth);
        
        /**
         * Returns the end point type of the line.
         * @return The end point type of the line.
         */
        CustomLineEndType::CustomLineEndType getLineEndType();
        /**
         * Sets the style in which the end points of the line will be drawn. The default is LineEndType::ROUND.
         * @param lineEndType The new line end point type.
         */
        void setLineEndType(CustomLineEndType::CustomLineEndType lineEndType);
    
        /**
         * Returns the join type of the line.
         * @return The join type of the line.
         */
        CustomLineJoinType::CustomLineJoinType getLineJoinType();
        /**
         * Sets the style in which the line segments will be connected with each other. The default is LineJoinType::MITER.
         * @param lineJoinType The new line join type.
         */
        void setLineJoinType(CustomLineJoinType::CustomLineJoinType lineJoinType);
    
        /**
         * Returns the stretch factor of the line.
         * @return The stretch factor of the line.
         */
        float getStretchFactor();
        /**
         * Sets the relative stretching coefficient for the line. The bitmap of the line will be stretched 
         * vertically by the stretch factor and then repeated along the length of the line. For example, 
         * setting the stretch factor to 2.0 will stretch the bitmap vertically to double the original height 
         * and reduces the number of times the bitmap gets repeated by half. The default is 1.0.
         * @param stretchFactor The new relative stretching coefficient for the line.
         */
        void setStretchFactor(float stretchFactor);
    
        /**
         * Returns the width of the line.
         * @return The width of the line in dp.
         */
        float getWidth();
        /**
         * Sets the width of line in screen density independent pixels (DP or DIP). The default is 12.
         * @param width The new line width in dp.
         */
        void setWidth(float width);

        float getGradientWidth();

        void setGradientWidth(float gradientWidth);
    
        /**
         * Builds a new instance of the LineStyle object using previously set parameters.
         * @return A new LineStyle object.
         */
        std::shared_ptr<CustomLineStyle> buildStyle() const;
    
    protected:
        static std::shared_ptr<Bitmap> GetDefaultBitmap();
    
        static std::shared_ptr<Bitmap> _DefaultBitmap;
        static std::mutex _DefaultBitmapMutex;
    
        std::shared_ptr<Bitmap> _beforeBitmap;
        std::shared_ptr<Bitmap> _afterBitmap;

        Color _beforeColor;
        Color _afterColor;
        Color _lightTrafficColor;
        Color _casualTrafficColor;
        Color _heavyTrafficColor;

        bool _isNight;

        float _clickWidth;
    
        CustomLineEndType::CustomLineEndType _lineEndType;
    
        CustomLineJoinType::CustomLineJoinType _lineJoinType;
    
        float _stretchFactor;
    
        float _width;

        float _gradientWidth;
    };
    
}

#endif
