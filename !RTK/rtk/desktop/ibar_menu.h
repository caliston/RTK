// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2004 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_DESKTOP_IBAR_MENU
#define _RTK_DESKTOP_IBAR_MENU

#include "rtk/desktop/menu.h"
#include "rtk/desktop/menu_item.h"

#include "rtk/events/menu_selection.h"

namespace rtk {
namespace desktop {

class window;

class ibar_menu:
	public rtk::desktop::menu,
	public rtk::events::menu_selection::handler
{
private:
	/** The class from which this one is derived. */
	typedef menu inherited;

	/** The attached program information dialogue box, or 0 if there is
	 * none */
	window* _prog_info_dbox;

	/** The attached choices window, or 0 if there is none. */
	window* _choices_window;

	/** The menu item 'Info'. */
	menu_item _menu_info;

	/** The menu item 'Choices'. */
	menu_item _menu_choices;

	/** The menu item 'Quit'. */
	menu_item _menu_quit;
public:
	/** Construct menu for use on the icon bar.
	 * By default there are no attached windows or dialogue boxes,
	 * and the only menu item is "Quit".
	 * @param name the name of the application
	 */
	ibar_menu(const string& name);

	virtual void handle_event(rtk::events::menu_selection& ev);

	/** Get program information dialogue box attached to this icon.
	 * @return the attached dialogue box, or 0 if there is none
	 */
	window* prog_info_dbox() const
		{ return _prog_info_dbox; }

	/** Get choices window attached to this icon.
	 * @return the attached choices window, or 0 if there is none
	 */
	window* choices_window() const
		{ return _choices_window; }

	/** Attach program information dialogue box.
	 * This dialogue box is attached to the "Info" menu item, which is
	 * created if it does not already exist.
	 * @param dbox the required program information dialogue box
	 */
	ibar_menu& attach_prog_info_dbox(window& dbox);

	/** Attach choices window.
	 * This window is opened when the user selects the "Choices ..."
	 * menu item, which is created if it does not already exist.
	 * @param w the required choices window
	 */
	ibar_menu& attach_choices_window(window& w);

	/** Detach program information dialogue box.
	 * The corresponding menu item is removed.  If there is no attached
	 * main window then no action is taken.
	 */
	ibar_menu& detach_prog_info_dbox();

	/** Detach choices window.
	 * The corresponding menu item is removed.  If there is no attached
	 * main window then no action is taken.
	 */
	ibar_menu& detach_choices_window();
private:
	/** Rebuild menu. */
	void rebuild();
};

}; /* namespace desktop */
}; /* namespace rtk */

#endif
