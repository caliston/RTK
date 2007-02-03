// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2005 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#include "rtk/swi/pdriver.h"
#include "rtk/os/call_swi.h"
#include "rtk/os/pdriver.h"
#include "rtk/graphics/point.h"
#include "rtk/graphics/box.h"

namespace rtk {
namespace os {

void PDriver_Info(int* _type,int* _xres,int* _yres,int* _features,
	const char** _name,int* _xhres,int* _yhres,int* _pnum)
{
	_kernel_swi_regs regs;
	call_swi(swi::PDriver_CurrentJob,&regs);
	if (_type) *_type=regs.r[0];
	if (_xres) *_xres=regs.r[1];
	if (_yres) *_yres=regs.r[2];
	if (_features) *_features=regs.r[3];
	if (_name) *_name=(const char*)regs.r[4];
	if (_xhres) *_xhres=regs.r[5];
	if (_yhres) *_yhres=regs.r[6];
	if (_pnum) *_pnum=regs.r[7];
}

void PDriver_CheckFeatures(int mask,int features)
{
	_kernel_swi_regs regs;
	regs.r[0]=mask;
	regs.r[1]=features;
	call_swi(swi::PDriver_CurrentJob,&regs);
}

void PDriver_PageSize(point* _size,box* _bbox)
{
	_kernel_swi_regs regs;
	call_swi(swi::PDriver_CurrentJob,&regs);
	if (_size) *_size=point(regs.r[1],regs.r[2]);
	if (_bbox) *_bbox=box(regs.r[3],regs.r[4],regs.r[5],regs.r[6]);
}

void PDriver_SelectJob(int handle,const char* title,int* _handle)
{
	_kernel_swi_regs regs;
	regs.r[0]=handle;
	regs.r[1]=(int)title;
	call_swi(swi::PDriver_SelectJob,&regs);
	if (_handle) *_handle=regs.r[0];
}

void PDriver_CurrentJob(int* _handle)
{
	_kernel_swi_regs regs;
	call_swi(swi::PDriver_CurrentJob,&regs);
	if (_handle) *_handle=regs.r[0];
}

void PDriver_EndJob(int handle)
{
	_kernel_swi_regs regs;
	regs.r[0]=handle;
	call_swi(swi::PDriver_EndJob,&regs);
}

void PDriver_AbortJob(int handle)
{
	_kernel_swi_regs regs;
	regs.r[0]=handle;
	call_swi(swi::PDriver_AbortJob,&regs);
}

void PDriver_GiveRectangle(unsigned int id,const box& bbox,
	const linear_transformation& trans,const point& pos,int bcolour)
{
	_kernel_swi_regs regs;
	regs.r[0]=id;
	regs.r[1]=(int)&bbox;
	regs.r[2]=(int)&trans;
	regs.r[3]=(int)&pos;
	regs.r[4]=bcolour;
	call_swi(swi::PDriver_GiveRectangle,&regs);
}

void PDriver_DrawPage(int copies,box* clip,int page_num,
	const char* page_str,int* _more,unsigned int* _id)
{
	_kernel_swi_regs regs;
	regs.r[0]=copies;
	regs.r[1]=(int)clip;
	regs.r[2]=page_num;
	regs.r[3]=(int)page_str;
	call_swi(swi::PDriver_DrawPage,&regs);
	if (_more) *_more=regs.r[0];
	if (_id) *_id=regs.r[2];
}

void PDriver_GetRectangle(box* clip,int* _more,unsigned int* _id)
{
	_kernel_swi_regs regs;
	regs.r[1]=(int)clip;
	call_swi(swi::PDriver_GetRectangle,&regs);
	if (_more) *_more=regs.r[0];
	if (_id) *_id=regs.r[2];
}

} /* namespace os */
} /* namespace rtk */
