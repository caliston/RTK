// This file is part of the RISC OS Toolkit (RTK).
// Copyright � 2003 Graham Shaw.
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

void OS_File1(const char* name,unsigned int loadaddr,unsigned int execaddr,
	unsigned int attr)
{
	_kernel_swi_regs regs;
	regs.r[0]=1;
	regs.r[1]=(int)name;
	regs.r[2]=loadaddr;
	regs.r[3]=execaddr;
	regs.r[5]=attr;
	call_swi(swi::OS_File,&regs);
}

void OS_File4(const char* name,unsigned int attr)
{
	_kernel_swi_regs regs;
	regs.r[0]=4;
	regs.r[1]=(int)name;
	regs.r[5]=attr;
	call_swi(swi::OS_File,&regs);
}

void OS_File6(const char* name,unsigned int* _objtype,unsigned int* _loadaddr,
	unsigned int* _execaddr,unsigned int* _length,unsigned int* _attr)
{
	_kernel_swi_regs regs;
	regs.r[0]=6;
	regs.r[1]=(int)name;
	call_swi(swi::OS_File,&regs);
	if (_objtype) *_objtype=regs.r[0];
	if (_loadaddr) *_loadaddr=regs.r[2];
	if (_execaddr) *_execaddr=regs.r[3];
	if (_length) *_length=regs.r[4];
	if (_attr) *_attr=regs.r[5];
}

void OS_File8(const char* name,unsigned int entries)
{
	_kernel_swi_regs regs;
	regs.r[0]=8;
	regs.r[1]=(int)name;
	regs.r[4]=entries;
	call_swi(swi::OS_File,&regs);
}

void OS_File17(const char* name,unsigned int* _objtype,unsigned int* _loadaddr,
	unsigned int* _execaddr,unsigned int* _length,unsigned int* _attr)
{
	_kernel_swi_regs regs;
	regs.r[0]=17;
	regs.r[1]=(int)name;
	call_swi(swi::OS_File,&regs);
	if (_objtype) *_objtype=regs.r[0];
	if (_loadaddr) *_loadaddr=regs.r[2];
	if (_execaddr) *_execaddr=regs.r[3];
	if (_length) *_length=regs.r[4];
	if (_attr) *_attr=regs.r[5];
}

void OS_File18(const char* name,unsigned int filetype)
{
	_kernel_swi_regs regs;
	regs.r[0]=18;
	regs.r[1]=(int)name;
	regs.r[2]=filetype;
	call_swi(swi::OS_File,&regs);
}

void OS_Args5(int handle,bool* _eof)
{
	_kernel_swi_regs regs;
	regs.r[0]=5;
	regs.r[1]=handle;
	call_swi(swi::OS_Args,&regs);
	if (_eof) *_eof=regs.r[2]!=0;
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

void OS_Find0(int handle)
{
	_kernel_swi_regs regs;
	regs.r[0]=0;
	regs.r[1]=handle;
	call_swi(swi::OS_Find,&regs);
}

void OS_GBPB2(int handle,const void* buffer,unsigned int count,
	unsigned int* _fp)
{
	_kernel_swi_regs regs;
	regs.r[0]=2;
	regs.r[1]=handle;
	regs.r[2]=(int)buffer;
	regs.r[3]=count;
	call_swi(swi::OS_GBPB,&regs);
	if (_fp) *_fp=regs.r[4];
}

void OS_GBPB4(int handle,void* buffer,unsigned int count,
	unsigned int* _excess,unsigned int* _fp)
{
	_kernel_swi_regs regs;
	regs.r[0]=4;
	regs.r[1]=handle;
	regs.r[2]=(int)buffer;
	regs.r[3]=count;
	call_swi(swi::OS_GBPB,&regs);
	if (_excess) *_excess=regs.r[3];
	if (_fp) *_fp=regs.r[4];
}

void OS_GBPB12(const char* name,void* buffer,unsigned int count,
	int offset,unsigned int length,const char* pattern,
	unsigned int* _count,int* _offset)
{
	_kernel_swi_regs regs;
	regs.r[0]=12;
	regs.r[1]=(int)name;
	regs.r[2]=(int)buffer;
	regs.r[3]=count;
	regs.r[4]=offset;
	regs.r[5]=length;
	regs.r[6]=(int)pattern;
	call_swi(swi::OS_GBPB,&regs);
	if (_count) *_count=regs.r[3];
	if (_offset) *_offset=regs.r[4];
}

void OS_SetVarVal(const char* varname,const char* value,unsigned int length,
	unsigned int context,unsigned int vartype,unsigned int* _context,
	unsigned int* _vartype)
{
	_kernel_swi_regs regs;
	regs.r[0]=(int)varname;
	regs.r[1]=(int)value;
	regs.r[2]=length;
	regs.r[3]=context;
	regs.r[4]=vartype;
	call_swi(swi::OS_SetVarVal,&regs);
	if (_context) *_context=regs.r[3];
	if (_vartype) *_vartype=regs.r[4];
}

void OS_FSControl25(const char* src_name,const char* dst_name)
{
	_kernel_swi_regs regs;
	regs.r[0]=25;
	regs.r[1]=(int)src_name;
	regs.r[2]=(int)dst_name;
	call_swi(swi::OS_FSControl,&regs);
}

void OS_FSControl26(const char* src_name,const char* dst_name,
	unsigned int mask,unsigned long long start_time,
	unsigned long long end_time,void* extra_info)
{
	_kernel_swi_regs regs;
	regs.r[0]=26;
	regs.r[1]=(int)src_name;
	regs.r[2]=(int)dst_name;
	regs.r[3]=mask;
	regs.r[4]=start_time>>0;
	regs.r[5]=start_time>>32;
	regs.r[6]=end_time>>0;
	regs.r[7]=end_time>>32;
	regs.r[8]=(int)extra_info;
	call_swi(swi::OS_FSControl,&regs);
}

void OS_FSControl37(const char* pathname,char* buffer,const char* pathvar,
	const char* path,unsigned int size,unsigned int* _size)
{
	_kernel_swi_regs regs;
	regs.r[0]=37;
	regs.r[1]=(int)pathname;
	regs.r[2]=(int)buffer;
	regs.r[3]=(int)pathvar;
	regs.r[4]=(int)path;
	regs.r[5]=size;
	call_swi(swi::OS_FSControl,&regs);
	if (_size) *_size=regs.r[5];
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

} /* namespace os */
} /* namespace rtk */
