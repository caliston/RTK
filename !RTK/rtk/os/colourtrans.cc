// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#include "rtk/swi/colourtrans.h"
#include "rtk/os/call_swi.h"
#include "rtk/os/colourtrans.h"

namespace rtk {
namespace os {

void ColourTrans_SetFontColours(int handle,int bcolour,int fcolour,int max,
	int* _bcolour,int* _fcolour,int* _max)
{
	_kernel_swi_regs regs;
	regs.r[0]=handle;
	regs.r[1]=bcolour;
	regs.r[2]=fcolour;
	regs.r[3]=max;
	call_swi(swi::ColourTrans_SetFontColours,&regs);
	if (_bcolour) *_bcolour=regs.r[1];
	if (_fcolour) *_fcolour=regs.r[2];
	if (_max) *_max=regs.r[3];
}

}; /* namespace os */
}; /* namespace rtk */
