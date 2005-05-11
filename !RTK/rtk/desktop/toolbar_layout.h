// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2004-2005 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_DESKTOP_TOOLBAR_LAYOUT
#define _RTK_DESKTOP_TOOLBAR_LAYOUT

#include <vector>
#include <string>

#include "rtk/desktop/sizeable_component.h"

namespace rtk {
namespace desktop {

/** A layout class for positioning toolbars around the content of a window.
 * Two types of child are permitted: one to act as the work area, and
 * zero or more to act as toolbars.  The toolbars are indexed from the
 * outside in.
 */
class toolbar_layout:
	public sizeable_component
{
private:
	/** The class from which this one is derived. */
	typedef sizeable_component inherited;
public:
	/** A type for representing toolbar counts or indicies. */
	typedef unsigned int size_type;

	/** A null value for use in place of a toolbar index. */
	static const size_type npos=static_cast<size_type>(-1);

	/** An enumeration for representing the alignment of a toolbar. */
	enum align_type
	{
		/** A toolbar attached to the left side of the window. */
		align_left,
		/** A toolbar attached to the right side of the window. */
		align_right,
		/** A toolbar attached to the bottom of the window. */
		align_bottom,
		/** A toolbar attached to the top of the window. */
		align_top
	};
private:
	/** A structure for recording information about a toolbar. */
	struct toolbar_type
	{
	public:
		/** The window which acts as the toolbar. */
		basic_window* w;
		/** The alignment of the toolbar. */
		align_type align;
		/** Construct toolbar structure. */
		toolbar_type();
	};

	/** The child which acts as the work area. */
	component* _content;

	/** A vector of toolbars indexed from the outside in. */
	std::vector<toolbar_type> _toolbars;

	/** The current bounding box. */
	box _bbox;

	/** The minimum bounding box. */
	box _min_bbox;
public:
	/** Construct toolbar layout. */
	toolbar_layout();

	/** Destroy toolbar layout. */
	virtual ~toolbar_layout();

	virtual box auto_bbox() const;
	virtual component* find(const point& p) const;
	virtual box bbox() const;
	virtual void reformat(const point& origin,const box &pbbox);
	virtual void unformat();
	virtual void redraw(gcontext& context,const box& clip);
protected:
	virtual void remove_notify(component& c);
public:
	/** Add work area to layout.
	 * If the layout already has a work-area child then it is removed.
	 * @param c the child to be added
	 * @return a reference to this
	 */
	toolbar_layout& add(component& c);

	/** Add toolbar window to layout.
	 * If the layout already has a toolbar at the given index then that
	 * toolbar is removed.
	 * @param w the window to be added as a toolbar
	 * @param align the required alignment of the toolbar
	 * @param index the required index of the toolbar
	 */
	toolbar_layout& add_toolbar(basic_window& w,align_type align=align_top,
		size_type index=npos);
};

}; /* namespace desktop */
}; /* namespace rtk */

#endif
