// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#include "rtk/graphics/font.h"
#include "rtk/graphics/vdu_gcontext.h"
#include "rtk/os/os.h"
#include "rtk/os/wimp.h"

namespace rtk {
namespace graphics {

vdu_gcontext::vdu_gcontext(const point& _origin):
	gcontext(_origin)
{}

vdu_gcontext::~vdu_gcontext()
{}

vdu_gcontext* vdu_gcontext::_current=0;

void vdu_gcontext::current(vdu_gcontext* current)
{
	if (current!=_current)
	{
	current=current;
		if (current)
		{
			os::Wimp_SetColour(current->fcolour());
			os::Wimp_SetColour(current->bcolour()|0x80);
			os::Wimp_TextColour(current->fcolour());
			os::Wimp_TextColour(current->bcolour()|0x80);
		}
	}
}

void vdu_gcontext::plot(int code,const point& p)
{
	current(this);
	os::OS_Plot(code,origin()+p);
}

void vdu_gcontext::draw(const string &s,const point& p)
{
	current(this);
	os::Wimp_TextOp2(s.c_str(),origin()+p);
}

void vdu_gcontext::draw(const font& f,const string& s,const point& p)
{
	current(this);
	f.paint(s,origin()+p);
}

void vdu_gcontext::fcolour_notify(int fcolour)
{
	os::Wimp_SetColour(fcolour);
	os::Wimp_TextColour(fcolour);
}

void vdu_gcontext::bcolour_notify(int bcolour)
{
	os::Wimp_SetColour(bcolour|0x80);
	os::Wimp_TextColour(bcolour|0x80);
}

}; /* namespace graphics */
}; /* namespace rtk */
