// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2004 Graham Shaw.
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
#include "rtk/transfer/load_lines.h"

namespace rtk {
namespace transfer {

std::string load_lines::null_sink::back() const
{
	return std::string();
}

void load_lines::null_sink::push_back(const std::string& line)
{}

void load_lines::null_sink::pop_back()
{}

void load_lines::null_sink::clear()
{}

load_lines::load_lines(size_type buffer_size):
	load(),
	_sink(new null_sink),
	_buffer(new char[buffer_size]),
	_buffer_size(buffer_size),
	_newline(true)
{}

load_lines::~load_lines()
{
	delete[] _buffer;
}

void load_lines::start(size_type estsize)
{
	_sink->clear();
	_newline=true;
}

void load_lines::get_block(void** data,size_type* size)
{
	if (data) *data=_buffer;
	if (size) *size=_buffer_size;
}

void load_lines::put_block(size_type count)
{
	char* p=_buffer;
	char* q=p;
	for (size_type i=0;i!=count;++i)
	{
		if (*p++=='\n')
		{
			std::string s;
			if (!_newline)
			{
				s+=_sink->back();
				_sink->pop_back();
			}
			s+=std::string(q,p-1);
			_sink->push_back(s);
			q=p;
			_newline=true;
		}
	}
	if (p!=q)
	{
		std::string s;
		if (!_newline)
		{
			s+=_sink->back();
			_sink->pop_back();
		}
		s+=std::string(q,p);
		_sink->push_back(s);
		q=p;
		_newline=false;
	}
}

void load_lines::finish()
{}

load_lines& load_lines::clear()
{
	basic_sink* null=new null_sink;
	delete _sink;
	_sink=null;
	return *this;
}

bool load_lines::final_newline() const
{
	return _final_newline;
}

load_lines& load_lines::final_newline(bool value)
{
	_final_newline=value;
	return *this;
}

} /* namespace transfer */
} /* namespace rtk */
