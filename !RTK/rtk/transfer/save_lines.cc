// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2004 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#include "rtk/transfer/save_lines.h"

namespace rtk {
namespace transfer {

std::string save_lines::null_source::operator()()
{
	return std::string();
}

bool save_lines::null_source::eof() const
{
	return true;
}

void save_lines::null_source::reset()
{}

save_lines::size_type save_lines::null_source::estsize() const
{
	return 0;
}

save_lines::save_lines():
	_source(new null_source),
	_eol(false),
	_final_newline(true)
{}

save_lines::~save_lines()
{
	delete _source;
}

void save_lines::start()
{
	_source->reset();
	_eol=false;
}

void save_lines::get_block(const void** data,size_type* count)
{
	if (_eol)
	{
		// A line has been sent but the corresponding newline has not.
		static char linefeed='\n';
		if (data) *data=&linefeed;
		if (count) *count=1;
		_eol=false;
	}
	else if (!_source->eof())
	{
		// Fetch line from source.
		const std::string& s=(*_source)();

		// Decide whether line is terminated by a newline character.
		bool has_newline=_final_newline||!_source->eof();

		if (s.length())
		{
			// Line was not empty.
			if (data) *data=s.data();
			if (count) *count=s.length();
			_eol=has_newline;
		}
		else if (has_newline)
		{
			// Line was empty but has a newline.
			// Cannot return a block of length 0 (because that would
			// terminate the save operation), so must return a newline
			// now.
			static char linefeed='\n';
			if (data) *data=&linefeed;
			if (count) *count=1;
			_eol=false;
		}
		else
		{
			// Line was empty and does not have a newline.  It must be
			// the final line, so finish by returning an empty block.
			if (data) *data=0;
			if (count) *count=0;
		}
	}
	else
	{
		// If finished then return an empty block.
		if (data) *data=0;
		if (count) *count=0;
	}
}

void save_lines::finish()
{}

save_lines::size_type save_lines::estsize()
{
	return _source->estsize();
}

save_lines& save_lines::clear()
{
	basic_source* null=new null_source;
	delete _source;
	_source=null;
	return *this;
}

bool save_lines::final_newline() const
{
	return _final_newline;
}

save_lines& save_lines::final_newline(bool value)
{
	_final_newline=value;
	return *this;
}

}; /* namespace transfer */
}; /* namespace rtk */
