// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#include <kernel.h>

#include "rtk/swi/os.h"
#include "rtk/os/call_swi.h"
#include "rtk/os/os.h"

namespace rtk {
namespace os {

void OS_Byte161(int address,int* _value)
{
	_kernel_swi_regs regs;
	regs.r[0]=161;
	regs.r[1]=address;
	call_swi(swi::OS_Byte,&regs);
	if (_value) *_value=regs.r[2];
}

void OS_Find(int code,const char* name,const char* path,int* _handle)
{
	_kernel_swi_regs regs;
	regs.r[0]=code;
	regs.r[1]=(int)name;
	regs.r[2]=(int)path;
	call_swi(swi::OS_Find,&regs);
	if (_handle) *_handle=regs.r[0];
}

void OS_SpriteOp40(sprite_area* area,sprite* sp,int* _xsize,int* _ysize,
	int* _mask,int* _mode)
{
	_kernel_swi_regs regs;
	regs.r[0]=0x228;
	regs.r[1]=(int)area;
	regs.r[2]=(int)sp;
	call_swi(swi::OS_SpriteOp,&regs);
	if (_xsize) *_xsize=regs.r[3];
	if (_ysize) *_ysize=regs.r[4];
	if (_mask) *_mask=regs.r[5];
	if (_mode) *_mode=regs.r[6];
}

void OS_SpriteOp40(sprite_area* area,const char* name,int* _xsize,int* _ysize,
	int* _mask,int* _mode)
{
	_kernel_swi_regs regs;
	regs.r[0]=(area)?0x128:0x028;
	regs.r[1]=(int)area;
	regs.r[2]=(int)name;
	call_swi(swi::OS_SpriteOp,&regs);
	if (_xsize) *_xsize=regs.r[3];
	if (_ysize) *_ysize=regs.r[4];
	if (_mask) *_mask=regs.r[5];
	if (_mode) *_mode=regs.r[6];
}

void OS_ReadModeVariable(int index,int* _value)
{
	_kernel_swi_regs regs;
	regs.r[0]=-1;
	regs.r[1]=index;
	call_swi(swi::OS_ReadModeVariable,&regs);
	if (_value) *_value=regs.r[2];
}

void OS_Plot(int code,const point& p)
{
	_kernel_swi_regs regs;
	regs.r[0]=code;
	regs.r[1]=p.x();
	regs.r[2]=p.y();
	call_swi(swi::OS_Plot,&regs);
}

void OS_ReadMonotonicTime(unsigned int* _time)
{
	_kernel_swi_regs regs;
	call_swi(swi::OS_ReadMonotonicTime,&regs);
	if (_time) *_time=regs.r[0];
}

}; /* namespace os */
}; /* namespace rtk */
