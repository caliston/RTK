// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#include <stdexcept>
#include <algorithm>

#include "rtk/graphics/gcontext.h"
#include "rtk/desktop/progress_bar.h"

namespace rtk {
namespace desktop {

progress_bar::progress_bar():
	_numerator(0),
	_denominator(1)
{
	link_child(_background);
	link_child(_bar);

	_background.validation("R2");
	_background.border(true);
	_background.fill(true);
	_background.fcolour(7).bcolour(1);
	_background.xbaseline(xbaseline_left);
	_background.ybaseline(ybaseline_top);

	_bar.border(false);
	_bar.fill(true);
	_bar.fcolour(10).bcolour(10);
	_bar.xbaseline(xbaseline_left);
	_bar.ybaseline(ybaseline_top);

	yfit(0);
}

progress_bar::~progress_bar()
{}

box progress_bar::auto_bbox() const
{
	box abbox(0,-40,128,0);
	abbox-=external_origin(abbox,xbaseline_left,ybaseline_top);
	return abbox;
}

box progress_bar::bbox() const
{
	return _bbox;
}

void progress_bar::resize() const
{
	_background.resize();
	_bar.resize();
	inherited::resize();
}

void progress_bar::reformat(const point& origin,const box& pbbox)
{
	// Fit bounding box to parent.
	box bbox=fit(pbbox);

	// Update origin and bounding box of this component, force redraw
	// if necessary.  (This must happen before reformat() is called for
	// the children.)
	bool moved=(origin!=this->origin())||(bbox!=this->bbox());
	if (moved) force_redraw(true);
	_bbox=bbox;
	inherited::reformat(origin,bbox);
	if (moved) force_redraw(true);

	// Get border box.
	box bdrbox(_background.border_box());

	// Calculate outer origin (includes border).
	point ocpos(_bbox.xminymax());

	// Calculate outer bounding box (includes border).
	box ocbbox(_bbox-_bbox.xminymax());

	// Calculate inner origin (excludes border).
	point icpos(ocpos-bdrbox.xminymax());

	// Calculate inner bounding box (excludes border).
	box icbbox((ocbbox-bdrbox)-bdrbox.xminymax());

	// Calculate progress bounding box.
	box pcbbox(0,-icbbox.ysize(),
		(icbbox.xsize()*min(_numerator,_denominator))/_denominator,0);

	// Reformat icons.
	_background.reformat(ocpos,ocbbox);
	_bar.reformat(icpos,pcbbox);
}

void progress_bar::unformat()
{
	_background.unformat();
	_bar.unformat();
	inherited::unformat();
}

void progress_bar::redraw(gcontext& context,const box& clip)
{
	{
		point origin=_background.origin();
		context+=origin;
		_background.redraw(context,clip-origin);
		context-=origin;
	}
	{
		point origin=_bar.origin();
		context+=origin;
		_bar.redraw(context,clip-origin);
		context-=origin;
	}
	inherited::redraw(context,clip);
}

progress_bar& progress_bar::numerator(unsigned int numerator)
{
	_numerator=numerator;
	invalidate();
	return *this;
}

progress_bar& progress_bar::denominator(unsigned int denominator)
{
	if (!denominator)
		throw std::invalid_argument("denominator cannot be zero");
	_denominator=denominator;
	invalidate();
	return *this;
}

progress_bar& progress_bar::fcolour(int fcolour)
{
	_bar.fcolour(fcolour);
	_bar.bcolour(fcolour);
	return *this;
}

progress_bar& progress_bar::bcolour(int bcolour)
{
	_background.bcolour(bcolour);
	return *this;
}

}; /* namespace desktop */
}; /* namespace rtk */
