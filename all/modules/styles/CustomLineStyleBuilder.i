#ifndef _CUSTOMLINESTYLEBUILDER_I
#define _CUSTOMLINESTYLEBUILDER_I

%module CustomLineStyleBuilder

!proxy_imports(carto::CustomLineStyleBuilder, graphics.Bitmap, styles.CustomLineStyle, styles.StyleBuilder, graphics.Color)

%{
#include "styles/CustomLineStyleBuilder.h"
#include "components/Exceptions.h"
#include <memory>
%}

%include <std_shared_ptr.i>
%include <cartoswig.i>

%import "styles/CustomLineStyle.i"
%import "styles/StyleBuilder.i"

!polymorphic_shared_ptr(carto::CustomLineStyleBuilder, styles.CustomLineStyleBuilder)

%attribute(carto::CustomLineStyleBuilder, float, Width, getWidth, setWidth)
%attribute(carto::CustomLineStyleBuilder, float, ClickWidth, getClickWidth, setClickWidth)
%attribute(carto::CustomLineStyleBuilder, float, StretchFactor, getStretchFactor, setStretchFactor)
%attribute(carto::CustomLineStyleBuilder, float, CurrentProgress, getCurrentProgress,setCurrentProgress)
%attribute(carto::CustomLineStyleBuilder, carto::CustomLineJoinType::CustomLineJoinType, LineJoinType, getLineJoinType, setLineJoinType)
%attribute(carto::CustomLineStyleBuilder, carto::CustomLineEndType::CustomLineEndType, LineEndType, getLineEndType, setLineEndType)
%attributestring(carto::CustomLineStyleBuilder, std::shared_ptr<carto::Bitmap>, Bitmap, getBitmap, setBitmap)
%std_exceptions(carto::CustomLineStyleBuilder::setBitmap)

%include "styles/CustomLineStyleBuilder.h"

#endif
