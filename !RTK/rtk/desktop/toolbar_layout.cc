// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2004 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#include "rtk/graphics/gcontext.h"
#include "rtk/desktop/toolbar_layout.h"
#include "rtk/desktop/basic_window.h"

namespace rtk {
namespace desktop {

toolbar_layout::toolbar_layout():
	_content(0)
{}

toolbar_layout::~toolbar_layout()
{
	// Remove toolbars.
	for (vector<toolbar_type>::iterator i=_toolbars.begin();
		i!=_toolbars.end();++i)
	{
		if (basic_window* w=i->w) w->remove();
	}

	// Remove work area.
	if (_content) _content->remove();

	remove();
}

toolbar_layout& toolbar_layout::add(component& c)
{
	if (_content) _content->remove();
	_content=&c;
	link_child(c);
	invalidate();
	return *this;
}

toolbar_layout& toolbar_layout::add_toolbar(basic_window& w,align_type align,
	size_type index)
{
	if (index==npos) index=_toolbars.size();
	if (index>=_toolbars.size()) _toolbars.resize(index+1);
	if (_toolbars[index].w) _toolbars[index].w->remove();
	_toolbars[index].w=&w;
	_toolbars[index].align=align;
	link_child(w);
	invalidate();
	return *this;
}

component* toolbar_layout::find(const point& p) const
{
	component* f=0;

	// Check whether point is in a toolbar first.
	vector<toolbar_type>::const_iterator i=_toolbars.begin();
	while (!f&&i!=_toolbars.end())
	{
		basic_window* w=i->w;
		if (w&&((p-w->origin())<=w->bbox()))
		{
			f=w;
		}
		++i;
	}

	// If not found, check whether point is in work area.
	if (!f&&_content&&((p-_content->origin())<=_content->bbox()))
	{
		f=_content;
	}

	return f;
}

box toolbar_layout::auto_bbox() const
{
	box abbox;

	// Add work area.
	if (_content)
	{
		abbox=_content->min_bbox();
	}

	// Add toolbars, starting from the inside and working outwards.
	for (vector<toolbar_type>::const_reverse_iterator i=_toolbars.rbegin();
		i!=_toolbars.rend();++i)
	{
		if (basic_window* w=i->w)
		{
			box mcbbox=w->min_bbox();
			switch (i->align)
			{
			case align_left:
				abbox.extend_left(mcbbox);
				break;
			case align_right:
				abbox.extend_right(mcbbox);
				break;
			case align_top:
				abbox.extend_up(mcbbox);
				break;
			case align_bottom:
				abbox.extend_down(mcbbox);
				break;
			}
		}
	}

	abbox+=internal_origin(abbox,xbaseline_text,ybaseline_text);
	return abbox;
}

box toolbar_layout::bbox() const
{
	return _bbox;
}

void toolbar_layout::reformat(const point& origin,const box& pbbox)
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

	// Initialise tbox, the area available for placing toolbars.
	// This is initially eqaul to the area within this component which is
	// also within the visible area of the window.
	component* p=parent();
	box tbox=(p->bbox()-origin)&_bbox;

	// Initialise cbox, the area available for placing all children.
	box cbox=_bbox;

	// Reformat toolbars, starting from the outside and working in.
	for (vector<toolbar_type>::iterator i=_toolbars.begin();
		i!=_toolbars.end();++i)
	{
		if (basic_window* w=i->w)
		{
			box mcbbox(w->min_bbox());

			// Calculate cbbox, the bounding box wrt the layout origin
			// that will contain the toolbar.
			// Also calculate dbox, the difference which must be applied
			// to cbox and dbox to make space for the toolbar.
			box cbbox;
			box dbox;
			switch (i->align)
			{
			case align_left:
				cbbox=box(tbox.xmin(),tbox.ymin(),tbox.xmin()+mcbbox.xsize(),
					tbox.ymax());
				dbox=box(mcbbox.xsize(),0,0,0);
				break;
			case align_right:
				cbbox=box(tbox.xmax()-mcbbox.xsize(),tbox.ymin(),
					tbox.xmax(),tbox.ymax());
				dbox=box(0,0,-mcbbox.xsize(),0);
				break;
			case align_bottom:
				cbbox=box(tbox.xmin(),tbox.ymin(),tbox.xmax(),
					tbox.ymin()+mcbbox.ysize());
				dbox=box(0,mcbbox.ysize(),0,0);
				break;
			case align_top:
				cbbox=box(tbox.xmin(),tbox.ymax()-mcbbox.ysize(),tbox.xmax(),
					tbox.ymax());
				dbox=box(0,0,0,-mcbbox.ysize());
				break;
			}

			// Construct baseline sets just for this child.
			xbaseline_set xbs;
			ybaseline_set ybs;
			xbs.add(cbbox,w->xbaseline());
			ybs.add(cbbox,w->ybaseline());
	
			// Calculate offset from top left-hand corner of cbox to
			// origin of child.
			int xoffset=xbs.offset(xbaseline_left,
				w->xbaseline(),cbbox.xsize());
			int yoffset=ybs.offset(ybaseline_top,
				w->ybaseline(),cbbox.ysize());
			point coffset(xoffset,yoffset);

			// Calculate origin of cell with respect to origin of layout.
			point cpos(cbbox.xminymax()+coffset);

			// Reformat child.
			w->reformat(cpos,cbbox-cpos);

			// Reduce size of tbox and cbox to make space for toolbar.
			tbox+=dbox;
			cbox+=dbox;
		}
	}

	// Reformat work area.
	if (component* c=_content)
	{
		box mcbbox(c->min_bbox());

		// Construct baseline sets just for this child.
		xbaseline_set xbs;
		ybaseline_set ybs;
		xbs.add(mcbbox,c->xbaseline());
		ybs.add(mcbbox,c->ybaseline());

		// Calculate offset from top left-hand corner of cbox to
		// origin of child.
		int xoffset=xbs.offset(xbaseline_left,
			c->xbaseline(),cbox.xsize());
		int yoffset=ybs.offset(ybaseline_top,
			c->ybaseline(),cbox.ysize());
		point coffset(xoffset,yoffset);

		// Calculate origin of cell with respect to origin of layout.
		point cpos(cbox.xminymax()+coffset);

		// Reformat child.
		c->reformat(cpos,cbox-cpos);
	}
}

void toolbar_layout::unformat()
{
	// Unformat work area.
	if (component* c=_content) c->unformat();

	// Unformat toolbars.
	for (vector<toolbar_type>::iterator i=_toolbars.begin();
		i!=_toolbars.end();++i)
	{
		if (basic_window* w=i->w) w->unformat();
	}
}

void toolbar_layout::redraw(gcontext& context,const box& clip)
{
	// Redraw work area.
	if (component* c=_content)
	{
		point cpos=c->origin();
		context+=cpos;
		c->redraw(context,clip-cpos);
		context-=cpos;
	}

	// Redraw toolbars.
	for (vector<toolbar_type>::iterator i=_toolbars.begin();
		i!=_toolbars.end();++i)
	{
		if (basic_window* w=i->w)
		{
			point cpos=w->origin();
			context+=cpos;
			w->redraw(context,clip-cpos);
			context-=cpos;
		}
	}

	inherited::redraw(context,clip);
}

void toolbar_layout::remove_notify(component& c)
{
	vector<toolbar_type>::iterator f=_toolbars.begin();
	while ((f!=_toolbars.end())&&(f->w!=&c)) ++f;
	if (f!=_toolbars.end())
	{
		f->w=0;
		invalidate();
	}
	else if (_content==&c)
	{
		_content=0;
		invalidate();
	}
}

toolbar_layout::toolbar_type::toolbar_type():
	w(0),
	align(align_top)
{}

}; /* namespace desktop */
}; /* namespace rtk */
