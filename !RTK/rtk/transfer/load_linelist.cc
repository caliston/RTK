// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003 Graham Shaw.
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
#include "rtk/events/save_to_app.h"
#include "rtk/events/save_to_file.h"
#include "rtk/transfer/load_linelist.h"

namespace rtk {
namespace transfer {

load_linelist::load_linelist(size_type buffer_size):
	load(),
	_linelist(&_default_linelist),
	_buffer(new char[buffer_size]),
	_buffer_size(buffer_size),
	_newline(true)
{}

load_linelist::~load_linelist()
{
	delete[] _buffer;
}

void load_linelist::start(size_type estsize)
{
	_linelist->clear();
	_newline=true;
}

void load_linelist::get_block(void** data,size_type* size)
{
	if (data) *data=_buffer;
	if (size) *size=_buffer_size;
}

void load_linelist::put_block(size_type count)
{
	char* p=_buffer;
	char* q=p;
	for (size_type i=0;i!=count;++i)
	{
		if (*p++=='\n')
		{
			if (_newline) _linelist->push_back(string());
			_linelist->back()+=string(q,p-1);
			q=p;
			_newline=true;
		}
	}
	if (p!=q)
	{
		if (_newline) _linelist->push_back(string());
		_linelist->back()+=string(q,p);
		q=p;
		_newline=false;
	}
}

void load_linelist::finish()
{}

load_linelist& load_linelist::linelist(list<string>& linelist)
{
	_linelist=&linelist;
	return *this;
}

}; /* namespace transfer */
}; /* namespace rtk */
