// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#include "rtk/transfer/save_linelist.h"

namespace rtk {
namespace transfer {

save_linelist::save_linelist():
	_linelist(&_default_linelist),
	_pos(_default_linelist.end()),
	_eol(false)
{}

save_linelist::~save_linelist()
{}

void save_linelist::start()
{
	_pos=_linelist->begin();
	_eol=false;
}

void save_linelist::get_block(const void** data,size_type* count)
{
	if (_eol)
	{
		static char linefeed='\n';
		if (data) *data=&linefeed;
		if (count) *count=1;
		_eol=false;
	}
	else if (_pos!=_linelist->end())
	{
		const string& s=*_pos++;
		if (s.length())
		{
			if (data) *data=s.data();
			if (count) *count=s.length();
			_eol=true;
		}
		else
		{
			// Cannot return a block of length 0 (because that would
			// terminate the save operation), so must return a newline
			// now.
			static char linefeed='\n';
			if (data) *data=&linefeed;
			if (count) *count=1;
			_eol=false;
		}
	}
	else
	{
		// If finished then return an empty block.
		if (data) *data=0;
		if (count) *count=0;
	}
}

void save_linelist::finish()
{}

save_linelist::size_type save_linelist::estsize()
{
	size_type size=0;
	for (list<string>::const_iterator i=_linelist->begin();
		i!=_linelist->end();++i)
	{
		const string& s=*i;
		size+=s.length()+1;
	}
	return size;
}

save_linelist& save_linelist::linelist(list<string>& linelist)
{
	_linelist=&linelist;
	return *this;
}

}; /* namespace transfer */
}; /* namespace rtk */
