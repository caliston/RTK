// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#include "rtk/graphics/point.h"
#include "rtk/graphics/font.h"
#include "rtk/os/font.h"
#include "rtk/os/colourtrans.h"

namespace rtk {
namespace graphics {

struct font::_font
{
	int refcount;
	int handle;
};

font::font(const string& id,int xsize,int ysize):
	_f(0)
{
	try
	{
		_f=new _font;
		os::Font_FindFont(id.c_str(),xsize,ysize,0,0,&_f->handle,0,0);
	}
	catch (...)
	{
		delete _f;
	}
}

font::font(const font& f):
	_f(f._f)
{
	++_f->refcount;
}

font::~font()
{
	if (!--_f->refcount)
	{
		os::Font_LoseFont(_f->handle);
		delete _f;
	}
}

font& font::operator=(const font& f)
{
	++f._f->refcount;
	if (!--_f->refcount)
	{
		os::Font_LoseFont(_f->handle);
		delete _f;
	}
	_f=f._f;
	return *this;
}

void font::paint(const string& s,const point& p) const
{
	// Note: s.c_str() is used in preference to s.data() because
	// Font_Paint must be able to access the character following
	// the end of the string.
	os::ColourTrans_SetFontColours(_f->handle,0xffffff00,0x00000000,14,0,0,0);
	os::Font_Paint(_f->handle,s.c_str(),0x190,p,0,0,s.length());
}

int font::width(const string& s) const
{
	// Note: s.c_str() is used in preference to s.data() because
	// Font_Paint must be able to access the character following
	// the end of the string.
	const int maxint=0xffffff;
	point size;
	os::Font_ScanString(_f->handle,s.c_str(),0x180,point(maxint,maxint),0,0,
		s.length(),0,&size,0);
	return size.x()/400;
}

}; /* namespace graphics */
}; /* namespace rtk */
