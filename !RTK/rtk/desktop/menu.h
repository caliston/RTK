// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003-2004 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_DESKTOP_MENU
#define _RTK_DESKTOP_MENU

#include <vector>
#include <string>

#include "rtk/util/refcount.h"
#include "rtk/desktop/component.h"
#include "rtk/events/redirection.h"

namespace rtk {
namespace os {

class menu;

}; /* namespace os */

namespace desktop {

class menu_item;

/** A component to represent a RISC OS menu.
 */
class menu:
	public component,
	public events::redirection
{
private:
	/** The class from which this one is derived. */
	typedef component inherited;
public:
	/** A type for representing indices. */
	typedef unsigned int size_type;

	/** A null value for use in place of an index. */
	static const size_type npos=static_cast<size_type>(-1);

	class menu_data;
private:
	/** A vector containing child pointers. */
	vector<menu_item*> _items;

	/** The menu data used by the Wimp.
	 * This may be null, in which case no menu data has been created.
	 */
	menu_data* _mdata;

	/** The opened flag.
	 * True if this is a top-level menu currently visible on the screen,
	 * otherwise false.  (Correct maintenance of this flag depends on
	 * receipt of MenusDeleted messages.)
	 */
	bool _opened:1;

	/** The auto-reopen flag.
	 * True if this menu should be re-opened automatically if the user
	 * makes a menu selection using the adjust button.  The top-level
	 * menu determines the behaviour for the whole menu tree.
	 */
	bool _auto_reopen:1;

	/** The title foreground colour.
	 * This is one of the 16 standard Wimp colours.
	 */
	unsigned int _tf_colour:4;

	/** The title background colour.
	 * This is one of the 16 standard Wimp colours.
	 */
	unsigned int _tb_colour:4;

	/** The work area foreground colour.
	 * This is one of the 16 standard Wimp colours.
	 */
	unsigned int _wf_colour:4;

	/** The work area background colour.
	 * This is one of the 16 standard Wimp colours.
	 */
	unsigned int _wb_colour:4;

	/** The height of each menu item. */
	int _height;

	/** The gap between menu items. */
	int _gap;

	/** The title buffer.
	 * This may be null, in which case no buffer has been allocated.
	 */
	char* _title;

	/** The title buffer size.
	 * This is the maximum number of characters that the buffer can
	 * hold, not counting the terminator.
	 */
	size_type _titlesize;

	/** The cached minimum bounding box.
	 */
	mutable box _min_bbox;
public:
	/** Construct menu.
	 * By default a menu:
	 * <UL>
	 * <LI>has a title foreground colour of 7;</LI>
	 * <LI>has a title background colour of 2;</LI>
	 * <LI>has a work area foreground colour of 7;</LI>
	 * <LI>has a work area background colour of 0;</LI>
	 * <LI>has a height for each menu item of 44 OS units;</LI>
	 * <LI>has a gap between menu items of 0 OS units; and</LI>
	 * <LI>has no title.</LI>
	 * </UL>
	 */
	menu();

	/** Destroy menu. */
	virtual ~menu();

	virtual box min_bbox() const;
	virtual void resize() const;
	virtual void reformat(const point& origin,const box& pbbox);
	void reformat(const point& origin,const box& pbbox,size_type level);
	virtual void unformat();
protected:
	virtual void remove_notify(component& c);
public:
	/** Get number of cells.
	 * @return the number of cells
	 */
	size_type cells() const
		{ return _items.size(); }

	/** Set number of cells.
	 * @param ycells the required number of cells
	 * @return a reference to this
	 */
	menu& cells(size_type ycells);

	/** Add menu item to menu.
	 * If the specified cell is occupied then the occupant is replaced.
	 * @param item the menu item to add
	 * @param y the cell index (defaults to npos, meaning new cell)
	 */
	menu& add(menu_item& item,size_type y=npos);

	/** Get menu title.
	 * @return the menu title
	 */
	string title() const;

	/** Get menu item height.
	 * @return the menu item height
	 */
	int height() const
		{ return _height; }

	/** Get gap between menu items.
	 * @return the gap between menu items
	 */
	int gap() const
		{ return _gap; }

	/** Get auto-reopen flag.
	 * The top-level menu determines the behaviour for the whole
	 * menu tree.
	 * @return true if this menu re-opens automatically when a menu
	 *  selection is made using adjust, otherwise false.
	 */
	bool auto_reopen() const
		{ return _auto_reopen; }

	/** Get title foreground colour.
	 * This is one of the 16 standard Wimp colours.
	 * @return the title foreground colour
	 */
	int tf_colour() const
		{ return _tf_colour; }

	/** Get title background colour.
	 * This is one of the 16 standard Wimp colours.
	 * @return the title background colour
	 */
	int tb_colour() const
		{ return _tb_colour; }

	/** Get work area foreground colour.
	 * This is one of the 16 standard Wimp colours.
	 * @return the work area foreground colour
	 */
	int wf_colour() const
		{ return _wf_colour; }

	/** Get work area background colour.
	 * This is one of the 16 standard Wimp colours.
	 * @return the work area background colour
	 */
	int wb_colour() const
		{ return _wb_colour; }

	/** Set menu title.
	 * If the title is likely to change then its capacity should be set
	 * to the maximum number of characters likely to be needed.
	 * @param title the required menu title
	 * @param capacity the required capacity (defaults to length of
	 *  specified menu title)
	 * @return a reference to this
	 */
	menu& title(const string& title,size_type capacity=0);

	/** Set menu item height.
	 * @param value the required menu item height
	 * @return a reference to this
	 */
	menu& height(int value);

	/** Set gap between menu items.
	 * @param value the required gap between menu items
	 * @return a reference to this
	 */
	menu& gap(int value);

	/** Set auto-reopen flag.
	 * The top-level menu determines the behaviour for the whole
	 * menu tree.
	 * @param value true if this menu is to re-open automatically
	 *  when a menu selection is made using adjust, otherwise false.
	 */
	menu& auto_reopen(bool value);

	/** Set title foreground colour.
	 * This is one of the 16 standard Wimp colours.
	 * @param tf_colour the required title foreground colour
	 * @return a reference to this
	 */
	menu& tf_colour(int tf_colour);

	/** Set title background colour.
	 * This is one of the 16 standard Wimp colours.
	 * @param tb_colour the required title background colour
	 * @return a reference to this
	 */
	menu& tb_colour(int tb_colour);

	/** Set work area foreground colour.
	 * This is one of the 16 standard Wimp colours.
	 * @param wf_colour the required work area foreground colour
	 * @return a reference to this
	 */
	menu& wf_colour(int wf_colour);

	/** Set work area background colour.
	 * This is one of the 16 standard Wimp colours.
	 * @param wb_colour the required work area background colour
	 * @return a reference to this
	 */
	menu& wb_colour(int wb_colour);

	/** Show menu in response to mouse click.
	 * The parent and coordinates for the menu are taken from the
	 * specified mouse click event.  A suitable offset is applied
	 * to the coordinates to comply with the style guide.
	 * @param ev the mouse click event
	 */
	void show(const events::mouse_click& ev);

	/** Re-open menu.
	 * This function may be called on receipt of a menu selection
	 * event.  It causes the current menu tree to remain open instead
	 * of being closed.  By default, menus and menu items are
	 * configured to do this automatically when the selection is
	 * made using the adjust button.
	 */
	void reopen();

	/** Create menu data.
	 * @internal
	 * Create the menu data block that is used by the Wimp.
	 * Reallocation (which usually changes the address of the data)
	 * is performed only when necessary.
	 */
	void create();

	/** Uncreate menu data.
	 * @internal
	 */
	void uncreate();

	/** Get handle (address of menu data).
	 * @return the address of the menu data, for use by the Wimp
	 */
	int handle() const;

	/** Deliver Wimp event block.
	 * @internal
	 * The event block is converted into a suitable event object and
	 * posted to the appropriate target (which should be this menu, one
	 * of its children, or a component lower in the menu heirarchy).
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
	 * into a suitable event object and posted to the appropriate target
	 * (which should be this menu, one of its children, or a component
	 * lower in the menu heirarchy).
	 * @param wimpcode the Wimp event code
	 * @param wimpblock the Wimp event block
	 * @param level the level within the menu heirarchy (0=top level,
	    add 1 for each level down)
	 */
	void deliver_message(int wimpcode,os::wimp_block& wimpblock,
		unsigned int level);
};

/** A class to represent a RISC OS menu data block.
 * This class is used to ensure that a menu data block is not deleted
 * until both the menu object and the Wimp have finished using it.
 */
class menu::menu_data:
	public rtk::util::refcount
{
private:
	/** The size of the menu data block, in words. */
	unsigned int _size;
	/** A pointer to the menu data block. */
	int* _data;
public:
	/** Construct menu data object.
	 * @param size the required size of the menu data block, in words
	 */
	menu_data(unsigned int size);

	/** Destroy menu data object.
	 */
	virtual ~menu_data();

	/** Get menu data block.
	 * @return a reference to the menu data block.
	 */
	os::menu& operator*() const
		{ return *(os::menu*)_data; }

	/** Get member of menu data block.
	 * @return a pointer to the menu data block (which the compiler will
	 *  resolve to a reference to the specified member)
	 */
	os::menu* operator->() const
		{ return (os::menu*)_data; }

	/** Get size of menu data block.
	 * @return the size of the menu data block, in words.
	 */
	unsigned int size() const
		{ return _size; }
};

}; /* namespace desktop */
}; /* namespace rtk */

#endif
