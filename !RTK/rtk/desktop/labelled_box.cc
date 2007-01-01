// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2007 Alex Waugh.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#include "rtk/graphics/gcontext.h"
#include "rtk/desktop/labelled_box.h"

namespace rtk {
namespace desktop {

labelled_box::labelled_box():
	_content(0)
{
	_border.border(true);
	_border.fcolour(7);
	_border.validation("R4");
	_border.bcolour(1);
	_border.xbaseline(xbaseline_left);
	_border.ybaseline(ybaseline_top);
	link_child(_border);

	// The label needs to be text and sprite, but without a valid sprite,
	// to ensure the background is correct
	_label.text_and_sprite(true);
	_label.vcentre(true);
	_label.fcolour(7);
	_label.bcolour(1);
	_label.xbaseline(xbaseline_left);
	_label.ybaseline(ybaseline_top);
	link_child(_label);
}

labelled_box::~labelled_box()
{
	remove();
}

labelled_box& labelled_box::add(component& c)
{
	if (_content) _content->remove();
	_content=&c;
	link_child(c);
	invalidate();
	return *this;
}

string labelled_box::text() const
{
	return _label.text();
}

labelled_box& labelled_box::text(const string& text,icon::size_type capacity)
{
	_label.text(text,capacity);
	return *this;
}

box labelled_box::auto_bbox() const
{
	box abbox;
	// Must be big enough to hold the child
	if (_content) abbox=_content->min_bbox();

	// Add space for the border and label
	abbox+=box(-24,-16,24,std::max(16,_label.min_bbox().ysize()-8));

	// Ensure wide enough for the label
	int labelwidth=_label.min_bbox().xsize()+52;
	if (abbox.xsize()<labelwidth) abbox.xmax(abbox.xmin()+labelwidth);

	return abbox;
}

box labelled_box::bbox() const
{
	return _bbox;
}

void labelled_box::resize() const
{
	if (_content) _content->resize();
	_border.resize();
	_label.resize();
	inherited::resize();
}

void labelled_box::reformat(const point& origin,const box& pbbox)
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

	// Reformat icons.
	int halflabelheight=_label.min_bbox().ysize()/2-4;

	// Put the border right up to the edge, except leaving some space
	// at the top for the label
	box borderbbox=bbox-box(0,0,0,halflabelheight);
	_border.reformat(point(0,0),borderbbox);

	// Set the origin of the label without allowing it to expand
	_label.reformat(bbox.xminymax()+point(26,4),_label.min_bbox());

	// Leave a larger gap at the top if there is a label
	box contentbbox=borderbbox-box(-24,-16,24,std::max(16,halflabelheight));
	if (_content) _content->reformat(point(0,0),contentbbox);
}

void labelled_box::unformat()
{
	if (_content) _content->unformat();
	_border.unformat();
	_label.unformat();
	inherited::unformat();
}

void labelled_box::redraw(gcontext& context,const box& clip)
{
	point origin=_border.origin();
	context+=origin;
	_border.redraw(context,clip-origin);
	_label.redraw(context,clip-origin);
	if (_content) _content->redraw(context,clip-origin);
	context-=origin;
	inherited::redraw(context,clip);
}

} /* namespace desktop */
} /* namespace rtk */
