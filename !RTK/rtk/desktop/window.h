// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_DESKTOP_WINDOW
#define _RTK_DESKTOP_WINDOW

#include <map>
#include <string>

#include "rtk/desktop/component.h"
#include "rtk/events/close_window.h"

namespace rtk {
namespace os {

union wimp_block;

}; /* namespace os */

namespace desktop {

class icon;

/** A class to represent a RISC OS window.
 * A window object can have at most one child.  Usually this will be
 * a layout object which in turn contains further children.
 *
 * Coordinates:
 * - The bounding box of the window corresponds to the visible area.
 * - The origin and bounding box of the child correspond to the origin
 *   and extent of the work area.  Scrolling is achieved by moving
 *   the origin of the child.
 * - If the window has an adjust icon then the work area will expand
 *   automatically to cover the visible area.
 * - If the window does not have an adjust icon then the visible area
 *   will expand automatically to cover the work area.
 */

class window:
	public component,
	public events::close_window::handler
{
private:
	/** The class from which this one is derived. */
	typedef component inherited;
public:
	/** A type for representing the size of a buffer. */
	typedef unsigned int size_type;
private:
	/** The child pointer. */
	component* _child;

	/** The RISC OS window handle. */
	int _handle;

	/** A map from icon handles to icon components.
	 * All descendant icons that currently have a RISC OS icon handle
	 * are included.
	 */
	map<int,icon*> _ihandles;

	/** The current bounding box. */
	box _bbox;

	/** The current extent. */
	box _extent;

	/** The window created flag.
	 * True if a RISC OS window exists for this component, otherwise false.
	 */
	unsigned int _created:1;

	/** The opened-as-dialogue-box flag.
	 * True if this window has been opened as a dialogue box and should not
	 * be opened again when it is reformatted.
	 */
	unsigned int _dbox_opened:1;

	/** The movable flag.
	 * True if this window is movable (by the user), otherwise false.
	 */
	unsigned int _movable:1;

	/** The back icon flag.
	 * True if this window has a back icon, otherwise false.
	 */
	unsigned int _back_icon:1;

	/** The close icon flag.
	 * True if this window has a close icon, otherwise false.
	 */
	unsigned int _close_icon:1;

	/** The title bar flag.
	 * True if this window has a title bar, otherwise false.
	 */
	unsigned int _title_bar:1;

	/** The toggle-size icon flag.
	 * True if this window has a toggle-size icon, otherwise false.
	 */
	unsigned int _toggle_icon:1;

	/** The horizontal scroll bar flag.
	 * True if this window has a horizontal scroll bar, otherwise false.
	 */
	unsigned int _x_scroll_bar:1;

	/** The vertical scroll bar flag.
	 * True if this window has a vertical scroll bar, otherwise false.
	 */
	unsigned int _y_scroll_bar:1;

	/** The adjust-size icon flag.
	 * True if this window has an adjust-size icon, otherwise false.
	 */
	unsigned int _adjust_icon:1;

	/** The button type. */
	unsigned int _button:4;

	/** The window foreground colour.
	 * This is one of the 16 standard Wimp colours.
	 */
	unsigned int _wf_colour:8;

	/** The window background colour.
	 * This is one of the 16 standard Wimp colours.
	 */
	unsigned int _wb_colour:8;

	/** The current title buffer.
	 * This may be null, in which case no buffer has been allocated.
	 */
	char* _title;

	/** The current title buffer size.
	 * This is the maximum number of characters that the title buffer can
	 * hold, not counting the terminator.
	 */
	size_type _titlesize;
public:
	/** Construct window.
	 * By default a window:
	 * - is movable;
	 * - has a title bar;
	 * - has a back icon;
	 * - has a close icon;
	 * - does not have scroll bars;
	 * - does not have a toggle icon;
	 * - does not have an adjust icon;
	 * - has a button type of 10;
	 * - has a work area foreground colour of 7; and
	 * - has a work area background colour of 1.
	 * @param size the required initial size
	 */
	window(point size=point(640,512));

	/** Destroy window.
	 */
	virtual ~window();

	virtual window* as_window();
	virtual component* find(const point& p) const;
	virtual box bbox() const;
	virtual box min_bbox() const;
	virtual void resize() const;
	virtual void reformat(const point& origin,const box& bbox);
	virtual void unformat();
	virtual void redraw(gcontext& context,const box& clip);
protected:
	void remove_notify(component& c);
	void baseline_notify(const point& offset);
public:
	/** Add child to window.
	 * If the window already has a child then it is removed.
	 * @param c the child to be added
	 */
	window& add(component& c);

	/** Reformat window as dialogue box.
	 * This function is used in place of reformat(point&,box&) when
	 * the window is to be opened as a dialogue box.
	 * @param origin the new origin of this component, with respect to
	 *  its parent
	 * @param bbox the proposed bounding box for this component, with
	 *  respect to its own origin
	 * @param level the level within the menu heirarchy (0=top level,
	 *  add 1 for each level down)
	 */
	void reformat(const point& origin,const box& bbox,size_type level);

	/** Get window title.
	 * @return the window title
	 */
	string title() const;

	/** Get movable flag.
	 * @return true if window is movable (by the user), otherwise false
	 */
	bool movable() const
		{ return _movable; }

	/** Get back icon flag.
	 * @return true if window has a back icon, otherwise false
	 */
	bool back_icon() const
		{ return _back_icon; }

	/** Get close icon flag.
	 * @return true if window has a close icon, otherwise false
	 */
	bool close_icon() const
		{ return _close_icon; }

	/** Get title bar flag.
	 * @return true if window has a title bar, otherwise false
	 */
	bool title_bar() const
		{ return _title_bar; }

	/** Get toggle-size icon flag.
	 * @return true if window has a toggle-size icon, otherwise false
	 */
	bool toggle_icon() const
		{ return _toggle_icon; }

	/** Get horizontal scroll bar flag.
	 * @return true if window has a horizontal scroll bar, otherwise false
	 */
	bool x_scroll_bar() const
		{ return _x_scroll_bar; }

	/** Get vertical scroll bar flag.
	 * @return true if window has a vertical scroll bar, otherwise false
	 */
	bool y_scroll_bar() const
		{ return _y_scroll_bar; }

	/** Get adjust-size icon flag.
	 * @return true if window has an adjust-size icon, otherwise false
	 */
	bool adjust_icon() const
		{ return _adjust_icon; }

	/** Get window background colour.
	 * This is one of the 16 standard Wimp colours.
	 * @return the window background colour
	 */
	int wb_colour() const
		{ return _wb_colour; }

	/** Get window foreground colour.
	 * This is one of the 16 standard Wimp colours.
	 * @return the window foreground colour
	 */
	int wf_colour() const
		{ return _wf_colour; }

	/** Set window title.
	 * If the title is likely to change then its capacity should be set
	 * to the maximum number of characters likely to be needed.
	 * @param title the required window title
	 * @param capacity the required capacity (defaults to length of
	 *  specified window title)
	 * @return a reference to this
	 */
	window& title(const string& title,size_type capacity=0);

	/** Set movable flag.
	 * @param value true if the window is to be movable, otherwise false
	 * @return a reference to this
	 */
	window& movable(bool value);

	/** Set back icon flag.
	 * @param value true if the window is to have a back icon, otherwise
	 *  false
	 * @return a reference to this
	 */
	window& back_icon(bool value);

	/** Set close icon flag.
	 * @param value true if the window is to have a close icon, otherwise
	 *  false
	 * @return a reference to this
	 */
	window& close_icon(bool value);

	/** Set title bar flag.
	 * @param value true if the window is to have a title bar, otherwise
	 *  false
	 * @return a reference to this
	 */
	window& title_bar(bool value);

	/** Set toggle-size icon flag.
	 * @param value true if the window is to have a toggle-size icon,
	 *  otherwise false
	 * @return a reference to this
	 */
	window& toggle_icon(bool value);

	/** Set horizontal scroll bar flag.
	 * @param value true if the window is to have a horizontal scroll bar,
	 *  otherwise false
	 * @return a reference to this
	 */
	window& x_scroll_bar(bool value);

	/** Set vertical scroll bar flag.
	 * @param value true if the window is to have a vertical scroll bar,
	 *  otherwise false
	 * @return a reference to this
	 */
	window& y_scroll_bar(bool value);

	/** Set adjust-size icon flag.
	 * @param value true if the window is to have an adjust-size icon,
	 *  otherwise false
	 * @return a reference to this
	 */
	window& adjust_icon(bool value);

	/** Set window background colour.
	 * This is one of the 16 standard Wimp colours.
	 * @param colour the required window background colour
	 * @return a reference to this
	 */
	window& wb_colour(int colour);

	/** Set window foreground colour.
	 * This is one of the 16 standard Wimp colours.
	 * @param colour the required window foreground colour
	 * @return a reference to this
	 */
	window& wf_colour(int colour);

	/** Default handler for close_window events.
	 * If this handler remains registered then the window will close itself
	 * (by removing itself from the component heirarchy) when the close
	 * button is pressed.
	 * @param ev the close_window event
	 */
	void handle_event(events::close_window& ev);

	/** Deliver Wimp event block.
	 * @internal
	 * The event block is converted into a suitable event object and
	 * posted to the appropriate target.  This function should be
	 * used when it has been established that this window, or a child
	 * of this window, is the target.
	 * @param wimpcode the Wimp event code
	 * @param wimpblock the Wimp event block
	 */
	void deliver_wimp_block(int wimpcode,os::wimp_block& wimpblock);

	/** Deliver Wimp message.
	 * @internal
	 * The event block (which should contain a message) is converted
	 * into a suitable event object and posted to the appropriate target.
	 * This function should be used when it has been established that
	 * this window, or a child of this window, is the target.
	 * @param wimpcode the Wimp event code
	 * @param wimpblock the Wimp event block
	 */
	void deliver_message(int wimpcode,os::wimp_block& wimpblock);

	/** Register icon for event delivery.
	 * @internal
	 * This function is called by an icon object when it is given an
	 * icon handle by the Wimp.  The window object uses this information
	 * to deliver Wimp event blocks.
	 * @param ic the icon object to be registered.
	 */
	void register_icon(icon& ic);

	/** Deregister icon for event delivery.
	 * @internal
	 * This function is called by an icon object when it releases an
	 * icon handle.
	 * @param ic the icon object to be deregistered.
	 */
	void deregister_icon(icon& ic);

	/** Find icon.
	 * @param handle the icon handle
	 * @return the icon, or 0 if not found
	 */
	icon* find_icon(int handle) const;

	/** Instruct Wimp to create window.
	 * @internal
	 */
	void create();

	/** Instruct Wimp to delete window.
	 * @internal
	 */
	void uncreate();

	/** Get window handle.
	 * Be aware that this class assumes that it has full control over
	 * any RISC OS windows that it creates.  It may override, or fail to
	 * take account of, changes that are made directly using the window
	 * handle.
	 * @return the window handle
	 */
	int handle() const
		{ return _handle; }

	/** Get window flags.
	 * @internal
	 * @return window flags for use by the Wimp
	 */
	int window_flags() const;

	/** Get work area flags.
	 * @internal
	 * @return work area flags for use by the Wimp
	 */
	int work_area_flags() const;

	/** Get handle of window in front of this one.
	 * @internal
	 * @return the handle of the window in front of this one, or -1 if
	 *  this window is at the top of the window stack
	 */
	int behind() const;
private:
	/** Reformat window.
	 * @internal
	 * This function is equivalent to reformat(const point&,const box&),
	 * except that the required position in the window stack can (and must)
	 * be specified.
	 * @param origin the new origin of this component, with respect to
	 *  its parent
	 * @param bbox the proposed bounding box for this component, with
	 *  respect to its own origin
	 * @param behind the window behind which this one should be opened,
	 *  or -1 to open at the top of the window stack
	 */
	void _reformat(const point& origin,const box& bbox,int behind);

	/** Calculate extent.
	 * @internal
	 * @param bbox the proposed bounding box of this component
	 * @return the extent
	 */
	box calculate_extent(const box& bbox);
};

}; /* namespace desktop */
}; /* namespace rtk */

#endif
