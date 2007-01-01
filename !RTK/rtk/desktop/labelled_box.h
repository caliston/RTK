// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2007 Alex Waugh.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_DESKTOP_LABELLED_BOX
#define _RTK_DESKTOP_LABELLED_BOX

#include "rtk/desktop/sizeable_component.h"
#include "rtk/desktop/icon.h"

namespace rtk {
namespace desktop {

/** A class to represent a labelled group box.
 * The box can contain one child component, which will typically
 * be a layout class.
 */
class labelled_box:
	public sizeable_component
{
private:
	/** The class from which this one is derived. */
	typedef sizeable_component inherited;

	/** The bounding box. */
	box _bbox;

	/** The icon used as the border. */
	icon _border;

	/** The icon used as the label. */
	icon _label;

	/** The child component. */
	component *_content;

public:
	/** Construct labelled box. */
	labelled_box();

	/** Destroy labelled box. */
	virtual ~labelled_box();

	virtual box auto_bbox() const;
	virtual box bbox() const;
	virtual void resize() const;
	virtual void reformat(const point& origin,const box& pbbox);
	virtual void unformat();
	virtual void redraw(gcontext& context,const box& clip);

	/** Add a child component to be displayed inside the box.
	 * @param c the child component to add
	 * @return a reference to this
	 */
	labelled_box& add(component& c);

	/** Get label icon text.
	 * @return the label icon text
	 */
	string text() const;

	/** Set label icon text.
	 * @param text the required label text
	 * @param capacity the required capacity (defaults to length of
	 *  specified icon text)
	 * @return a reference to this
	 */
	labelled_box& text(const string& text,icon::size_type capacity=0);

};

} /* namespace desktop */
} /* namespace rtk */

#endif
