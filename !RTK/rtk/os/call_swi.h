// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_OS_SWI
#define _RTK_OS_SWI

#include "kernel.h"

#include "rtk/os/exception.h"

class _kernel_swi_regs;

namespace rtk {
namespace os {

/** Call a RISC OS software interrupt.
 * @param number the software interrupt number
 * @param regs the register state (for input and output)
 */
inline void call_swi(unsigned int number,_kernel_swi_regs* regs)
{
	const int X=0x20000;
	_kernel_oserror* err=_kernel_swi(X+number,regs,regs);
	if (err) throw exception(err);
}

}; /* namespace os */
}; /* namespace rtk */

#endif
