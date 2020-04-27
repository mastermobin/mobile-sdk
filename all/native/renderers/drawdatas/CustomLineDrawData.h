/*
 * Copyright (c) 2016 CartoDB. All rights reserved.
 * Copying and using this code is allowed only according
 * to license terms, as given in https://cartodb.com/terms/
 */

#ifndef _CARTO_CUSTOMLINEDRAWDATA_H_
#define _CARTO_CUSTOMLINEDRAWDATA_H_

#include "renderers/drawdatas/VectorElementDrawData.h"

#include <memory>
#include <vector>

#include <cglib/vec.h>

namespace carto {
    class Bitmap;
    class LineGeometry;
    class CustomLineStyle;
    class MapPos;
    class PolygonGeometry;
    class Projection;
    class ProjectionSurface;
    
    class CustomLineDrawData : public VectorElementDrawData {
    public:
        CustomLineDrawData(const LineGeometry& geometry, const CustomLineStyle& style, const Projection& projection, const ProjectionSurface& projectionSurface);
        CustomLineDrawData(const std::vector<MapPos>& poses, const CustomLineStyle& style, const Projection& projection, const ProjectionSurface& projectionSurface);
        virtual ~CustomLineDrawData();
    
        const std::shared_ptr<Bitmap> getBeforeBitmap() const;

        const std::shared_ptr<Bitmap> getAfterBitmap() const;

        float getNormalScale() const;
    
        float getClickScale() const;

        float getCurrentProgress() const;

        float getGradientPercent() const;
    
        const std::vector<std::vector<cglib::vec3<double>*> >& getCoords() const;
    
        const std::vector<std::vector<cglib::vec4<float> > >& getNormals() const;
    
        const std::vector<std::vector<cglib::vec2<float> > >& getTexCoords() const;

        const std::vector<std::vector<float> >& getProgresses() const;
    
        const std::vector<std::vector<unsigned int> >& getIndices() const;

        double calculateDistance(const cglib::vec3<double> a, const cglib::vec3<double> b, const Projection& projection) const;
    
        virtual void offsetHorizontally(double offset);
    
    private:
        static const float LINE_ENDPOINT_TESSELATION_FACTOR;
        static const float LINE_JOIN_TESSELATION_FACTOR;
        static const float LINE_JOIN_MIN_MITER_DOT;
    
        static const int IDEAL_CLICK_WIDTH = 64;
    
        static const float CLICK_WIDTH_COEF;
        
        void init(const std::vector<MapPos>& poses, const Projection& projection, const ProjectionSurface& projectionSurface, const CustomLineStyle& style);
    
        std::shared_ptr<Bitmap> _beforeBitmap;

        std::shared_ptr<Bitmap> _afterBitmap;

        float _currentProgress;
    
        float _normalScale;

        float _clickScale;

        float _gradientPercent;
        float _gradientWidth;

        // Actual line coordinates
        std::vector<cglib::vec3<double> > _poses;
    
        // Origin point and normal for each vertex
        std::vector<std::vector<cglib::vec3<double>*> > _coords;
        std::vector<std::vector<cglib::vec4<float> > > _normals;
        std::vector<std::vector<cglib::vec2<float> > > _texCoords;
        std::vector<std::vector<float> > _progresses;
    
        std::vector<std::vector<unsigned int> > _indices;
    };
    
}

#endif
