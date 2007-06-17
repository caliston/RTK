// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2007 Alex Waugh.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#include "rtk/desktop/string_set.h"
#include "rtk/desktop/application.h"

namespace rtk {
namespace desktop {


string_set::string_set()
{
	_label.xbaseline(xbaseline_left);
	_label.ybaseline(ybaseline_centre);
	_value.xbaseline(xbaseline_left);
	_value.ybaseline(ybaseline_centre);
	_menuicon.xbaseline(xbaseline_left);
	_menuicon.ybaseline(ybaseline_centre);
	_units.xbaseline(xbaseline_left);
	_units.ybaseline(ybaseline_centre);

	_menuicon.button(3);
	_menuicon.text_and_sprite(true);
	_menuicon.validation("R5;Sgright,pgright");

	link_child(_label);
	link_child(_units);
	link_child(_menuicon);
	link_child(_value);
}

string_set::~string_set()
{
	for (unsigned i=0; i<_menu.cells(); i++)
	{
		delete &_menu[i];
	}
	_menu.cells(0);
	remove();
}

box string_set::auto_bbox() const
{
	box lbbox=_label.min_bbox();
	box vbbox=_value.min_bbox();
	box mbbox=_menuicon.min_bbox();
	box nbbox=_units.min_bbox();

	ybaseline_set ybs;
	ybs.add(lbbox,_label.ybaseline());
	ybs.add(vbbox,_value.ybaseline());
	ybs.add(mbbox,_menuicon.ybaseline());
	ybs.add(nbbox,_units.ybaseline());

	box abbox;
	abbox.ymax(ybs.ysize());
	// The width should be enough to include all components, plus a gap
	// of 8 units between the value and the menu icon
	abbox.xmax(lbbox.xsize()+vbbox.xsize()+8+mbbox.xsize()+nbbox.xsize());

	// Translate to external origin and return.
	abbox-=external_origin(abbox,xbaseline_left,ybaseline_bottom);
	return abbox;
}

box string_set::bbox() const
{
	return _bbox;
}

void string_set::resize() const
{
	_label.resize();
	_value.resize();
	_menuicon.resize();
	_units.resize();

	inherited::resize();
}

void string_set::reformat(const point& origin,const box& pbbox)
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

	box lbbox=_label.min_bbox();
	box vbbox=_value.min_bbox();
	box mbbox=_menuicon.min_bbox();
	box nbbox=_units.min_bbox();

	// The label, down, up and units icons should remain minimum size
	// The value icon should fill all remaining space
	vbbox.xmax(vbbox.xmin()+bbox.xsize()-(lbbox.xsize()+mbbox.xsize()+nbbox.xsize()));

	int offset=bbox.xmin();
	_label.reformat(point(offset,0),lbbox);
	offset+=lbbox.xsize();
	_value.reformat(point(offset,0),vbbox);
	offset+=vbbox.xsize()+8;
	_menuicon.reformat(point(offset,0),mbbox);
	offset+=mbbox.xsize();
	_units.reformat(point(offset,0),nbbox);
}

void string_set::unformat()
{
	_label.unformat();
	_value.unformat();
	_menuicon.unformat();
	_units.unformat();
	inherited::unformat();
}

void string_set::redraw(gcontext& context,const box& clip)
{
	_label.redraw(context,clip);
	_value.redraw(context,clip);
	_menuicon.redraw(context,clip);
	_units.redraw(context,clip);
	inherited::redraw(context,clip);
}

void string_set::handle_event(rtk::events::mouse_click& ev)
{
	if (ev.buttons()==2) return;
	if (ev.target()==&_menuicon)
	{
		_menu.redirect(this);
		point pos;
		pos=_menuicon.bbox().xmaxymax();
		point offset=_menu.min_bbox().xminymax();
		_menuicon.parent_application(pos)->add(_menu,pos-offset);
	}
}

void string_set::handle_event(rtk::events::menu_selection& ev)
{
	for (unsigned i=0; i<_menu.cells(); i++)
	{
		if (ev.target()==&_menu[i])
		{
			value(_menu[i].text());
			break;
		}
	}
}

string_set& string_set::add(const std::string& text)
{
	menu_item& m = *new menu_item;
	m.text(text);
	_menu.add(m);
	return *this;
}

string_set& string_set::writable(bool writable)
{
	if (writable)
	{
		_value.validation("Ktar;Pptr_write");
		_value.button(14);
		_value.bcolour(0);
	}
	else
	{
		_value.validation("R2");
		_value.button(0);
		_value.bcolour(1);
	}
	return *this;
}

} /* namespace desktop */
} /* namespace rtk */

