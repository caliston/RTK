// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_SWI_COLOURTRANS
#define _RTK_SWI_COLOURTRANS

namespace rtk {
namespace swi {

const int ColourTrans_SelectTable       =0x40740;
const int ColourTrans_SelectGCOLTable   =0x40741;
const int ColourTrans_ReturnGCOL        =0x40742;
const int ColourTrans_SetGCOL           =0x40743;
const int ColourTrans_ReturnColourNumber=0x40744;
const int ColourTrans_ReturnGCOLForMode =0x40745;
const int ColourTrans_ReturnColourNumberForMode=0x40746;
const int ColourTrans_ReturnOppGCOL     =0x40747;
const int ColourTrans_SetOppGCOL        =0x40748;
const int ColourTrans_ReturnOppColourNumber=0x40749;
const int ColourTrans_ReturnOppGCOLForMode=0x4074A;
const int ColourTrans_ReturnOppColourNumberForMode=0x4074B;
const int ColourTrans_GCOLToColourNumber=0x4074C;
const int ColourTrans_ColourNumberToGCOL=0x4074D;
const int ColourTrans_ReturnFontColours =0x4074E;
const int ColourTrans_SetFontColours    =0x4074F;
const int ColourTrans_InvalidateCache   =0x40750;
const int ColourTrans_SetCalibration    =0x40751;
const int ColourTrans_ReadCalibration   =0x40752;
const int ColourTrans_ConvertDeviceColour=0x40753;
const int ColourTrans_ConvertDevicePalette=0x40754;
const int ColourTrans_ConvertRGBToCIE   =0x40755;
const int ColourTrans_ConvertCIEToRGB   =0x40756;
const int ColourTrans_WriteCalibrationToFile=0x40757;
const int ColourTrans_ConvertRGBToHSV   =0x40758;
const int ColourTrans_ConvertHSVToRGB   =0x40759;
const int ColourTrans_ConvertRGBToCMYK  =0x4075A;
const int ColourTrans_ConvertCMYKToRGB  =0x4075B;
const int ColourTrans_ReadPalette       =0x4075C;
const int ColourTrans_WritePalette      =0x4075D;
const int ColourTrans_SetColour         =0x4075E;
const int ColourTrans_MiscOp            =0x4075F;
const int ColourTrans_WriteLoadingsToFile=0x40760;
const int ColourTrans_SetTextColour     =0x40761;
const int ColourTrans_SetOppTextColour  =0x40762;
const int ColourTrans_GenerateTable     =0x40763;

} /* namespace swi */
} /* namespace rtk */

#endif
