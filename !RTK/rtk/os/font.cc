// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#include "rtk/graphics/point.h"
#include "rtk/swi/font.h"
#include "rtk/os/call_swi.h"
#include "rtk/os/font.h"

namespace rtk {
namespace os {

void Font_FindFont(const char* id,int xsize,int ysize,int xres,int yres,
	int* _handle,int* _xres,int* _yres)
{
	_kernel_swi_regs regs;
	regs.r[1]=(int)id;
	regs.r[2]=xsize;
	regs.r[3]=ysize;
	regs.r[4]=xres;
	regs.r[5]=yres;
	call_swi(swi::Font_FindFont,&regs);
	if (_handle) *_handle=regs.r[0];
	if (_xres) *_xres=regs.r[4];
	if (_yres) *_yres=regs.r[5];
}

void Font_LoseFont(int handle)
{
	_kernel_swi_regs regs;
	regs.r[0]=handle;
	call_swi(swi::Font_LoseFont,&regs);
}

void Font_ReadDefn(int handle,int* _buffer_size)
{
	_kernel_swi_regs regs;
	regs.r[0]=handle;
	regs.r[1]=0;
	regs.r[3]=0x4c4c5546;
	call_swi(swi::Font_ReadDefn,&regs);
	if (_buffer_size) *_buffer_size=regs.r[2];
}

void Font_ReadDefn(int handle,char* _id,int* _xsize,int* _ysize,
	int* _xres,int* _yres,int* _age,int* _usage_count)
{
	_kernel_swi_regs regs;
	regs.r[0]=handle;
	regs.r[1]=(int)_id;
	regs.r[3]=0;
	call_swi(swi::Font_ReadDefn,&regs);
	if (_xsize) *_xsize=regs.r[2];
	if (_ysize) *_ysize=regs.r[3];
	if (_xres) *_xres=regs.r[4];
	if (_yres) *_yres=regs.r[5];
	if (_age) *_age=regs.r[6];
	if (_usage_count) *_usage_count=regs.r[7];
}

void Font_CharBBox(int handle,int code,int flags,box* _bbox)
{
	_kernel_swi_regs regs;
	regs.r[0]=handle;
	regs.r[1]=code;
	regs.r[2]=flags;
	call_swi(swi::Font_CharBBox,&regs);
	if (_bbox) *_bbox=box(regs.r[1],regs.r[2],regs.r[3],regs.r[4]);
}

void Font_Paint(int handle,const char* s,int plot,const point& p,void* coord,
	void* trans,int length)
{
	_kernel_swi_regs regs;
	regs.r[0]=handle;
	regs.r[1]=(int)s;
	regs.r[2]=plot;
	regs.r[3]=p.x();
	regs.r[4]=p.y();
	regs.r[5]=(int)coord;
	regs.r[6]=(int)trans;
	regs.r[7]=length;
	call_swi(swi::Font_Paint,&regs);
}

void Font_ScanString(int handle,const char* s,int plot,const point& p,
	void* coord,void* trans,int length,char** _split,point* _p,int* _length)
{
	_kernel_swi_regs regs;
	regs.r[0]=handle;
	regs.r[1]=(int)s;
	regs.r[2]=plot;
	regs.r[3]=p.x();
	regs.r[4]=p.y();
	regs.r[5]=(int)coord;
	regs.r[6]=(int)trans;
	regs.r[7]=length;
	call_swi(swi::Font_ScanString,&regs);
	if (_split) *_split=(char*)regs.r[1];
	if (_p) *_p=point(regs.r[3],regs.r[4]);
	if (_length) *_length=regs.r[7];
}

}; /* namespace os */
}; /* namespace rtk */
