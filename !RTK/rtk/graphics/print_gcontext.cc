// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2005 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#include "rtk/graphics/print_gcontext.h"

#include "rtk/os/pdriver.h"

namespace rtk {
namespace graphics {

print_gcontext::print_gcontext(const point& _origin,int handle):
	vdu_gcontext(_origin,false),
	_handle(handle)
{}

print_gcontext::~print_gcontext()
{}

void print_gcontext::activate()
{
	if (_handle)
	{
		os::PDriver_SelectJob(_handle,0,0);
	}

	inherited::activate();
}

void print_gcontext::deactivate()
{
	inherited::deactivate();

	// As a precaution, check that the file handles (of this graphics
	// context and the current print job) match before suspending the
	// current print job.
	int handle=0;
	os::PDriver_CurrentJob(&handle);
	if (_handle&&(handle==_handle))
	{
		os::PDriver_SelectJob(0,0,0);
	}
}

}; /* namespace graphics */
}; /* namespace rtk */
