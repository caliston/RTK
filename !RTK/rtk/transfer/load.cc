// This file is part of the RISC OS Toolkit (RTK).
// Copyright � 2003-2005 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#include "rtk/swi/wimp.h"
#include "rtk/os/os.h"
#include "rtk/os/wimp.h"
#include "rtk/desktop/application.h"
#include "rtk/events/wimp.h"
#include "rtk/events/message.h"
#include "rtk/events/ramfetch.h"
#include "rtk/events/datasaveack.h"
#include "rtk/events/loaded.h"
#include "rtk/transfer/load.h"

namespace rtk {
namespace transfer {

load::load():
	_state(state_idle),
	_allow_ram_transfer(true),
	_ldata(0),
	_lsize(0)
{}

load::~load()
{}

void load::handle_event(events::datasave& ev)
{
	// A Message_DataSave is acted upon at any time.
	if (_allow_ram_transfer)
	{
		// If RAM transfers are enabled then reply with Message_RAMFetch.
		start(ev.estsize());
		_ldata=0;
		_lsize=0;
		get_block(&_ldata,&_lsize);
		ev.reply(_ldata,_lsize);
		// Keep a copy of the Message_DataSave so that a Message_DataSaveAck
		// be sent if the Message_RAMFetch fails.
		unsigned int size=(ev.wimpblock().word[0]+3)>>2;
		for (unsigned int i=0;i!=size;++i)
			_datasave_block.word[i]=ev.wimpblock().word[i];
		_state=state_ramfetch_first;
	}
	else
	{
		// If RAM transfers are disabled then reply with Message_DataSaveAck.
		inherited::handle_event(ev);
		_state=state_idle;
	}
}

void load::handle_event(events::dataload& ev)
{
	inherited::handle_event(ev);
	_state=state_idle;
}

void load::handle_event(events::dataopen& ev)
{
	inherited::handle_event(ev);
	_state=state_idle;
}

void load::handle_event(events::ramfetch& ev)
{
	// A Message_RAMFetch will be a User_Message_Acknowledge if it has
	// been delivered to this component.
	// It is acted upon only if it was the first Message_RAMFetch
	// following a Message_DataSave.
	if (_state==state_ramfetch_first)
	{
		events::datasave ev2(*this,swi::User_Message,_datasave_block);
		ev2.reply(temp_pathname());
		_state=state_idle;
	}
}

void load::handle_event(events::ramtransmit& ev)
{
	// A Message_RAMTransmit is acted upon in response to a Message_RAMFetch.
	if ((_state==state_ramfetch_first)||(_state==state_ramfetch))
	{
		put_block(ev.buffer_size());
		if (ev.buffer_size()==_lsize)
		{
			_ldata=0;
			_lsize=0;
			get_block(&_ldata,&_lsize);
			ev.reply(_ldata,_lsize);
			_state=state_ramfetch;
		}
		else
		{
			finish();
			events::loaded ev(*this,*this);
			ev.post();
			remove();
			_state=state_idle;
		}
	}
}

load& load::allow_ram_transfer(bool value)
{
	_allow_ram_transfer=value;
	return *this;
}

void load::put_file(const string& pathname,size_type estsize)
{
	start(estsize);
	int fhandle;
	os::OS_Find(0x4f,pathname.c_str(),0,&fhandle);
	bool eof;
	os::OS_Args5(fhandle,&eof);
	while (!eof)
	{
		_ldata=0;
		_lsize=0;
		get_block(&_ldata,&_lsize);
		unsigned int excess=0;
		os::OS_GBPB4(fhandle,_ldata,_lsize,&excess,0);
		put_block(_lsize-excess);
		os::OS_Args5(fhandle,&eof);
	}
	os::OS_Find0(fhandle);
	finish();
}

} /* namespace transfer */
} /* namespace rtk */
