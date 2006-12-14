// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003-2005 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#include <string.h>
#include <algorithm>

#include "rtk/swi/wimp.h"
#include "rtk/os/os.h"
#include "rtk/os/wimp.h"
#include "rtk/desktop/application.h"
#include "rtk/events/wimp.h"
#include "rtk/events/message.h"
#include "rtk/events/datasave.h"
#include "rtk/events/dataload.h"
#include "rtk/events/ramtransmit.h"
#include "rtk/events/save_to_app.h"
#include "rtk/events/save_to_file.h"
#include "rtk/events/saved.h"
#include "rtk/transfer/save.h"

namespace rtk {
namespace transfer {

using std::min;
using std::max;

save::save():
	_state(state_idle),
	_thandle(0),
	_filetype(0xffd),
	_allow_ram_transfer(true),
	_secure(false),
	_ldata(0),
	_lsize(0)
{
	os::Wimp_ReadSysInfo(5,&_thandle,0);
}

save::~save()
{}

graphics::box save::min_bbox() const
{
	return graphics::box();
}

void save::handle_event(events::datasaveack& ev)
{
	// A Message_DataSaveAck is acted upon in response to a Message_DataSave.
	if (_state==state_datasave)
	{
		get_file(ev.pathname());
		_pathname=ev.pathname();
		_secure=ev.estsize()!=static_cast<size_type>(-1);
		ev.reply();
		_state=state_dataload;
	}
}

void save::handle_event(events::dataloadack& ev)
{
	// A Message_DataLoadAck is acted upon in response to a Message_DataLoad.
	if (_state==state_dataload)
	{
		_state=state_idle;
		events::saved ev2(*this,*this);
		ev2.post();
	}
}

void save::handle_event(events::ramfetch& ev)
{
	// A Message_RAMFetch is acted upon in response to a Message_DataSave
	// or a Message_RAMTransmit, but only if RAM transfers are enabled.
	if (((_state==state_datasave)||(_state==state_ramtransmit))&&
		_allow_ram_transfer)
	{
		size_type rfree=ev.buffer_size();
		void* rdata=ev.buffer();
		if (!_lsize) get_block(&_ldata,&_lsize);
		while (rfree&&_lsize)
		{
			size_type count=min(rfree,_lsize);
			os::Wimp_TransferBlock(_thandle,_ldata,ev.thandle(),rdata,count);
			rdata=static_cast<char*>(rdata)+count;
			_ldata=static_cast<const char*>(_ldata)+count;
			rfree-=count;
			_lsize-=count;
			if (!_lsize) get_block(&_ldata,&_lsize);
		}
		ev.reply(ev.buffer_size()-rfree);
		if (rfree)
		{
			// If there is space at the end of the buffer then this is
			// the last message and no reply is expected.
			finish();
			_state=state_idle;
			events::saved ev2(*this,*this);
			ev2.post();
		}
		else
		{
			// If there is no space at the end of the buffer then at
			// least one more Message_RAMFetch is expected.
			_state=state_ramtransmit;
		}
	}
}

void save::handle_event(events::save_to_app& ev)
{
	os::wimp_block& block=*new os::wimp_block;
	block.word[3]=0;
	block.word[4]=swi::Message_DataSave;
	block.word[5]=ev.whandle();
	block.word[6]=ev.ihandle();
	block.word[7]=ev.position().x();
	block.word[8]=ev.position().y();
	block.word[9]=estsize();
	block.word[10]=filetype();
	unsigned int i=ev.leafname().copy(block.byte+44,211);
	block.byte[44+i]=0;
	block.word[0]=44+((i+4)&~3);
	if (desktop::application* app=ev.target()->parent_application())
		app->send_message(swi::User_Message,block,ev.whandle(),ev.ihandle());
	start();
	_ldata=0;
	_lsize=0;
	_pathname=string();
	_secure=false;
	_state=state_datasave;
}

void save::handle_event(events::save_to_file& ev)
{
	_pathname=string();
	_secure=false;
	get_file(ev.pathname());
	_pathname=ev.pathname();
	_secure=true;
	_state=state_idle;
	events::saved ev2(*this,*this);
	ev2.post();
}

void save::handle_event(events::datarequest& ev)
{
	ev.reply(filetype(),estsize());
	start();
	_ldata=0;
	_lsize=0;
	_pathname=string();
	_secure=false;
	_state=state_datasave;
}

save& save::filetype(unsigned int filetype)
{
	_filetype=filetype;
	return *this;
}

save& save::allow_ram_transfer(bool value)
{
	_allow_ram_transfer=value;
	return *this;
}

void save::deliver_wimp_block(int wimpcode,os::wimp_block& wimpblock)
{
	switch (wimpcode)
	{
	case 17:
	case 18:
	case 19:
		deliver_message(wimpcode,wimpblock);
		break;
	default:
		{ 
			events::wimp ev(*this,wimpcode,wimpblock);
			ev.post();
		}
		break;
	}
}

void save::deliver_message(int wimpcode,os::wimp_block& wimpblock)
{
	switch (wimpblock.word[4])
	{
	case swi::Message_DataSave:
		{
			events::datasave ev(*this,wimpcode,wimpblock);
			ev.post();
		}
		break;
	case swi::Message_DataSaveAck:
		{
			events::datasaveack ev(*this,wimpcode,wimpblock);
			ev.post();
		}
		break;
	case swi::Message_DataLoad:
		{
			events::dataload ev(*this,wimpcode,wimpblock);
			ev.post();
		}
		break;
	case swi::Message_DataLoadAck:
		{
			events::dataloadack ev(*this,wimpcode,wimpblock);
			ev.post();
		}
		break;
	case swi::Message_RAMFetch:
		{
			events::ramfetch ev(*this,wimpcode,wimpblock);
			ev.post();
		}
		break;
	case swi::Message_RAMTransmit:
		{
			events::ramtransmit ev(*this,wimpcode,wimpblock);
			ev.post();
		}
		break;
	default:
		{
			events::message ev(*this,wimpcode,wimpblock);
			ev.post();
		}
		break;
	}
}

void save::get_file(const string& pathname)
{
	start();
	int fhandle;
	os::OS_Find(0x83,pathname.c_str(),0,&fhandle);
	get_block(&_ldata,&_lsize);
	while (_lsize)
	{
		os::OS_GBPB2(fhandle,_ldata,_lsize,0);
		get_block(&_ldata,&_lsize);
	}
	os::OS_Find0(fhandle);
	os::OS_File18(pathname.c_str(),filetype());
	finish();
}

} /* namespace transfer */
} /* namespace rtk */
