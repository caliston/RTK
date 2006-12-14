// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003-2005 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_DESKTOP_BASIC_WINDOW
#define _RTK_DESKTOP_BASIC_WINDOW

#include <map>
#include <string>

#include "rtk/desktop/component.h"
#include "rtk/events/close_window.h"
#include "rtk/events/auto_scroll.h"

namespace rtk {
namespace os {

union wimp_block;

} /* namespace os */

namespace desktop {

class icon;

/** A class to represent a RISC OS window.
 * Windows of this class by default have no window furniture.  For a
 * standard top-level window use the class rtk::desktop::window. 
 *
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

class basic_window:
	public component,
	public events::close_window::handler,
	public events::auto_scroll::handler
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
	std::map<int,icon*> _ihandles;

	/** The current bounding box. */
	box _bbox;

	/** The current extent. */
	box _extent;

	/** The window created flag.
	 * True if a RISC OS window exists for this component, otherwise false.
	 */
	unsigned int _created:1;

	/** The registered false.
	 * True if this window is registered with an application, otherwise false.
	 */
	unsigned int _registered:1;

	/** The window opened flag.
	 * True if the RISC OS window is or might be open, otherwise false.
	 */
	unsigned int _opened:1;

	/** The dialogue box flag.
	 * True if the window is a dialogue box, otherwise false.
	 */
	unsigned int _dbox:1;

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

	/** The border flag.
	 * True if this window has a border, otherwise false.
	 */
	unsigned int _border:1;

	/** The button type. */
	unsigned int _button:4;

	/** The title bar foreground colour.
	 * This is one of the 16 standard Wimp colours.
	 */
	unsigned int _tf_colour:8;

	/** The title bar background colour.
	 * This is one of the 16 standard Wimp colours.
	 */
	unsigned int _tb_colour:8;

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
	/** Construct basic window.
	 * By default a basic window:
	 * - is not movable;
	 * - does not have a title bar;
	 * - does not have a back icon;
	 * - does not have a close icon;
	 * - does not have scroll bars;
	 * - does not have a toggle icon;
	 * - does not have an adjust icon;
	 * - does not have a border;
	 * - has a button type of 10;
	 * - has a title foreground colour of 7;
	 * - has a title background colour of 2;
	 * - has a work area foreground colour of 7; and
	 * - has a work area background colour of 1.
	 * @param size the required initial size
	 */
	basic_window(point size=point(640,512));

	/** Destroy basic window.
	 */
	virtual ~basic_window();

	virtual basic_window* as_window();
	virtual component* find(const point& p) const;
	virtual box bbox() const;
	virtual box min_bbox() const;
	virtual box min_wrap_bbox(const box& wbox) const;
	virtual wrap_type wrap_direction() const;
	virtual void resize() const;
	virtual void reformat(const point& origin,const box& pbbox);
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
	basic_window& add(component& c);

	/** Reformat window as dialogue box.
	 * This function is used in place of reformat(point&,box&) when
	 * the window is to be opened as a dialogue box.
	 * @param origin the new origin of this component, with respect to
	 *  its parent
	 * @param pbbox the proposed bounding box for this component, with
	 *  respect to its own origin
	 * @param level the level within the menu heirarchy (0=top level,
	 *  add 1 for each level down)
	 */
	void reformat(const point& origin,const box& pbbox,size_type level);

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

	/** Get border flag.
	 * @return true if window has a border, otherwise false
	 */
	bool border() const
		{ return _border; }

	/** Get button type.
	 * @return the work area button type
	 */
	int button() const
		{ return _button; }

	/** Get title bar background colour.
	 * This is one of the 16 standard Wimp colours.
	 * @return the title bar background colour
	 */
	int tb_colour() const
		{ return _tb_colour; }

	/** Get title bar foreground colour.
	 * This is one of the 16 standard Wimp colours.
	 * @return the title bar foreground colour
	 */
	int tf_colour() const
		{ return _tf_colour; }

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
	basic_window& title(const string& title,size_type capacity=0);

	/** Set movable flag.
	 * @param value true if the window is to be movable, otherwise false
	 * @return a reference to this
	 */
	basic_window& movable(bool value);

	/** Set back icon flag.
	 * @param value true if the window is to have a back icon, otherwise
	 *  false
	 * @return a reference to this
	 */
	basic_window& back_icon(bool value);

	/** Set close icon flag.
	 * @param value true if the window is to have a close icon, otherwise
	 *  false
	 * @return a reference to this
	 */
	basic_window& close_icon(bool value);

	/** Set title bar flag.
	 * @param value true if the window is to have a title bar, otherwise
	 *  false
	 * @return a reference to this
	 */
	basic_window& title_bar(bool value);

	/** Set toggle-size icon flag.
	 * @param value true if the window is to have a toggle-size icon,
	 *  otherwise false
	 * @return a reference to this
	 */
	basic_window& toggle_icon(bool value);

	/** Set horizontal scroll bar flag.
	 * @param value true if the window is to have a horizontal scroll bar,
	 *  otherwise false
	 * @return a reference to this
	 */
	basic_window& x_scroll_bar(bool value);

	/** Set vertical scroll bar flag.
	 * @param value true if the window is to have a vertical scroll bar,
	 *  otherwise false
	 * @return a reference to this
	 */
	basic_window& y_scroll_bar(bool value);

	/** Set adjust-size icon flag.
	 * @param value true if the window is to have an adjust-size icon,
	 *  otherwise false
	 * @return a reference to this
	 */
	basic_window& adjust_icon(bool value);

	/** Set border flag.
	 * @param value true if the window is to have a border, otherwise false
	 * @return a reference to this
	 */
	basic_window& border(bool value);

	/** Set button type.
	 * This must be set before the window is created by the Wimp.
	 * @param value the required button type
	 * @return a reference to this
	 */
	basic_window& button(int value);

	/** Set title bar background colour.
	 * This is one of the 16 standard Wimp colours.
	 * @param colour the required title bar background colour
	 * @return a reference to this
	 */
	basic_window& tb_colour(int colour);

	/** Set title bar foreground colour.
	 * This is one of the 16 standard Wimp colours.
	 * @param colour the required title bar foreground colour
	 * @return a reference to this
	 */
	basic_window& tf_colour(int colour);

	/** Set window background colour.
	 * This is one of the 16 standard Wimp colours.
	 * @param colour the required window background colour
	 * @return a reference to this
	 */
	basic_window& wb_colour(int colour);

	/** Set window foreground colour.
	 * This is one of the 16 standard Wimp colours.
	 * @param colour the required window foreground colour
	 * @return a reference to this
	 */
	basic_window& wf_colour(int colour);

	/** Default handler for close_window events.
	 * If this handler remains active then the window will close itself
	 * (by removing itself from the component heirarchy) when the close
	 * button is pressed.
	 * @param ev the close_window event
	 */
	void handle_event(events::close_window& ev);

	/** Default handler for auto-scroll events.
	 * If this handler remains active then the window will automatically
	 * respond to auto-scroll events.
	 * @param ev the auto-scroll event
	 */
	void handle_event(events::auto_scroll& ev);

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

	/** Force update of component.
	 * The effect of this function is similar to force_redraw, except
	 * that:
	 * - the redraw happens immediately; and
	 * - the work area background is not cleared.
	 *
	 * This function overrides component::force_update.  It is not
	 * virtual, but the effect is the same as if it were.
	 */
	void force_update();

	/** Force update of given area.
	 * The effect of this function is similar to force_redraw, except
	 * that:
	 * - the redraw happens immediately; and
	 * - the work area background is not cleared.
	 *
	 * This function overrides component::force_update.  It is not
	 * virtual, but the effect is the same as if it were.
	 * @param clip the bounding box of the region to be redrawn,
	 *  with respect to the origin of this component
	 */
	void force_update(const box& clip);

	/** Get handle of window in front of this one.
	 * @internal
	 * @return the handle of the window in front of this one, or -1 if
	 *  this window is at the top of the window stack
	 */
	int behind() const;
private:
	/** Find target of event.
	 * @param pos the coordinates of the event, with respect to the screen
	 * @return the corresponding event target
	 */
	component* find_target(const point& pos);

	/** Find target for current focus.
	 * @return the corresponding event target
	 */
	component* find_focus_target();

	/** Reformat window.
	 * @internal
	 * This function is equivalent to reformat(const point&,const box&),
	 * except that the required position in the window stack can (and must)
	 * be specified.
	 * @param origin the new origin of this component, with respect to
	 *  its parent
	 * @param pbbox the proposed bounding box for this component, with
	 *  respect to its own origin
	 * @param behind the window behind which this one should be opened,
	 *  or -1 to open at the top of the window stack
	 */
	void _reformat(const point& origin,const box& pbbox,int behind);

	/** Calculate extent.
	 * @internal
	 * @param bbox the proposed bounding box of this component
	 * @return the extent
	 */
	box calculate_extent(const box& bbox);
};

} /* namespace desktop */
} /* namespace rtk */

#endif
