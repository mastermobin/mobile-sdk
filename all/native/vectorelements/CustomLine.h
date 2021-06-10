/*
 * Copyright (c) 2016 CartoDB. All rights reserved.
 * Copying and using this code is allowed only according
 * to license terms, as given in https://cartodb.com/terms/
 */

#ifndef _CARTO_CUSTOMLINE_H_
#define _CARTO_CUSTOMLINE_H_

#include "vectorelements/VectorElement.h"

#include <vector>

namespace carto {
    class CustomLineDrawData;
    class LineGeometry;
    class CustomLineStyle;
    class MapPos;
    
    /**
     * A geometric line string that can be displayed on the map.
     */
    class CustomLine : public VectorElement {
    public:
        /**
         * Constructs a Line object from a geometry object and a style.
         * @param geometry The geometry object that defines the location of this line.
         * @param style The style that defines what this line looks like.
         */
        CustomLine(const std::shared_ptr<LineGeometry>& geometry, const std::shared_ptr<CustomLineStyle>& style);
        /**
         * Constructs a Line object from a vector of map positions and a style.
         * @param poses The vector of map positions that defines the location of this line.
         * @param style The style that defines what this line looks like.
         */
        CustomLine(std::vector<MapPos> poses, const std::shared_ptr<CustomLineStyle>& style);
        virtual ~CustomLine();
    
        std::shared_ptr<LineGeometry> getGeometry() const;
        /**
         * Sets the location for this line.
         * @param geometry The new geometry object that defines the location of this line.
         */
        void setGeometry(const std::shared_ptr<LineGeometry>& geometry);

        /**
         * Returns the vertices that define this line.
         * @return The vector of map positions that defines this line.
         */
        std::vector<MapPos> getPoses() const;
        /**
         * Sets the vertices that define this line.
         * @param poses The new vector of map positions that defines this line.
         */
        void setPoses(const std::vector<MapPos>& poses);

        /**
         * Returns the traffic data of the line.
         * @return The vector of int(traffic modes).
         */
        std::vector<int> getTraffics() const;
        /**
         * Sets the traffic data of the line.
         * @param traffics The new vector of int(traffic modes). 0 for no color, 1 for light, 2 for casual, 3 for heavy.
         */
        void setTraffics(std::vector<int> traffics);
    
        /** 
         * Returns the style of this line.
         * @return The style that defines what this line looks like.
         */
        std::shared_ptr<CustomLineStyle> getStyle() const;
        /**
         * Sets the style for this line.
         * @param style The new style that defines what this line looks like.
         */
        void setStyle(const std::shared_ptr<CustomLineStyle>& style);

        float getProgress() const;

        void setProgress(float progress);
        
        std::shared_ptr<CustomLineDrawData> getDrawData() const;
        void setDrawData(const std::shared_ptr<CustomLineDrawData>& drawData);

    protected:
        friend class CustomLineRenderer;
        friend class VectorLayer;

    private:
        std::shared_ptr<CustomLineDrawData> _drawData;

        std::shared_ptr<std::vector<int> > _traffics;

        std::shared_ptr<CustomLineStyle> _style;

        float _progress;
    };
    
}

#endif
