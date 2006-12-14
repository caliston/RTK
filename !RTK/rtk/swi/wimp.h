// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_SWI_WIMP
#define _RTK_SWI_WIMP

namespace rtk {
namespace swi {

const int Wimp_Initialise       =0x400C0;
const int Wimp_CreateWindow     =0x400C1;
const int Wimp_CreateIcon       =0x400C2;
const int Wimp_DeleteWindow     =0x400C3;
const int Wimp_DeleteIcon       =0x400C4;
const int Wimp_OpenWindow       =0x400C5;
const int Wimp_CloseWindow      =0x400C6;
const int Wimp_Poll             =0x400C7;
const int Wimp_RedrawWindow     =0x400C8;
const int Wimp_UpdateWindow     =0x400C9;
const int Wimp_GetRectangle     =0x400CA;
const int Wimp_GetWindowState   =0x400CB;
const int Wimp_GetWindowInfo    =0x400CC;
const int Wimp_SetIconState     =0x400CD;
const int Wimp_GetIconState     =0x400CE;
const int Wimp_GetPointerInfo   =0x400CF;
const int Wimp_DragBox          =0x400D0;
const int Wimp_ForceRedraw      =0x400D1;
const int Wimp_SetCaretPosition =0x400D2;
const int Wimp_GetCaretPosition =0x400D3;
const int Wimp_CreateMenu       =0x400D4;
const int Wimp_DecodeMenu       =0x400D5;
const int Wimp_WhichIcon        =0x400D6;
const int Wimp_SetExtent        =0x400D7;
const int Wimp_SetPointerShape  =0x400D8;
const int Wimp_OpenTemplate     =0x400D9;
const int Wimp_CloseTemplate    =0x400DA;
const int Wimp_LoadTemplate     =0x400DB;
const int Wimp_ProcessKey       =0x400DC;
const int Wimp_CloseDown        =0x400DD;
const int Wimp_StartTask        =0x400DE;
const int Wimp_ReportError      =0x400DF;
const int Wimp_GetWindowOutline =0x400E0;
const int Wimp_PollIdle         =0x400E1;
const int Wimp_PlotIcon         =0x400E2;
const int Wimp_SetMode          =0x400E3;
const int Wimp_SetPalette       =0x400E4;
const int Wimp_ReadPalette      =0x400E5;
const int Wimp_SetColour        =0x400E6;
const int Wimp_SendMessage      =0x400E7;
const int Wimp_CreateSubMenu    =0x400E8;
const int Wimp_SpriteOp         =0x400E9;
const int Wimp_BaseOfSprites    =0x400EA;
const int Wimp_BlockCopy        =0x400EB;
const int Wimp_SlotSize         =0x400EC;
const int Wimp_ReadPixTrans     =0x400ED;
const int Wimp_ClaimFreeMemory  =0x400EE;
const int Wimp_CommandWindow    =0x400EF;
const int Wimp_TextColour       =0x400F0;
const int Wimp_TransferBlock    =0x400F1;
const int Wimp_ReadSysInfo      =0x400F2;
const int Wimp_SetFontColours   =0x400F3;
const int Wimp_GetMenuState     =0x400F4;
const int Wimp_RegisterFilter   =0x400F5;
const int Wimp_AddMessages      =0x400F6;
const int Wimp_RemoveMessages   =0x400F7;
const int Wimp_SetColourMapping =0x400F8;
const int Wimp_TextOp           =0x400F9;
const int Wimp_SetWatchdogState =0x400FA;
const int Wimp_Extend           =0x400FB;
const int Wimp_ResizeIcon       =0x400FC;

const int Null_Reason_Code      =0;
const int Redraw_Window_Request =1;
const int Open_Window_Request   =2;
const int Close_Window_Request  =3;
const int Pointer_Leaving_Window=4;
const int Pointer_Entering_Window=5;
const int Mouse_Click           =6;
const int User_Drag_Box         =7;
const int Key_Pressed           =8;
const int Menu_Selection        =9;
const int Scroll_Request        =10;
const int Lose_Caret            =11;
const int Gain_Caret            =12;
const int PollWord_NonZero      =13;
const int User_Message          =17;
const int User_Message_Recorded =18;
const int User_Message_Acknowledge=19;

const int Message_Quit          =0x00;
const int Message_DataSave      =0x01;
const int Message_DataSaveAck   =0x02;
const int Message_DataLoad      =0x03;
const int Message_DataLoadAck   =0x04;
const int Message_DataOpen      =0x05;
const int Message_RAMFetch      =0x06;
const int Message_RAMTransmit   =0x07;
const int Message_PreQuit       =0x08;
const int Message_PaletteChange =0x09;
const int Message_SaveDesktop   =0x0A;
const int Message_DeviceClaim   =0x0B;
const int Message_DeviceInUse   =0x0C;
const int Message_DataSaved     =0x0D;
const int Message_Shutdown      =0x0E;
const int Message_ClaimEntity   =0x0F;
const int Message_DataRequest   =0x10;
const int Message_Dragging      =0x11;
const int Message_DragClaim     =0x12;

const int Message_FilerOpenDir  =0x400;
const int Message_FilerCloseDir =0x401;
const int Message_FilerOpenDirAt=0x402;
const int Message_FilerSelectionDirectory=0x403;
const int Message_FilerAddSelection=0x404;
const int Message_FilerAction   =0x405;
const int Message_FilerControlAction=0x406;
const int Message_FilerSelection=0x407;

const int Message_AlarmSet      =0x500;
const int Message_AlarmGoneOff  =0x501;
const int Message_HelpRequest   =0x502;
const int Message_HelpReply     =0x503;

const int Message_MenuWarning   =0x400C0;
const int Message_ModeChange    =0x400C1;
const int Message_TaskInitialise=0x400C2;
const int Message_TaskClosedown =0x400C3;
const int Message_SlotSize      =0x400C4;
const int Message_SetSlot       =0x400C5;
const int Message_TaskNameRq    =0x400C6;
const int Message_TaskNameIs    =0x400C7;
const int Message_TaskStarted   =0x400C8;
const int Message_MenusDeleted  =0x400C9;
const int Message_Iconize       =0x400CA;
const int Message_WindowClosed  =0x400CB;
const int Message_WindowInfo    =0x400CC;

} /* namespace swi */
} /* namespace rtk */

#endif
