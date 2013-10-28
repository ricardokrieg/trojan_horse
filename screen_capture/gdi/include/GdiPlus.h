/**************************************************************************\
*
* Copyright (c) 1998-2001, Microsoft Corp.  All Rights Reserved.
*
* Module Name:
*
*   Gdiplus.h
*
* Abstract:
*
*   GDI+ public header file
*
\**************************************************************************/

#ifndef _GDIPLUS_H
#define _GDIPLUS_H

struct IDirectDrawSurface7;

typedef signed   short   INT16;
typedef unsigned short  UINT16;

#include <pshpack8.h>   // set structure packing to 8

namespace Gdiplus
{
    namespace DllExports
    {
        #include "GdiPlusMem.h"
    };

    #include "GdiPlusBase.h"

    #include "GdiPlusEnums.h"
    #include "GdiPlusTypes.h"
    #include "GdiPlusInit.h"
    #include "GdiPlusPixelFormats.h"
    #include "GdiPlusColor.h"
    #include "GdiPlusMetaHeader.h"
    #include "GdiPlusImaging.h"
    #include "GdiPlusColorMatrix.h"

    #include "GdiPlusGpStubs.h"
    #include "GdiPlusHeaders.h"

    namespace DllExports
    {
        #include "GdiPlusFlat.h"
    };


    #include "GdiPlusimageAttributes.h"
    #include "GdiPlusMatrix.h"
    #include "GdiPlusBrush.h"
    #include "GdiPlusPen.h"
    #include "GdiPlusStringFormat.h"
    #include "GdiPlusPath.h"
    #include "GdiPlusLineCaps.h"
    #include "GdiPlusMetaFile.h"
    #include "GdiPlusGraphics.h"
    #include "GdiPlusCachedBitmap.h"
    #include "GdiPlusRegion.h"
    #include "GdiPlusFontCollection.h"
    #include "GdiPlusFontFamily.h"
    #include "GdiPlusFont.h"
    #include "GdiPlusBitmap.h"
    #include "GdiPlusImageCodec.h"

}; // namespace Gdiplus

#include <poppack.h>    // pop structure packing back to previous state

#endif // !_GDIPLUS_HPP
