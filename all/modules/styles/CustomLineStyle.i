#ifndef _CUSTOMLINESTYLE_I
#define _CUSTOMLINESTYLE_I

%module CustomLineStyle

!proxy_imports(carto::CustomLineStyle, graphics.Bitmap, graphics.Color, styles.Style)

%{
#include "styles/CustomLineStyle.h"
#include <memory>
%}

%include <std_shared_ptr.i>
%include <cartoswig.i>

%import "graphics/Bitmap.i"
%import "styles/Style.i"

!polymorphic_shared_ptr(carto::CustomLineStyle, styles.CustomLineStyle)

%attribute(carto::CustomLineStyle, float, Width, getWidth)
%attribute(carto::CustomLineStyle, float, ClickWidth, getClickWidth)
%attribute(carto::CustomLineStyle, float, StretchFactor, getStretchFactor)
%attribute(carto::CustomLineStyle, float, CurrentProgress, getCurrentProgress)
%attribute(carto::CustomLineStyle, carto::CustomLineJoinType::CustomLineJoinType, LineJoinType, getLineJoinType)
%attribute(carto::CustomLineStyle, carto::CustomLineEndType::CustomLineEndType, LineEndType, getLineEndType)
%attributestring(carto::CustomLineStyle, std::shared_ptr<carto::Bitmap>, Bitmap, getBitmap)
%ignore carto::CustomLineStyle::CustomLineStyle;

%include "styles/CustomLineStyle.h"

#endif
