// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#include <algorithm>
#include <functional>

#include "rtk/util/divider.h"
#include "rtk/graphics/gcontext.h"
#include "rtk/desktop/grid_layout.h"

namespace rtk {
namespace desktop {

using rtk::util::divider;

grid_layout::grid_layout(size_type xcells,size_type ycells):
	_xcells(xcells),
	_ycells(ycells),
	_components(xcells*ycells,0),
	_xmin(xcells+1,0),
	_ymax(ycells+1,0),
	_xbs(xcells),
	_ybs(ycells),
	_xgap(0),
	_ygap(0)
{}

grid_layout::~grid_layout()
{
	for (vector<component*>::iterator i=_components.begin();
		i!=_components.end();++i)
	{
		if (component* c=*i) c->remove();
	}
}

box grid_layout::auto_bbox() const
{
	// Reset baseline sets.
	for (size_type x=0;x!=_xcells;++x) _xbs[x]=xbaseline_set();
	for (size_type y=0;y!=_ycells;++y) _ybs[y]=ybaseline_set();

	// Request min_bbox for each cell.  Incorporate into appropriate
	// x-baseline and y-baseline set.
	vector<component*>::const_iterator i=_components.begin();
	for (size_type y=0;y!=_ycells;++y)
	{
		for (size_type x=0;x!=_xcells;++x)
		{
			if (component* c=*i++)
			{
				box mcbbox=c->min_bbox();
				_xbs[x].add(mcbbox,c->xbaseline());
				_ybs[y].add(mcbbox,c->ybaseline());
			}
		}
	}

	// Calculate total width and height.
	int xsize=0;
	int ysize=0;
	for (size_type x=0;x!=_xcells;++x)
		xsize+=_xbs[x].xsize();
	for (size_type y=0;y!=_ycells;++y)
		ysize+=_ybs[y].ysize();

	// Add gaps to width and height.
	if (_xcells) xsize+=(_xcells-1)*_xgap;
	if (_ycells) ysize+=(_ycells-1)*_ygap;

	// Add margin to width and height
	xsize+=_margin.xsize();
	ysize+=_margin.ysize();

	// Construct minimum bounding box, with respect to top left-hand
	// corner of layout.
	box abbox(0,-ysize,xsize,0);

	// Translate to external origin and return.
	abbox-=external_origin(abbox,xbaseline_left,ybaseline_top);
	return abbox;
}

component* grid_layout::find(const point& p) const
{
	vector<int>::const_iterator xf=
		lower_bound(_xmin.begin(),_xmin.end(),p.x()+1,less<int>());
	vector<int>::const_iterator yf=
		lower_bound(_ymax.begin(),_ymax.end(),p.y()-1,greater<int>());
	if (xf==_xmin.begin()) return 0;
	if (yf==_ymax.begin()) return 0;
	size_type x=(xf-_xmin.begin())-1;
	size_type y=(yf-_ymax.begin())-1;
	component* c=_components[y*_xcells+x];
	if (!c) return 0;
	box cbbox=c->bbox()+c->origin();
	if (!(p<=cbbox)) return 0;
	return c;
}

box grid_layout::bbox() const
{
	return _bbox;
}

void grid_layout::resize() const
{
	for (vector<component*>::const_iterator i=_components.begin();
		i!=_components.end();++i)
	{
		if (component* c=*i) c->resize();
	}
	inherited::resize();
}

void grid_layout::reformat(const point& origin,const box &bbox)
{
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

	// Get bounding box before adding excess.
	box abbox=auto_bbox();

	// Calculate excess in x-direction.
	int xexcess=_bbox.xsize()-abbox.xsize();
	if (xexcess<0) xexcess=0;
	int xspread=abbox.xsize();
	xspread-=_margin.xsize();
	if (_xcells) xspread-=(_xcells-1)*_xgap;
	if (xspread<0) xspread=0;
	divider xdiv(xexcess,xspread);

	// Set minimum x-coordinate for each column.
	int xpos=ibox.xmin();
	_xmin[0]=xpos;
	for (size_type x=0;x!=_xcells;++x)
	{
		int xsize=_xbs[x].xsize();
		xpos+=xsize+xdiv(xsize);
		xpos+=_xgap;
		_xmin[x+1]=xpos;
	}

	// Calculate excess in y-direction.
	int yexcess=_bbox.ysize()-abbox.ysize();
	if (yexcess<0) yexcess=0;
	int yspread=abbox.ysize();
	yspread-=_margin.ysize();
	if (_ycells) yspread-=(_ycells-1)*_ygap;
	if (yspread<0) yspread=0;
	divider ydiv(yexcess,yspread);

	// Set minimum and maximum y-coordinates for each row.
	int ypos=ibox.ymax();
	_ymax[0]=ypos;
	for (size_type y=0;y!=_ycells;++y)
	{
		int ysize=_ybs[y].ysize();
		ypos-=ysize+ydiv(ysize);
		ypos-=_ygap;
		_ymax[y+1]=ypos;
	}

	// Place children.
	vector<component*>::iterator i=_components.begin();
	for (size_type y=0;y!=_ycells;++y)
	{
		for (size_type x=0;x!=_xcells;++x)
		{
			if (component* c=*i++)
			{
				// Construct bounding box for cell with respect to
				// origin of layout.
				box cbbox(_xmin[x],_ymax[y+1]+_ygap,_xmin[x+1]-_xgap,_ymax[y]);

				// Calculate offset from top left-hand corner of cell
				// to origin of child.
				int xoffset=_xbs[x].offset(xbaseline_left,c->xbaseline(),
					cbbox.xsize());
				int yoffset=_ybs[y].offset(ybaseline_bottom,c->ybaseline(),
					cbbox.ysize());
				point coffset(xoffset,yoffset);

				// Calculate origin of cell with respect to origin of
				// layout.
				point cpos(cbbox.xminymin()+coffset);

				// Reformat child.
				c->reformat(cpos,cbbox-cpos);
			}
		}
	}
}

void grid_layout::unformat()
{
	for (vector<component*>::iterator i=_components.begin();
		i!=_components.end();++i)
	{
		if (component* c=*i) c->unformat();
	}
}

void grid_layout::redraw(gcontext& context,const box& clip)
{
	// Look for the first column with a right edge which overlaps (or is to
	// the right of) the clip box: _xmin[x0+1] -_xgap > clip.xmin().
	vector<int>::iterator xf0=upper_bound(
		_xmin.begin(),_xmin.end(),clip.xmin()+_xgap,less<int>());
	size_type x0=xf0-_xmin.begin();
	if (x0) --x0;

	// Look for the first column with a left edge which is to the right of
	// the clip box: _xmin[x1] >= clip.xmax().
	vector<int>::iterator xf1=lower_bound(
		_xmin.begin(),_xmin.end(),clip.xmax(),less<int>());
	size_type x1=xf1-_xmin.begin();
	if (x1>_xcells) x1=_xcells;

	// Look for the first row with a lower edge which overlaps (or is
	// below) the clip box: _ymax[y0+1] + _ygap < clip.ymax().
	vector<int>::iterator yf0=upper_bound(
		_ymax.begin(),_ymax.end(),clip.ymax()-_ygap,greater<int>());
	size_type y0=yf0-_ymax.begin();
	if (y0) --y0;

	// Look for the first row with an upper edge which is below the
	// clip box: _ymax[y1] <= clip.ymin().
	vector<int>::iterator yf1=lower_bound(
		_ymax.begin(),_ymax.end(),clip.ymin(),greater<int>());
	size_type y1=yf1-_ymax.begin();
	if (y1>_ycells) y1=_ycells;

	// Redraw children.
	// For safety, use inequalities in the for-loops.
	vector<component*>::iterator i=_components.begin()+y0*_xcells;
	for (size_type y=y0;y<y1;++y)
	{
		vector<component*>::iterator j=i+x0;
		for (size_type x=x0;x<x1;++x)
		{
			if (component* c=*j++)
			{
				point cpos(c->origin());
				context+=cpos;
				c->redraw(context,clip+cpos);
				context-=cpos;
			}
		}
		i+=_xcells;
	}
	inherited::redraw(context,clip);
}

void grid_layout::remove_notify(component& c)
{
	vector<component*>::iterator f=
		std::find(_components.begin(),_components.end(),&c);
	if (f!=_components.end())
	{
		*f=0;
		invalidate();
	}
}

grid_layout& grid_layout::cells(size_type xcells,size_type ycells)
{
	// Create new child array.
	vector<component*> components(xcells*ycells);

	// Initialise source and destination iterators.
	vector<component*>::iterator src=_components.begin();
	vector<component*>::iterator dst=components.begin();

	// Calculate number of columns and number of rows to be copied
	// from source to destination.
	size_type xcopy=min(_xcells,xcells);
	size_type ycopy=min(_ycells,ycells);

	// Handle rows that exist in source and destination.
	for (size_type y=0;y!=ycopy;++y)
	{
		// Handle columns that exist in source and destination (copy).
		for (size_type x=0;x!=xcopy;++x)
			*dst++=*src++;
		// Handle columns that do not exist in destination (discard).
		for (size_type x=xcopy;x!=_xcells;++x)
			if (component* c=*src++) c->remove();
		// Handle columns that do not exist in source (zero).
		for (size_type x=xcopy;x!=xcells;++x)
			*dst++=0;
	}
	// Handle rows that do not exist in destination (discard).
	for (size_type y=ycopy;y!=_ycells;++y)
	{
		for (size_type x=0;x!=_xcells;++x)
			if (component* c=*src++) c->remove();
	}
	// Handle rows that do not exist in source (zero).
	for (size_type y=ycopy;y!=ycells;++y)
	{
		for (size_type x=0;x!=xcells;++x)
			*dst++=0;
	}

	// Replace existing child array.
	_xcells=xcells;
	_ycells=ycells;
	_components.swap(components);

	// Resize auxiliary data structures.
	_xmin.resize(xcells+1,0);
	_ymax.resize(ycells+1,0);
	_xbs.resize(xcells);
	_ybs.resize(ycells);

	// Invalidate component and return.
	invalidate();
	return *this;
}

grid_layout& grid_layout::add(component& c,size_type x,size_type y)
{
	// If cell does not exist then resize grid.
	if ((x>=_xcells)||(y>=_ycells))
		cells(max(_xcells,x+1),max(_ycells,y+1));

	// If cell is occupied then remove content.
	if (component* c=_components[y*_xcells+x])
		c->remove();

	// Link from parent to child and child to parent. 
	_components[y*_xcells+x]=&c;
	link_child(c);

	// Invalidate component and return.
	invalidate();
	return *this;
}

grid_layout& grid_layout::xgap(int xgap)
{
	_xgap=xgap;
	invalidate();
	return *this;
}

grid_layout& grid_layout::ygap(int ygap)
{
	_ygap=ygap;
	invalidate();
	return *this;
}

grid_layout& grid_layout::margin(const box& margin)
{
	_margin=margin;
	invalidate();
	return *this;
}

grid_layout& grid_layout::margin(int margin)
{
	_margin=box(-margin,-margin,margin,margin);
	invalidate();
	return *this;
}

}; /* namespace desktop */
}; /* namespace rtk */
