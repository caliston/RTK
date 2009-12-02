// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003-2005 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#include <algorithm>
#include <functional>

#include "rtk/util/divider.h"
#include "rtk/graphics/gcontext.h"
#include "rtk/desktop/basic_row_layout.h"

namespace rtk {
namespace desktop {

using std::min;
using std::max;
using rtk::util::divider;

basic_row_layout::basic_row_layout(size_type xcells):
	_components(xcells,0),
	_xmin(xcells+1,0),
	_xgap(0)
{}

basic_row_layout::~basic_row_layout()
{
	for (std::vector<component*>::iterator i=_components.begin();
		i!=_components.end();++i)
	{
		if (component* c=*i) c->remove();
	}
	remove();
}

component* basic_row_layout::find(const point& p) const
{
	std::vector<int>::const_iterator xf=
		lower_bound(_xmin.begin(),_xmin.end(),p.x()+1,std::less<int>());
	if (xf==_xmin.begin()) return 0;
	size_type x=(xf-_xmin.begin())-1;
	component* c=_components[x];
	if (!c) return 0;
	box cbbox=c->bbox()+c->origin();
	if (!(p<=cbbox)) return 0;
	return c;
}

box basic_row_layout::bbox() const
{
	return _bbox;
}

void basic_row_layout::resize() const
{
	for (std::vector<component*>::const_iterator i=_components.begin();
		i!=_components.end();++i)
	{
		if (component* c=*i) c->resize();
	}
	inherited::resize();
}

void basic_row_layout::unformat()
{
	for (std::vector<component*>::iterator i=_components.begin();
		i!=_components.end();++i)
	{
		if (component* c=*i) c->unformat();
	}
}

void basic_row_layout::redraw(gcontext& context,const box& clip)
{
	// Look for the first column with a right edge which overlaps (or is to
	// the right of) the clip box: _xmin[x0+1] -_xgap > clip.xmin().
	std::vector<int>::iterator xf0=upper_bound(
		_xmin.begin(),_xmin.end(),clip.xmin()+_xgap,std::less<int>());
	size_type x0=xf0-_xmin.begin();
	if (x0) --x0;

	// Look for the first column with a left edge which is to the right of
	// the clip box: _xmin[x1] >= clip.xmax().
	std::vector<int>::iterator xf1=lower_bound(
		_xmin.begin(),_xmin.end(),clip.xmax(),std::less<int>());
	size_type x1=xf1-_xmin.begin();
	if (x1>_components.size()) x1=_components.size();

	// Redraw children.
	// For safety, use an inequality in the for-loop.
	for (size_type x=x0;x<x1;++x)
	{
		if (component* c=_components[x])
		{
			point cpos=c->origin();
			context+=cpos;
			c->redraw(context,clip-cpos);
			context-=cpos;
		}
	}
	inherited::redraw(context,clip);
}

void basic_row_layout::remove_notify(component& c)
{
	std::vector<component*>::iterator f=
		std::find(_components.begin(),_components.end(),&c);
	if (f!=_components.end())
	{
		*f=0;
		invalidate();
	}
}

basic_row_layout& basic_row_layout::cells(size_type xcells)
{
	for (size_type i=min((size_t)xcells,_components.size());i!=_components.size();++i)
		if (component* c=_components[i]) c->remove();
	_components.resize(xcells,0);
	_xmin.resize(xcells+1,0);
	invalidate();
	return *this;
}

basic_row_layout& basic_row_layout::add(component& c,size_type x)
{
	if (x==npos) x=_components.size();
	if (x>=_components.size()) cells(x+1);
	if (_components[x]) _components[x]->remove();
	_components[x]=&c;
	link_child(c);
	invalidate();
	return *this;
}

basic_row_layout& basic_row_layout::xgap(int xgap)
{
	_xgap=xgap;
	invalidate();
	return *this;
}

basic_row_layout& basic_row_layout::margin(const box& margin)
{
	_margin=margin;
	invalidate();
	return *this;
}

basic_row_layout& basic_row_layout::margin(int margin)
{
	_margin=box(-margin,-margin,margin,margin);
	invalidate();
	return *this;
}

} /* namespace desktop */
} /* namespace rtk */
