// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2005 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_SWI_PDRIVER
#define _RTK_SWI_PDRIVER

namespace rtk {
namespace swi {

const int PDriver_Info              =0x80140;
const int PDriver_SetInfo           =0x80141;
const int PDriver_CheckFeatures     =0x80142;
const int PDriver_PageSize          =0x80143;
const int PDriver_SetPageSize       =0x80144;
const int PDriver_SelectJob         =0x80145;
const int PDriver_CurrentJob        =0x80146;
const int PDriver_FontSWI           =0x80147;
const int PDriver_EndJob            =0x80148;
const int PDriver_AbortJob          =0x80149;
const int PDriver_Reset             =0x8014A;
const int PDriver_GiveRectangle     =0x8014B;
const int PDriver_DrawPage          =0x8014C;
const int PDriver_GetRectangle      =0x8014D;
const int PDriver_CancelJob         =0x8014E;
const int PDriver_ScreenDump        =0x8014F;
const int PDriver_EnumerateJobs     =0x80150;
const int PDriver_SetPrinter        =0x80151;
const int PDriver_CancelJobWithError=0x80152;
const int PDriver_SelectIllustration=0x80153;
const int PDriver_InsertIllustration=0x80154;
const int PDriver_DeclareFont       =0x80155;
const int PDriver_DeclareDriver     =0x80156;
const int PDriver_RemoveDriver      =0x80157;
const int PDriver_SelectDriver      =0x80158;
const int PDriver_EnumerateDrivers  =0x80159;
const int PDriver_MiscOp            =0x8015A;
const int PDriver_MiscOpForDriver   =0x8015B;
const int PDriver_SetDriver         =0x8015C;
const int PDriver_JPEGSWI           =0x8015D;
const int PDriver_Command           =0x8015E;

} /* namespace swi */
} /* namespace rtk */

#endif
