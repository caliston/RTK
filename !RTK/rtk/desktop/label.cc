// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#include "rtk/graphics/gcontext.h"
#include "rtk/os/wimp.h"
#include "rtk/desktop/label.h"

namespace rtk {
namespace desktop {

label::label()
{
	xbaseline(xbaseline_text);
	ybaseline(ybaseline_text);
}

label::label(const string& text):
	_text(text)
{
	xbaseline(xbaseline_text);
	ybaseline(ybaseline_text);
}

label::~label()
{
	remove();
}

box label::min_bbox() const
{
	box bbox=internal_bbox();
	bbox-=external_origin(bbox,xbaseline_text,ybaseline_text);
	return bbox;
}

void label::redraw(gcontext& context,const box& clip)
{
	box ibox=internal_bbox();
	point offset=external_origin(ibox,xbaseline_text,ybaseline_text);
	context.fcolour(7);
	context.bcolour(0);
	context.draw(_text.c_str(),-offset);
}

label& label::text(const string& text)
{
	force_redraw();
	_text=text;
	reformat(origin(),box());
	force_redraw();
	invalidate();
	return *this;
}

box label::internal_bbox() const
{
	int width;
	os::Wimp_TextOp1(_text.c_str(),0,&width);
	return box(0,-8,width,24);
}

}; /* namespace desktop */
}; /* namespace rtk */
