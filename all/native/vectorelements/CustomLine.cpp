#include "CustomLine.h"
#include "components/Exceptions.h"
#include "datasources/VectorDataSource.h"
#include "geometry/LineGeometry.h"
#include "renderers/drawdatas/CustomLineDrawData.h"
#include "utils/Log.h"

namespace carto {

    CustomLine::CustomLine(const std::shared_ptr<LineGeometry>& geometry, const std::shared_ptr<CustomLineStyle>& style) :
        VectorElement(geometry),
        _traffics(std::make_shared<std::vector<int> >()),
        _style(style),
        _progress(0)
    {
        if (!geometry) {
            throw NullArgumentException("Null geometry");
        }
        if (!style) {
            throw NullArgumentException("Null style");
        }

        _traffics->push_back(0);
        _traffics->push_back(1);
        _traffics->push_back(2);
    }
        
    CustomLine::CustomLine(std::vector<MapPos> poses, const std::shared_ptr<CustomLineStyle>& style) :
        VectorElement(std::make_shared<LineGeometry>(std::move(poses))),
        _traffics(std::make_shared<std::vector<int> >()),
        _style(style),
        _progress(0)
    {
        if (!style) {
            throw NullArgumentException("Null style");
        }

        _traffics->push_back(0);
        _traffics->push_back(1);
        _traffics->push_back(2);
    }
    
    CustomLine::~CustomLine() {
    }
    
    std::shared_ptr<LineGeometry> CustomLine::getGeometry() const {
        std::lock_guard<std::recursive_mutex> lock(_mutex);
        return std::static_pointer_cast<LineGeometry>(_geometry);
    }
    
    void CustomLine::setGeometry(const std::shared_ptr<LineGeometry>& geometry) {
        if (!geometry) {
            throw NullArgumentException("Null geometry");
        }

        {
            std::lock_guard<std::recursive_mutex> lock(_mutex);
            _geometry = geometry;
        }
        notifyElementChanged();
    }
    
    std::vector<MapPos> CustomLine::getPoses() const {
        return getGeometry()->getPoses();
    }
        
    void CustomLine::setPoses(const std::vector<MapPos>& poses) {
        {
            std::lock_guard<std::recursive_mutex> lock(_mutex);
            _geometry = std::make_shared<LineGeometry>(poses);
        }
        notifyElementChanged();
    
    }

    std::vector<int> CustomLine::getTraffics() const {
        return *_traffics;
    }

    void CustomLine::setTraffics(std::vector<int> traffics) {
        {
            std::lock_guard<std::recursive_mutex> lock(_mutex);
            _traffics = std::make_shared<std::vector<int> >(std::move(traffics));
        }
        notifyElementChanged();

    }
    
    std::shared_ptr<CustomLineStyle> CustomLine::getStyle() const {
        std::lock_guard<std::recursive_mutex> lock(_mutex);
        return _style;
    }
    
    void CustomLine::setStyle(const std::shared_ptr<CustomLineStyle>& style) {
        if (!style) {
            throw NullArgumentException("Null style");
        }

        {
            std::lock_guard<std::recursive_mutex> lock(_mutex);
            _style = style;
        }
        notifyElementChanged();
    }

    float CustomLine::getProgress() const {
        std::lock_guard<std::recursive_mutex> lock(_mutex);
        return _progress;
    }
    
    void CustomLine::setProgress(float progress) {
        std::lock_guard<std::recursive_mutex> lock(_mutex);
        _progress = progress;
    }
        
    std::shared_ptr<CustomLineDrawData> CustomLine::getDrawData() const {
        std::lock_guard<std::recursive_mutex> lock(_mutex);
        return _drawData;
    }
        
    void CustomLine::setDrawData(const std::shared_ptr<CustomLineDrawData>& drawData) {
        std::lock_guard<std::recursive_mutex> lock(_mutex);
        _drawData = drawData;
    }
    
}
