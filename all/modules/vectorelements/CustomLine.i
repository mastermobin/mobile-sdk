#ifndef _CUSTOMLINE_I
#define _CUSTOMLINE_I

%module CustomLine

!proxy_imports(carto::CustomLine, core.IntVector, core.MapPosVector, geometry.LineGeometry, styles.CustomLineStyle, vectorelements.VectorElement)

%{
#include "vectorelements/CustomLine.h"
#include "components/Exceptions.h"
#include <memory>
%}

%include <std_shared_ptr.i>
%include <std_vector.i>
%include <cartoswig.i>

%import "core/IntVector.i"
%import "geometry/LineGeometry.i"
%import "styles/CustomLineStyle.i"
%import "vectorelements/VectorElement.i"

!polymorphic_shared_ptr(carto::CustomLine, vectorelements.CustomLine)

%csmethodmodifiers carto::CustomLine::Geometry "public new";
!attributestring_polymorphic(carto::CustomLine, geometry.LineGeometry, Geometry, getGeometry, setGeometry)
%attributestring(carto::CustomLine, std::shared_ptr<carto::CustomLineStyle>, Style, getStyle, setStyle)
%attributestring(carto::CustomLine, %arg(std::vector<int>), Traffics, getTraffics, setTraffics)
%std_exceptions(carto::CustomLine::CustomLine)
%std_exceptions(carto::CustomLine::setGeometry)
%std_exceptions(carto::CustomLine::setTraffics)
%std_exceptions(carto::CustomLine::setStyle)
%ignore carto::CustomLine::getDrawData;
%ignore carto::CustomLine::setDrawData;

%include "vectorelements/CustomLine.h"

#endif
