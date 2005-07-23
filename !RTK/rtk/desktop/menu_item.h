// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003-2005 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_DESKTOP_MENU_ITEM
#define _RTK_DESKTOP_MENU_ITEM

#include "rtk/desktop/icon.h"

namespace rtk {
namespace desktop {

class window;
class menu;

/** A class to represent a RISC OS menu item. */
class menu_item:
	public icon
{
private:
	/** The class from which this one is derived. */
	typedef icon inherited;

	/** A union used to hold an attached menu or dialogue box. */
	union
	{
		/** The menu that is currently attached to this component, if any.
		 * This variable is meaningful if _has_menu is true.
		 */
		menu* _submenu;
		/** The dialogue box that is currently attached to this component,
		 * if any.  This variable is meaningful if _has_dbox is true.
		 */
		window* _dbox;
	};

	/** The has-submenu flag.
	 * True if _submenu points to a menu that is currently attached to
	 * this menu item.  (The submenu is not a child, so can be attached to
	 * any number of menu items.)
	 */
	bool _has_submenu:1;

	/** The has-dbox flag.
	 * True if _dbox points to a dialogue box window that is currently
	 * attached to this menu item.  (The dialogue box is not a child,
	 * so can be attached to any number of menu items.)
	 */
	bool _has_dbox:1;

	/** The tick flag.
	 * True if this menu item is ticked, otherwise false.
	 */
	bool _tick:1;

	/** The separator flag.
	 * True if this menu item is followed by a separator, otherwise false.
	 */
	bool _separator:1;

	/** The writable flag.
	 * True if this menu item is writable, otherwise false.
	 */
	bool _writable:1;
public:
	/** Construct menu item.
	 * By default, a menu item:
	 * - has a background colour of 0;
	 * - is not followed by a separator; and
	 * - keeps the menu tree open if clicked with adjust.
	 *
	 * Other default attributes are as for an icon.
	 */
	menu_item();

	/** Destroy menu item. */
	virtual ~menu_item();
public:
	/** Get submenu attached to this menu item.
	 * @return the attached submenu, or 0 if there is none
	 */
	menu* submenu();

	/** Attach submenu.
	 * Any existing submenu or dialogue box is detached.
	 * Attaching a submenu does not make it a child of this menu item.
	 * It becomes a child of the application when it is opened by the
	 * user, and ceases to be a child when it is closed.  This behaviour
	 * allows one submenu to be shared by several menu items.
	 * @param submenu the submenu to be attached
	 * @return a reference to this
	 */
	menu_item& attach_submenu(menu& submenu);

	/** Detach submenu.
	 * If there is no attached submenu then no action is taken.
	 * @return a reference to this
	 */
	menu_item& detach_submenu();

	/** Get dialogue box attached to this menu item.
	 * @return the attached dialogue box, or 0 is there is none
	 */
	window* dbox();

	/** Attach dialogue box.
	 * Any existing submenu or dialogue box is detached.
	 * Attaching a dialogue box does not make it a child of this menu
	 * item.  It becomes a child of the application when it is opened
	 * by the user, and ceases to be a child when it is closed.  This
	 * behaviour allows one dialogue box to be shared between several
	 * menu items.
	 * @param dbox the dialogue box to be attached
	 * @return a reference to this
	 */
	menu_item& attach_dbox(window& dbox);

	/** Detach dialogue box.
	 * If there is no attached dialogue box then no action is taken.
	 * @return a reference to this
	 */
	menu_item& detach_dbox();

	/** Get tick flag.
	 * @return true if this menu item is ticked, otherwise false.
	 */
	bool tick() const
		{ return _tick; }

	/** Get separator flag.
	 * @return true if this menu item is followed by a separator,
	 *  otherwise false
	 */
	bool separator() const
		{ return _separator; }

	/** Get writable flag.
	 * @return true if this menu item is writable, otherwise false
	 */
	bool writable() const
		{ return _writable; }

	/** Set tick flag.
	 * @param value true if this item is to be ticked, otherwise false
	 * @return a reference to this
	 */
	menu_item& tick(bool value);

	/** Set separator flag.
	 * @param value true if this item is to be followed by a separator,
	 *  otherwise false
	 */
	menu_item& separator(bool value);

	/** Set writable flag.
	 * @param value true if this menu item is writable, otherwise false
	 */
	menu_item& writable(bool value);

	/** Get menu flags.
	 * @return menu flags for use by the Wimp
	 */
	int menu_flags() const;

	/** Get submenu or dialogue box handle.
	 * @internal
	 * If this menu item has an attached submenu then return the
	 * address of the associated menu data block, otherwise if it
	 * has an attached dialogue box then return the associated
	 * window handle, otherwise return 0.
	 * If necessary, menu::create() or window::create() will be
	 * called to ensure that the menu has a data block or window
	 * has a handle.  Where this behaviour is not desirable
	 * use dummy_submenu_handle() instead.
	 * @return the submenu or dialogue box handle, or 0 if none
	 */
	int submenu_handle() const;

	/** Get dummy submenu or dialogue box handle.
	 * @internal
	 * If this menu item has an attached submenu then return the
	 * address of the associated menu data block, otherwise if it
	 * has an attached dialogue box then return the associated
	 * window handle, otherwise return 0.
	 * If the menu does not have a data block, or window does not
	 * have a handle, a dummy value is returned which is suitable
	 * for use when populating a menu data block (provided it is
	 * only used to generate Message_MenuWarning events).
	 * @return a dummy submenu or dialogue box handle, or 0 if none
	 */
	int dummy_submenu_handle() const;

	/** Deliver Wimp event block.
	 * @internal
	 * The event block is delivered to the appropriate target, which
	 * is determined by the supplied menu tree.
	 * @param wimpcode the Wimp event code
	 * @param wimpblock the Wimp event block
	 * @param tree the menu tree (a array of menu indexes ending in -1)
	 * @param level the level within the menu heirarchy (0=top level,
	    add 1 for each level down)
	 */
	void deliver_wimp_block(int wimpcode,os::wimp_block& wimpblock,
		const int* tree,unsigned int level);

	/** Deliver Wimp event block.
	 * @internal
	 * The event block is converted into a suitable event object and
	 * posted.  This object will be the target.
	 * @param wimpcode the Wimp event code
	 * @param wimpblock the Wimp event block
	 * @param level the level within the menu heirarchy (0=top level,
	    add 1 for each level down)
	 */
	void deliver_wimp_block(int wimpcode,os::wimp_block& wimpblock,
		unsigned int level);

	/** Deliver Wimp message.
	 * @internal
	 * The event block (which should contain a message) is converted
	 * into a suitable event object and posted.  This object will be
	 * the target.
	 * @param wimpcode the Wimp event code
	 * @param wimpblock the Wimp event block
	 * @param level the level within the menu heirarchy (0=top level,
	    add 1 for each level down)
	 */
	void deliver_message(int wimpcode,os::wimp_block& wimpblock,
		unsigned int level);
};

}; /* namespace desktop */
}; /* namespace rtk */

#endif
