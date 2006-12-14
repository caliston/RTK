// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#include "rtk/swi/dragasprite.h"
#include "rtk/os/call_swi.h"
#include "rtk/os/dragasprite.h"

namespace rtk {
namespace os {

void DragASprite_Start(int flags,sprite_area* area,const char* name,
	box* dbox,box* pbox)
{
	_kernel_swi_regs regs;
	regs.r[0]=flags;
	regs.r[1]=(int)area;
	regs.r[2]=(int)name;
	regs.r[3]=(int)dbox;
	regs.r[4]=(int)pbox;
	call_swi(swi::DragASprite_Start,&regs);
}

void DragASprite_Stop()
{
	_kernel_swi_regs regs;
	call_swi(swi::DragASprite_Stop,&regs);
}

} /* namespace os */
} /* namespace rtk */
