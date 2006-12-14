// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2004 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#include "rtk/swi/messagetrans.h"
#include "rtk/os/call_swi.h"
#include "rtk/os/messagetrans.h"

namespace rtk {
namespace os {

void MessageTrans_FileInfo(const char* pathname,int* _flags,
	unsigned int* _size)
{
	_kernel_swi_regs regs;
	regs.r[1]=(int)pathname;
	call_swi(swi::MessageTrans_FileInfo,&regs);
	if (_flags) *_flags=regs.r[0];
	if (_size) *_size=regs.r[2];
}

void MessageTrans_OpenFile(int* descriptor,const char* pathname,
	void* buffer)
{
	_kernel_swi_regs regs;
	regs.r[0]=(int)descriptor;
	regs.r[1]=(int)pathname;
	regs.r[2]=(int)buffer;
	call_swi(swi::MessageTrans_OpenFile,&regs);
}

void MessageTrans_Lookup(int* descriptor,const char* token,char* buffer,
	unsigned int size,const char* arg0,const char* arg1,const char* arg2,
	const char* arg3,const char** _token,const char** _buffer,
	unsigned int* _size)
{
	_kernel_swi_regs regs;
	regs.r[0]=(int)descriptor;
	regs.r[1]=(int)token;
	regs.r[2]=(int)buffer;
	regs.r[3]=size;
	regs.r[4]=(int)arg0;
	regs.r[5]=(int)arg1;
	regs.r[6]=(int)arg2;
	regs.r[7]=(int)arg3;
	call_swi(swi::MessageTrans_Lookup,&regs);
	if (_token) *_token=(const char*)regs.r[1];
	if (_buffer) *_buffer=(const char*)regs.r[2];
	if (_size) *_size=regs.r[3];
}

void MessageTrans_CloseFile(int* descriptor)
{
	_kernel_swi_regs regs;
	regs.r[0]=(int)descriptor;
	call_swi(swi::MessageTrans_CloseFile,&regs);
}

} /* namespace os */
} /* namespace rtk */
