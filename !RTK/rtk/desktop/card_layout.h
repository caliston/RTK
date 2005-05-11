// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003-2005 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_DESKTOP_CARD_LAYOUT
#define _RTK_DESKTOP_CARD_LAYOUT

#include <map>
#include <string>

#include "rtk/desktop/sizeable_component.h"

namespace rtk {
namespace desktop {

/** A layout class for arranging components in a stack.
 * At any given time, one component is visible and the remainder are hidden.
 */
class card_layout:
	public sizeable_component
{
private:
	/** The class from which this one is derived. */
	typedef sizeable_component inherited;

	/** A map of child components indexed by tag. */
	std::map<string,component*> _components;

	/** The cached x-baseline set for the layout. */
	mutable xbaseline_set _xbs;

	/** The cached y-baseline set for the layout. */
	mutable ybaseline_set _ybs;

	/** The currently selected tag. */
	string _tag;

	/** The currently selected child, or 0 if none. */
	component* _component;

	/** The current bounding box. */
	box _bbox;
public:
	/** Construct card layout. */
	card_layout();

	/** Destroy card layout. */
	virtual ~card_layout();

	virtual box auto_bbox() const;
	virtual component* find(const point& p) const;
	virtual box bbox() const;
	virtual void resize() const;
	virtual void reformat(const point& origin,const box &pbbox);
	virtual void unformat();
	virtual void redraw(gcontext& context,const box& clip);
protected:
	virtual void remove_notify(component& c);
public:
	/** Add card.
	 * If the requested tag is already in use then the child corresponding
	 * to that tag is removed.
	 * @param c the component that will provide the card content
	 * @param tag the tag by which the card may be selected
	 * @return a reference to this
	 */
	card_layout& add(component& c,const string& tag);

	/** Select card.
	 * @param tag the tag of the card to be selected
	 * @return a reference to this
	 */
	card_layout& select(const string& tag);
};

}; /* namespace desktop */
}; /* namespace rtk */

#endif
