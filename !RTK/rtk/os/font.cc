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
	void* coord,void* trans,int length,int* _split,point* _p,int* _length)
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
	if (_split) *_split=regs.r[1];
	if (_p) *_p=point(regs.r[3],regs.r[4]);
	if (_length) *_length=regs.r[7];
}

}; /* namespace os */
}; /* namespace rtk */
