// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#include "rtk/swi/wimp.h"
#include "rtk/os/call_swi.h"
#include "rtk/os/wimp.h"

namespace rtk {
namespace os {

void Wimp_Initialise(int version,const char* name,int* messages,
	int* _version,int* _handle)
{
	_kernel_swi_regs regs;
	regs.r[0]=version;
	regs.r[1]=0x4B534154;
	regs.r[2]=(int)name;
	regs.r[3]=(int)messages;
	call_swi(swi::Wimp_Initialise,&regs);
	if (_version) *_version=regs.r[0];
	if (_handle) *_handle=regs.r[1];
}

void Wimp_CreateWindow(const window_create& block,int* _handle)
{
	_kernel_swi_regs regs;
	regs.r[1]=(int)&block;
	call_swi(swi::Wimp_CreateWindow,&regs);
	if (_handle) *_handle=regs.r[0];
}

void Wimp_CreateIcon(int priority,const icon_create& block,int* _handle)
{
	_kernel_swi_regs regs;
	regs.r[0]=priority;
	regs.r[1]=(int)&block;
	call_swi(swi::Wimp_CreateIcon,&regs);
	if (_handle) *_handle=regs.r[0];
}

void Wimp_DeleteWindow(const window_delete& block)
{
	_kernel_swi_regs regs;
	regs.r[1]=(int)&block;
	call_swi(swi::Wimp_DeleteWindow,&regs);
}

void Wimp_DeleteIcon(const icon_delete& block)
{
	_kernel_swi_regs regs;
	regs.r[1]=(int)&block;
	call_swi(swi::Wimp_DeleteIcon,&regs);
}

void Wimp_OpenWindow(const window_open& block,int phandle,int flags)
{
	_kernel_swi_regs regs;
	regs.r[1]=(int)&block;
	regs.r[2]=0x4b534154;
	regs.r[3]=phandle;
	regs.r[4]=flags;
	call_swi(swi::Wimp_OpenWindow,&regs);
}

void Wimp_OpenWindow(const window_open& block)
{
	_kernel_swi_regs regs;
	regs.r[1]=(int)&block;
	call_swi(swi::Wimp_OpenWindow,&regs);
}

void Wimp_CloseWindow(const window_close& block)
{
	_kernel_swi_regs regs;
	regs.r[1]=(int)&block;
	call_swi(swi::Wimp_CloseWindow,&regs);
}

void Wimp_Poll(int mask,wimp_block& block,int* pollword,int* _code)
{
	_kernel_swi_regs regs;
	regs.r[0]=mask;
	regs.r[1]=(int)&block;
	regs.r[3]=(int)pollword;
	call_swi(swi::Wimp_Poll,&regs);
	if (_code) *_code=regs.r[0];
}

void Wimp_RedrawWindow(window_redraw& block,int* _more)
{
	_kernel_swi_regs regs;
	regs.r[1]=(int)&block;
	call_swi(swi::Wimp_RedrawWindow,&regs);
	if (_more) *_more=regs.r[0];
}

void Wimp_UpdateWindow(window_redraw& block,int* _more)
{
	_kernel_swi_regs regs;
	regs.r[1]=(int)&block;
	call_swi(swi::Wimp_UpdateWindow,&regs);
	if (_more) *_more=regs.r[0];
}

void Wimp_GetRectangle(window_redraw& block,int* _more)
{
	_kernel_swi_regs regs;
	regs.r[1]=(int)&block;
	call_swi(swi::Wimp_GetRectangle,&regs);
	if (_more) *_more=regs.r[0];
}

void Wimp_GetWindowState(window_state_get& block)
{
	_kernel_swi_regs regs;
	regs.r[1]=(int)&block;
	call_swi(swi::Wimp_GetWindowState,&regs);
}

void Wimp_SetIconState(icon_state_set& block)
{
	_kernel_swi_regs regs;
	regs.r[1]=(int)&block;
	call_swi(swi::Wimp_SetIconState,&regs);
}

void Wimp_GetIconState(icon_state_get& block)
{
	_kernel_swi_regs regs;
	regs.r[1]=(int)&block;
	call_swi(swi::Wimp_GetIconState,&regs);
}

void Wimp_GetPointerInfo(pointer_info_get& block)
{
	_kernel_swi_regs regs;
	regs.r[1]=(int)&block;
	call_swi(swi::Wimp_GetPointerInfo,&regs);
}

void Wimp_DragBox(drag_box& block)
{
	_kernel_swi_regs regs;
	regs.r[1]=(int)&block;
	call_swi(swi::Wimp_DragBox,&regs);
}

void Wimp_ForceRedraw(int handle,const box& bbox)
{
	_kernel_swi_regs regs;
	regs.r[0]=handle;
	regs.r[1]=bbox.xmin();
	regs.r[2]=bbox.ymin();
	regs.r[3]=bbox.xmax();
	regs.r[4]=bbox.ymax();
	call_swi(swi::Wimp_ForceRedraw,&regs);
}

void Wimp_SetCaretPosition(int whandle,int ihandle,const point& p,
	int height,int index)
{
	_kernel_swi_regs regs;
	regs.r[0]=whandle;
	regs.r[1]=ihandle;
	regs.r[2]=p.x();
	regs.r[3]=p.y();
	regs.r[4]=height;
	regs.r[5]=index;
	call_swi(swi::Wimp_SetCaretPosition,&regs);
}

void Wimp_GetCaretPosition(caret_position_get& block)
{
	_kernel_swi_regs regs;
	regs.r[1]=(int)&block;
	call_swi(swi::Wimp_GetCaretPosition,&regs);
}

void Wimp_CreateMenu(int handle,const point& p)
{
	_kernel_swi_regs regs;
	regs.r[1]=handle;
	regs.r[2]=p.x();
	regs.r[3]=p.y();
	call_swi(swi::Wimp_CreateMenu,&regs);
}

void Wimp_SetExtent(int handle,const box& workarea)
{
	_kernel_swi_regs regs;
	regs.r[0]=handle;
	regs.r[1]=(int)&workarea;
	call_swi(swi::Wimp_SetExtent,&regs);
}

void Wimp_ProcessKey(int code)
{
	_kernel_swi_regs regs;
	regs.r[0]=code;
	call_swi(swi::Wimp_ProcessKey,&regs);
}

void Wimp_StartTask(const char* command,int* _handle)
{
	_kernel_swi_regs regs;
	regs.r[0]=(int)command;
	call_swi(swi::Wimp_StartTask,&regs);
	if (_handle) *_handle=regs.r[0];
}

void Wimp_ReportError(const _kernel_oserror &block,const char* title,
	int flags,int* _result)
{
	_kernel_swi_regs regs;
	if (title==0) title="";
	regs.r[0]=(int)&block;
	regs.r[1]=flags;
	regs.r[2]=(int)title;
	call_swi(swi::Wimp_ReportError,&regs);
	if (_result) *_result=regs.r[1];
}

void Wimp_ReportError(int errnum,const char* message,const char* title,
	int flags,int* _result)
{
	_kernel_swi_regs regs;
	_kernel_oserror block;
	block.errnum=errnum;
	strcpy(block.errmess,message);
	if (title==0) title="";
	regs.r[0]=(int)&block;
	regs.r[1]=flags;
	regs.r[2]=(int)title;
	call_swi(swi::Wimp_ReportError,&regs);
	if (_result) *_result=regs.r[1];
}

void Wimp_SetColour(int colour)
{
	_kernel_swi_regs regs;
	regs.r[0]=colour;
	call_swi(swi::Wimp_SetColour,&regs);
}

void Wimp_SendMessage(int wimpcode,wimp_block& wimpblock,int thandle,
	int ihandle,int* _thandle)
{
	_kernel_swi_regs regs;
	regs.r[0]=wimpcode;
	regs.r[1]=(int)&wimpblock;
	regs.r[2]=thandle;
	regs.r[3]=ihandle;
	call_swi(swi::Wimp_SendMessage,&regs);
	if (_thandle) *_thandle=regs.r[2];
}

void Wimp_CreateSubMenu(int handle,const point& p)
{
	_kernel_swi_regs regs;
	regs.r[1]=handle;
	regs.r[2]=p.x();
	regs.r[3]=p.y();
	call_swi(swi::Wimp_CreateSubMenu,&regs);
}

void Wimp_SpriteOp40(const char* name,int* _xsize,int* _ysize,
	int* _mask,int* _mode)
{
	_kernel_swi_regs regs;
	regs.r[0]=40;
	regs.r[2]=(int)name;
	call_swi(swi::Wimp_SpriteOp,&regs);
	if (_xsize) *_xsize=regs.r[3];
	if (_ysize) *_ysize=regs.r[4];
	if (_mask) *_mask=regs.r[5];
	if (_mode) *_mode=regs.r[6];
}

void Wimp_BlockCopy(int whandle,const box& src,const point& dst)
{
	_kernel_swi_regs regs;
	regs.r[0]=whandle;
	regs.r[1]=src.xmin();
	regs.r[2]=src.ymin();
	regs.r[3]=src.xmax();
	regs.r[4]=src.ymax();
	regs.r[5]=dst.x();
	regs.r[6]=dst.y();
	call_swi(swi::Wimp_BlockCopy,&regs);
}

void Wimp_ReadPixTrans(sprite_area* area,const char* name,
	unsigned int* _scale,unsigned char* _table)
{
	_kernel_swi_regs regs;
	regs.r[0]=(area)?0x100:0x000;
	regs.r[1]=(int)area;
	regs.r[2]=(int)name;
	regs.r[6]=(int)_scale;
	regs.r[7]=(int)_table;
	call_swi(swi::Wimp_ReadPixTrans,&regs);
}

void Wimp_ReadPixTrans(sprite_area* area,sprite* sp,
	unsigned int* _scale,unsigned char* _table)
{
	_kernel_swi_regs regs;
	regs.r[0]=0x200;
	regs.r[1]=(int)area;
	regs.r[2]=(int)sp;
	regs.r[6]=(int)_scale;
	regs.r[7]=(int)_table;
	call_swi(swi::Wimp_ReadPixTrans,&regs);
}

void Wimp_TextColour(int colour)
{
	_kernel_swi_regs regs;
	regs.r[0]=colour;
	call_swi(swi::Wimp_TextColour,&regs);
}

void Wimp_TransferBlock(int src_thandle,const void* src_buffer,
	int dst_thandle,void* dst_buffer,unsigned int count)
{
	_kernel_swi_regs regs;
	regs.r[0]=src_thandle;
	regs.r[1]=(int)src_buffer;
	regs.r[2]=dst_thandle;
	regs.r[3]=(int)dst_buffer;
	regs.r[4]=count;
	call_swi(swi::Wimp_TransferBlock,&regs);
}

void Wimp_ReadSysInfo(int index,int* _r0,int* _r1)
{
	_kernel_swi_regs regs;
	regs.r[0]=index;
	call_swi(swi::Wimp_ReadSysInfo,&regs);
	if (_r0) *_r0=regs.r[0];
	if (_r1) *_r1=regs.r[1];
}

void Wimp_SetFontColours(int bcolour,int fcolour)
{
	_kernel_swi_regs regs;
	regs.r[1]=bcolour;
	regs.r[2]=fcolour;
	call_swi(swi::Wimp_SetFontColours,&regs);
}

void Wimp_GetMenuState(int* buffer)
{
	_kernel_swi_regs regs;
	regs.r[0]=0;
	regs.r[1]=(int)buffer;
	call_swi(swi::Wimp_GetMenuState,&regs);
}

void Wimp_GetMenuState(int whandle,int ihandle,int* buffer)
{
	_kernel_swi_regs regs;
	regs.r[0]=1;
	regs.r[1]=(int)buffer;
	regs.r[2]=whandle;
	regs.r[3]=ihandle;
	call_swi(swi::Wimp_GetMenuState,&regs);
}

void Wimp_TextOp0(int fcolour,int bcolour)
{
	_kernel_swi_regs regs;
	regs.r[0]=0;
	regs.r[1]=fcolour;
	regs.r[2]=bcolour;
	call_swi(swi::Wimp_TextOp,&regs);
}

void Wimp_TextOp1(const char* s,int count,int* _width)
{
	_kernel_swi_regs regs;
	regs.r[0]=1;
	regs.r[1]=(int)s;
	regs.r[2]=count;
	call_swi(swi::Wimp_TextOp,&regs);
	if (_width) *_width=regs.r[0];
}

void Wimp_TextOp2(const char* s,const point& p)
{
	_kernel_swi_regs regs;
	regs.r[0]=2;
	regs.r[1]=(int)s;
	regs.r[2]=-1;
	regs.r[3]=-1;
	regs.r[4]=p.x();
	regs.r[5]=p.y();
	call_swi(swi::Wimp_TextOp,&regs);
}

void Wimp_TextOp3(const char* s,int width,char split,const char** _index)
{
	_kernel_swi_regs regs;
	regs.r[0]=3;
	regs.r[1]=(int)s;
	regs.r[2]=width;
	regs.r[3]=split;
	call_swi(swi::Wimp_TextOp,&regs);
	if (_index) *_index=(const char*)regs.r[0];
}

void Wimp_ResizeIcon(int whandle,int ihandle,const box& bbox)
{
	_kernel_swi_regs regs;
	regs.r[0]=whandle;
	regs.r[1]=ihandle;
	regs.r[2]=bbox.xmin();
	regs.r[3]=bbox.ymin();
	regs.r[4]=bbox.xmax();
	regs.r[5]=bbox.ymax();
	call_swi(swi::Wimp_ResizeIcon,&regs);
}

}; /* namespace os */
}; /* namespace rtk */
