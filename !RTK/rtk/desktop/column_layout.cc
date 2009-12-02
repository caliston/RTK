// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003-2005 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#include <algorithm>
#include <functional>

#include "rtk/util/divider.h"
#include "rtk/graphics/gcontext.h"
#include "rtk/desktop/column_layout.h"

namespace rtk {
namespace desktop {

using std::min;
using std::max;
using rtk::util::divider;

column_layout::column_layout(size_type ycells):
	_components(ycells,0),
	_ymax(ycells+1,0),
	_ygap(0)
{}

column_layout::~column_layout()
{
	for (std::vector<component*>::iterator i=_components.begin();
		i!=_components.end();++i)
	{
		if (component* c=*i) c->remove();
	}
	remove();
}

box column_layout::auto_bbox() const
{
	// Determine number of cells.
	size_type ycells=_components.size();

	// Reset x-baseline set.
	_xbs=xbaseline_set();

	// Request min_bbox for each cell.  Incorporate into x-baseline set
	// and total height.
	int ysize=0;
	std::vector<component*>::const_iterator i=_components.begin();
	for (size_type y=0;y!=ycells;++y)
	{
		if (component* c=*i++)
		{
			box mcbbox=c->min_bbox();
			_xbs.add(mcbbox,c->xbaseline());
			ysize+=mcbbox.ysize();
		}
	}

	// Calculate combined width.
	int xsize=_xbs.xsize();

	// Add gaps to total height.
	if (ycells) ysize+=(ycells-1)*_ygap;

	// Add margin to width and height.
	xsize+=_margin.xsize();
	ysize+=_margin.ysize();

	// Construct minimum bounding box, with respect to top left-hand
	// corner of layout.
	box abbox(0,-ysize,xsize,0);

	// Translate to external origin and return.
	abbox-=external_origin(abbox,xbaseline_left,ybaseline_top);
	return abbox;
}

component* column_layout::find(const point& p) const
{
	std::vector<int>::const_iterator yf=
		lower_bound(_ymax.begin(),_ymax.end(),p.y()-1,std::greater<int>());
	if (yf==_ymax.begin()) return 0;
	size_type y=(yf-_ymax.begin())-1;
	component* c=_components[y];
	if (!c) return 0;
	box cbbox=c->bbox()+c->origin();
	if (!(p<=cbbox)) return 0;
	return c;
}

box column_layout::bbox() const
{
	return _bbox;
}

void column_layout::resize() const
{
	for (std::vector<component*>::const_iterator i=_components.begin();
		i!=_components.end();++i)
	{
		if (component* c=*i) c->resize();
	}
	inherited::resize();
}

void column_layout::reformat(const point& origin,const box& pbbox)
{
	// Fit bounding box to parent.
	box bbox=fit(pbbox);

	// Determine number of cells.
	size_type ycells=_components.size();

	// Update origin and bounding box of this component, force redraw
	// if necessary.  (This must happen before reformat() is called for
	// any children.)
	bool moved=(origin!=this->origin())||(bbox!=this->bbox());
	if (moved) force_redraw(true);
	_bbox=bbox;
	inherited::reformat(origin,bbox);
	if (moved) force_redraw(true);

	// Remove margin.
	box ibox(_bbox-_margin);

	box abbox=auto_bbox();

	// Calculate excess.
	int yexcess=_bbox.ysize()-abbox.ysize();
	if (yexcess<0) yexcess=0;
	int yspread=abbox.ysize();
	yspread-=_margin.ysize();
	if (ycells) yspread-=(ycells-1)*_ygap;
	if (yspread<0) yspread=0;
	divider ydiv(yexcess,yspread);

	// Set maximum y-coordinate for each cell.
	int ypos=ibox.ymax();
	_ymax[0]=ypos;
	for (size_type y=0;y!=ycells;++y)
	{
		// Get minimum bounding box for cell, if it is occupied.
		component* c=_components[y];
		box mcbbox=(c)?c->min_bbox():box();

		// Set maximum y-coordinate for next cell.
		int ysize=mcbbox.ysize();
		ypos-=ysize+ydiv(ysize);
		ypos-=_ygap;
		_ymax[y+1]=ypos;
	}

	// Place children.
	for (size_type y=0;y!=ycells;++y)
	{
		if (component* c=_components[y])
		{
			// Create y-baseline set for just this cell.
			ybaseline_set ybs;
			ybs.add(c->min_bbox(),c->ybaseline());

			// Construct bounding box for cell with respect to origin
			// of layout.
			box cbbox(ibox.xmin(),_ymax[y+1]+_ygap,ibox.xmax(),_ymax[y]);

			// Calculate offset from top left-hand corner of cell to
			// origin of child.
			int xoffset=_xbs.offset(xbaseline_left,c->xbaseline(),
				cbbox.xsize());
			int yoffset=ybs.offset(ybaseline_bottom,c->ybaseline(),
				cbbox.ysize());
			point coffset(xoffset,yoffset);

			// Calculate origin of cell with respect to origin of layout.
			point cpos(cbbox.xminymin()+coffset);

			// Reformat child.
			c->reformat(cpos,cbbox-cpos);
		}
	}
}

void column_layout::unformat()
{
	for (std::vector<component*>::iterator i=_components.begin();
		i!=_components.end();++i)
	{
		if (component* c=*i) c->unformat();
	}
}

void column_layout::redraw(gcontext& context,const box& clip)
{
	// Look for the first row with a lower edge which overlaps (or is
	// below) the clip box: _ymax[y0+1] + _ygap < clip.ymax().
	std::vector<int>::iterator yf0=upper_bound(
		_ymax.begin(),_ymax.end(),clip.ymax()-_ygap,std::greater<int>());
	size_type y0=yf0-_ymax.begin();
	if (y0) --y0;

	// Look for the first row with an upper edge which is below the
	// clip box: _ymax[y1] <= clip.ymin().
	std::vector<int>::iterator yf1=lower_bound(
		_ymax.begin(),_ymax.end(),clip.ymin(),std::greater<int>());
	size_type y1=yf1-_ymax.begin();
	if (y1>_components.size()) y1=_components.size();

	// Redraw children.
	// For safety, use an inequality in the for loop.
	for (size_type y=y0;y<y1;++y)
	{
		if (component* c=_components[y])
		{
			point cpos=c->origin();
			context+=cpos;
			c->redraw(context,clip-cpos);
			context-=cpos;
		}
	}
	inherited::redraw(context,clip);
}

void column_layout::remove_notify(component& c)
{
	std::vector<component*>::iterator f=
		std::find(_components.begin(),_components.end(),&c);
	if (f!=_components.end())
	{
		*f=0;
		invalidate();
	}
}

column_layout& column_layout::cells(size_type ycells)
{
	for (size_type i=min((size_t)ycells,_components.size());i!=_components.size();++i)
		if (component* c=_components[i]) c->remove();
	_components.resize(ycells,0);
	_ymax.resize(ycells+1,0);
	invalidate();
	return *this;
}

column_layout& column_layout::add(component& c,size_type y)
{
	if (y==npos) y=_components.size();
	if (y>=_components.size()) cells(y+1);
	if (_components[y]) _components[y]->remove();
	_components[y]=&c;
	link_child(c);
	invalidate();
	return *this;
}

column_layout& column_layout::ygap(int ygap)
{
	_ygap=ygap;
	invalidate();
	return *this;
}

column_layout& column_layout::margin(const box& margin)
{
	_margin=margin;
	invalidate();
	return *this;
}

column_layout& column_layout::margin(int margin)
{
	_margin=box(-margin,-margin,margin,margin);
	invalidate();
	return *this;
}

} /* namespace desktop */
} /* namespace rtk */
