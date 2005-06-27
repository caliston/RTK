// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003-2005 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#include "rtk/graphics/font.h"
#include "rtk/graphics/vdu_gcontext.h"
#include "rtk/os/os.h"
#include "rtk/os/wimp.h"

namespace rtk {
namespace graphics {

vdu_gcontext::vdu_gcontext(const point& origin,bool update):
	gcontext(origin,update)
{}

vdu_gcontext::~vdu_gcontext()
{
	if (_current==this) current(0);
}

vdu_gcontext* vdu_gcontext::_current=0;

vdu_gcontext* vdu_gcontext::current()
{
	return _current;
}

void vdu_gcontext::current(vdu_gcontext* current)
{
	if (current!=_current)
	{
		if (_current) _current->deactivate();
		_current=current;
		if (_current) _current->activate();
	}
}

void vdu_gcontext::plot(int code,const point& p)
{
	current(this);
	os::OS_Plot(code,origin()+p);
}

void vdu_gcontext::draw(const char* s,const point& p)
{
	current(this);
	os::Wimp_TextOp2(s,origin()+p);
}

void vdu_gcontext::draw(const font& f,const char* s,const point& p)
{
	current(this);
	f.paint(s,origin()+p);
}

void vdu_gcontext::fcolour_notify(int fcolour)
{
	os::Wimp_SetColour(fcolour);
	os::Wimp_TextColour(fcolour);
	os::Wimp_SetFontColours(bcolour(),fcolour);
}

void vdu_gcontext::bcolour_notify(int bcolour)
{
	os::Wimp_SetColour(bcolour|0x80);
	os::Wimp_TextColour(bcolour|0x80);
	os::Wimp_SetFontColours(bcolour,fcolour());
}

void vdu_gcontext::activate()
{
	os::Wimp_SetColour(fcolour());
	os::Wimp_SetColour(bcolour()|0x80);
	os::Wimp_TextColour(fcolour());
	os::Wimp_TextColour(bcolour()|0x80);
	os::Wimp_SetFontColours(bcolour(),fcolour());
}

void vdu_gcontext::deactivate()
{}

}; /* namespace graphics */
}; /* namespace rtk */
