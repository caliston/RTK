// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#include "rtk/graphics/gcontext.h"
#include "rtk/desktop/card_layout.h"

namespace rtk {
namespace desktop {

card_layout::card_layout():
	_component(0)
{}

card_layout::~card_layout()
{
	while (_components.begin()!=_components.end())
	{
		_components.begin()->second->remove();
	}
}

box card_layout::auto_bbox() const
{
	// Reset baseline sets.
	_xbs=xbaseline_set();
	_ybs=ybaseline_set();

	// Add the minimum bounding box of each card to both baseline sets.
	for (map<string,component*>::const_iterator i=_components.begin();
		i!=_components.end();++i)
	{
		component* c=(*i).second;
		box mcbbox=c->min_bbox();
		_xbs.add(mcbbox,c->xbaseline());
		_ybs.add(mcbbox,c->ybaseline());
	}

	// Construct minimum bounding box, with respect to top left-hand
	// corner of layout.
	box abbox(0,-_ybs.ysize(),_xbs.xsize(),0);

	// Translate to external origin and return.
	abbox-=external_origin(abbox,xbaseline_left,ybaseline_top);
	return abbox;
}

component* card_layout::find(const point& p) const
{
	return _component;
}

box card_layout::bbox() const
{
	return _bbox;
}

void card_layout::resize() const
{
	for (map<string,component*>::const_iterator i=_components.begin();
		i!=_components.end();++i)
	{
		component* c=(*i).second;
		c->resize();
	}
	inherited::resize();
}

void card_layout::reformat(const point& origin,const box& pbbox)
{
	// Fit bounding box to parent.
	box bbox=fit(pbbox);

	// Update origin and bounding box of this component, force redraw
	// if necessary.  (This must happen before reformat() is called for
	// any children.)
	bool moved=(origin!=this->origin())||(bbox!=this->bbox());
	if (moved) force_redraw(true);
	_bbox=bbox;
	inherited::reformat(origin,bbox);
	if (moved) force_redraw(true);

	// If a card is selected then reformat the content of that card.
	if (_component)
	{
		// Calculate offset from top left-hand corner of card to
		// origin of child.
		int xoffset=_xbs.offset(xbaseline_left,
			_component->xbaseline(),_bbox.xsize());
		int yoffset=_ybs.offset(ybaseline_bottom,
			_component->ybaseline(),_bbox.ysize());
		point coffset(xoffset,yoffset);

		// Calculate origin of cell with respect to origin of layout.
		point cpos(_bbox.xminymin()+coffset);

		// Reformat child.
		_component->reformat(cpos,_bbox-cpos);
	}
	inherited::reformat(origin,bbox);
}

void card_layout::unformat()
{
	for (map<string,component*>::iterator i=_components.begin();
		i!=_components.end();++i)
	{
		(*i).second->unformat();
	}
	inherited::unformat();
}

void card_layout::redraw(gcontext& context,const box& clip)
{
	if (_component)
	{
		point origin=_component->origin();
		context+=origin;
		_component->redraw(context,clip+origin);
		context-=origin;
	}
	inherited::redraw(context,clip);
}

void card_layout::remove_notify(component& c)
{
	// If c is currently selected then deselect it.
	// (The tag remains selected even though the child is not.)
	if (_component==&c)
	{
		_component->unformat();
		_component=0;
		force_redraw();
		invalidate();
	}

	// If c is a child then remove it.
	// (The layout is invalidated because its dimensions may change when
	// the child is removed, even if the child was not previously selected.)
	map<string,component*>::iterator f=_components.begin();
	while ((f!=_components.end())&&((*f).second!=&c)) ++f;
	if (f!=_components.end())
	{
		_components.erase(f);
		invalidate();
	}
}

card_layout& card_layout::add(component& c,const string& tag)
{
	// Do nothing unless the component associated with the tag is to change.
	if (_components[tag]!=&c)
	{
		// If the tag already in use then remove the associated child.
		if (_components[tag]) _components[tag]->remove();

		// Associate child with tag.
		_components[tag]=&c;
		link_child(c);
		invalidate();

		// If the tag is currently selected then select the child too.
		if (tag==_tag)
		{
			_component=&c;
			force_redraw();
			invalidate();
		}
	}
	return *this;
}

card_layout& card_layout::select(const string& tag)
{
	// Change tag.
	_tag=tag;

	// Search for card corresponding to new tag.
	map<string,component*>::iterator f=_components.find(tag);
	component* c=(f!=_components.end())?(*f).second:0;

	// Do nothing unless selected component is to change.
	if (c!=_component)
	{
		if (_component) _component->unformat();
		_component=c;
		force_redraw();
		invalidate();
	}
	return *this;
}

}; /* namespace desktop */
}; /* namespace rtk */
