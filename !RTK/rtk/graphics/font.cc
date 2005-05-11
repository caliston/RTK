// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003-2004 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#include "rtk/graphics/point.h"
#include "rtk/graphics/font.h"

#include "rtk/os/font.h"
#include "rtk/os/colourtrans.h"
#include "rtk/os/wimp.h"

namespace rtk {
namespace graphics {

class font::basic_font
{
	friend class font;
private:
	unsigned int _refcount;
public:
	basic_font(unsigned int refcount);
	virtual ~basic_font();
	virtual int handle() const=0;
	static basic_font* select_default_font(default_font_type type);
};

class font::desktop_font:
	public font::basic_font
{
public:
	desktop_font();
	virtual int handle() const;
};

class font::symbol_font:
	public font::basic_font
{
public:
	symbol_font();
	virtual int handle() const;
};

class font::custom_font:
	public font::basic_font
{
private:
	int _handle;
public:
	custom_font(const string& id,int xsize,int ysize);
	virtual ~custom_font();
	virtual int handle() const;
};

font::basic_font::basic_font(unsigned int refcount):
	_refcount(refcount)
{}

font::basic_font::~basic_font()
{}

font::basic_font* font::basic_font::select_default_font(default_font_type type)
{
	static desktop_font df;
	static symbol_font sf;
	switch (type)
	{
	case font_desktop:
		return &df;
	case font_symbol:
		return &sf;
	default:
		return 0;
	}
}

font::desktop_font::desktop_font():
	basic_font(1)
{}

int font::desktop_font::handle() const
{
	int handle=0;
	os::Wimp_ReadSysInfo(8,&handle,0);
	return handle;
}

font::symbol_font::symbol_font():
	basic_font(1)
{}

int font::symbol_font::handle() const
{
	int handle=0;
	os::Wimp_ReadSysInfo(8,0,&handle);
	return handle;
}

font::custom_font::custom_font(const string& id,int xsize,int ysize):
	basic_font(0),
	_handle(0)
{
	os::Font_FindFont(id.c_str(),xsize,ysize,0,0,&_handle,0,0);
}

font::custom_font::~custom_font()
{
	os::Font_LoseFont(_handle);
}

int font::custom_font::handle() const
{
	return _handle;
}

font::font(const string& id,int xsize,int ysize):
	_f(new custom_font(id,xsize,ysize))
{
	++_f->_refcount;
}

font::font(default_font_type type):
	_f(basic_font::select_default_font(type))
{
	++_f->_refcount;
}

font::font(const font& f):
	_f(f._f)
{
	++_f->_refcount;
}

font::~font()
{
	if (!--_f->_refcount) delete _f;
}

font& font::operator=(const font& f)
{
	++f._f->_refcount;
	if (!--_f->_refcount) delete _f;
	_f=f._f;
	return *this;
}

void font::paint(const string& s,const point& p) const
{
	// Note: s.c_str() is used in preference to s.data() because
	// Font_Paint must be able to access the character following
	// the end of the string.
	int handle=_f->handle();
	os::Font_Paint(handle,s.c_str(),0x190,p,0,0,s.length());
}

int font::width(const char* s,unsigned int length) const
{
	int handle=_f->handle();
	point size;
	os::Font_ScanString(handle,s,0x180,point(-1,0),0,0,length,0,&size,0);
	return size.x()/400;
}

int font::handle() const
{
	return _f->handle();
}

}; /* namespace graphics */
}; /* namespace rtk */
