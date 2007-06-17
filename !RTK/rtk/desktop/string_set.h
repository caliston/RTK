// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2007 Alex Waugh.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_DESKTOP_STRING_SET
#define _RTK_DESKTOP_STRING_SET

#include "rtk/desktop/sizeable_component.h"
#include "rtk/desktop/icon.h"
#include "rtk/desktop/menu.h"
#include "rtk/desktop/menu_item.h"
#include "rtk/desktop/writable_field.h"
#include "rtk/events/mouse_click.h"
#include "rtk/events/menu_selection.h"

namespace rtk {
namespace desktop {

/** A class to allow a selection from a set of strings.
 * This consists of an optional text label, the value display
 * (which may optionally be writable), a popup menu icon,
 * and optionally a units label.
 */
class string_set:
	public sizeable_component,
	public events::mouse_click::handler,
	public events::menu_selection::handler
{
private:
	/** The class from which this one is derived. */
	typedef sizeable_component inherited;

	/** The bounding box. */
	box _bbox;

	/** The icon used as the text label. */
	icon _label;

	/** The icon used as the menu button. */
	icon _menuicon;

	/** The icon used as the units label. */
	icon _units;

	/** The value display field. */
	writable_field _value;

	/** The menu which holds the string options. */
	menu _menu;

public:
	/** Construct string set. */
	string_set();

	/** Destroy string set. */
	virtual ~string_set();

	virtual box auto_bbox() const;
	virtual box bbox() const;
	virtual void resize() const;
	virtual void reformat(const point& origin,const box& pbbox);
	virtual void unformat();
	virtual void redraw(gcontext& context,const box& clip);
	void handle_event(rtk::events::mouse_click& ev);
	void handle_event(rtk::events::menu_selection& ev);

	/** Get the currently selected value.
	 * @return the current value
	 */
	std::string value() const
		{ return _value.text(); }

	/** Get label icon text.
	 * @return the label icon text
	 */
	std::string label() const
		{ return _label.text(); }

	/** Get units icon text.
	 * @return the units icon text
	 */
	std::string units() const
		{ return _units.text(); }

	/** Get writable flag.
	 * @return true if the field content is directly editable by the user,
	 *  otherwise false
	 */
	bool writable() const
		{ return _value.button()==14; }

	/** Set the current value.
	 * @param value the value to set
	 * @return a reference to this
	 */
	string_set& value(std::string value)
		{ _value.text(value); return *this; }

	/** Set the menu title.
	 * @param title the title to set
	 * @return a reference to this
	 */
	string_set& menutitle(std::string title)
		{ _menu.title(title); return *this; }

	/** Set label icon text.
	 * @param text the required label text
	 * @param capacity the required capacity (defaults to length of
	 *  specified icon text)
	 * @return a reference to this
	 */
	string_set& label(const std::string& text,icon::size_type capacity=0)
		{ _label.text(text,capacity); return *this; }

	/** Add an item to the menu.
	 * @param text the text to add
	 * @return a reference to this
	 */
	string_set& add(const std::string& text);

	/** Set units icon text.
	 * @param text the required units text
	 * @param capacity the required capacity (defaults to length of
	 *  specified icon text)
	 * @return a reference to this
	 */
	string_set& units(const string& text,icon::size_type capacity=0)
		{ _units.text(text,capacity); return *this; }

	/** Set writable flag.
	 * @param writable true if the field is required to be directly
	 *  writable by the user, otherwise false
	 * @return a reference to this
	 */
	string_set& writable(bool writable);

};

} /* namespace desktop */
} /* namespace rtk */


#endif
