// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_SWI_FONT
#define _RTK_SWI_FONT

namespace rtk {
namespace swi {

const int Font_CacheAddr        =0x40080;
const int Font_FindFont         =0x40081;
const int Font_LoseFont         =0x40082;
const int Font_ReadDefn         =0x40083;
const int Font_ReadInfo         =0x40084;
const int Font_StringWidth      =0x40085;
const int Font_Paint            =0x40086;
const int Font_Caret            =0x40087;
const int Font_ConverttoOS      =0x40088;
const int Font_Converttopoints  =0x40089;
const int Font_SetFont          =0x4008A;
const int Font_CurrentFont      =0x4008B;
const int Font_FutureFont       =0x4008C;
const int Font_FindCaret        =0x4008D;
const int Font_CharBBox         =0x4008E;
const int Font_ReadScaleFactor  =0x4008F;
const int Font_SetScaleFactor   =0x40090;
const int Font_ListFonts        =0x40091;
const int Font_SetFontColours   =0x40092;
const int Font_SetPalette       =0x40093;
const int Font_ReadThresholds   =0x40094;
const int Font_SetThresholds    =0x40095;
const int Font_FindCaretJ       =0x40096;
const int Font_StringBBox       =0x40097;
const int Font_ReadColourTable  =0x40098;
const int Font_MakeBitmap       =0x40099;
const int Font_UnCacheFile      =0x4009A;
const int Font_SetFontMax       =0x4009B;
const int Font_ReadFontMax      =0x4009C;
const int Font_ReadFontPrefix   =0x4009D;
const int Font_SwitchOutputToBuffer=0x4009E;
const int Font_ReadFontMetrics  =0x4009F;
const int Font_DecodeMenu       =0x400A0;
const int Font_ScanString       =0x400A1;
const int Font_SetColourTable   =0x400A2;
const int Font_CurrentRGB       =0x400A3;
const int Font_FutureRGB        =0x400A4;
const int Font_ReadEncodingFilename=0x400A5;
const int Font_FindField        =0x400A6;
const int Font_ApplyFields      =0x400A7;
const int Font_LookupFont       =0x400A8;

}; /* namespace swi */
}; /* namespace rtk */

#endif
