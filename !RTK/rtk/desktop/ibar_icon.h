// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2004 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_DESKTOP_IBAR_ICON
#define _RTK_DESKTOP_IBAR_ICON

#include "rtk/desktop/icon.h"

#include "rtk/events/mouse_click.h"

namespace rtk {
namespace desktop {

class menu;
class basic_window;

class ibar_icon:
	public rtk::desktop::icon,
	public rtk::events::mouse_click::handler
{
private:
	/** The class from which this one is derived. */
	typedef icon inherited;

	/** The attached window (select button), or 0 if there is none. */
	basic_window* _select_window;

	/** The attached window (adjust button), or 0 if there is none. */
	basic_window* _adjust_window;

	/** The attached menu, or 0 if there is none. */
	rtk::desktop::menu* _menu;
public:
	/** Construct icon for use on the icon bar.
	 * By default there are no attached windows or menus.
	 */
	ibar_icon();

	/** Construct icon for use on the icon bar, with given menu. */
	ibar_icon(rtk::desktop::menu& m);

	virtual void handle_event(rtk::events::mouse_click& ev);

	/** Get window attached to this icon (select button).
	 * @return the attached window, or 0 if there is none
	 */
	basic_window* select_window() const
		{ return _select_window; }

	/** Get window attached to this icon (adjust button).
	 * @return the attached window, or 0 if there is none
	 */
	basic_window* adjust_window() const
		{ return _adjust_window; }

	/** Get menu attached to this icon.
	 * @return the attached menu, or 0 if there is none
	 */
	rtk::desktop::menu* menu() const
		{ return _menu; }

	/** Attach window (select button).
	 * This window is opened when the user clicks select over the icon.
	 * @param w the window to be attached
	 */
	ibar_icon& attach_select_window(basic_window& w);

	/** Attach window (adjust button).
	 * This window is opened when the user clicks adjust over the icon.
	 * @param w the window to be attached
	 */
	ibar_icon& attach_adjust_window(basic_window& w);

	/** Attach menu.
	 * This menu is opened when the user clicks menu over the icon.
	 * @param m the menu to be attached
	 */
	ibar_icon& attach_menu(rtk::desktop::menu& m);

	/** Detach window (select button).
	 * If there is no attached main window then no action is taken.
	 */
	ibar_icon& detach_select_window();

	/** Detach window (adjust button).
	 * If there is no attached main window then no action is taken.
	 */
	ibar_icon& detach_adjust_window();

	/** Detach menu.
	 * If there is no attached menu then no action is taken.
	 */
	ibar_icon& detach_menu();
};

}; /* namespace desktop */
}; /* namespace rtk */

#endif
