// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#include "rtk/graphics/font.h"
#include "rtk/graphics/gcontext.h"

namespace rtk {
namespace graphics {

gcontext::gcontext(const point& origin):
	_origin(origin)
{}

gcontext::~gcontext()
{}

void gcontext::fcolour(int fcolour)
{
	if (fcolour!=_fcolour)
	{
		fcolour_notify(fcolour);
		_fcolour=fcolour;
	}
}

void gcontext::bcolour(int bcolour)
{
	if (bcolour!=_bcolour)
	{
		bcolour_notify(bcolour);
		_bcolour=bcolour;
	}
}

void gcontext::fcolour_notify(int fcolour)
{}

void gcontext::bcolour_notify(int bcolour)
{}

}; /* namespace graphics */
}; /* namespace rtk */
