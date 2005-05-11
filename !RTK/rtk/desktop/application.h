// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003-2005 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_DESKTOP_APPLICATION
#define _RTK_DESKTOP_APPLICATION

#include <vector>
#include <queue>
#include <map>
#include <string>

#include "rtk/util/refcount.h"
#include "rtk/desktop/component.h"
#include "rtk/events/quit.h"
#include "rtk/events/datasave.h"
#include "rtk/events/dataload.h"
#include "rtk/events/dataopen.h"
#include "rtk/events/reopen_menu.h"

namespace rtk {
namespace os {

union wimp_block;

}; /* namespace os */

namespace transfer {

class save;
class load;

}; /* namespace transfer */

namespace desktop {

class basic_window;
class icon;
class menu;

/** A class to represent a RISC OS application.
 * A name must be supplied when the application is constructed.
 * At present, applications do not correctly implement component::find().
 */
class application:
	public component,
	public events::quit::handler,
	public events::datasave::handler,
	public events::dataload::handler,
	public events::dataopen::handler,
	public events::reopen_menu::handler
{
private:
	/** The class from which this one is derived. */
	typedef component inherited;
public:
	/** A type for representing indices. */
	typedef unsigned int size_type;
private:
	/** A structure to represent an outbound Wimp message.
	 * Since this contains a pointer to (as opposed to a copy) of the
	 * Wimp event block, the latter must remain valid for the lifetime
	 * of this object. 
	 */
	struct message
	{
		/** The Wimp event code for the message. */
		int wimpcode;
		/** A pointer to the Wimp event block containing the message. */
		os::wimp_block* wimpblock;
		/** The window or task handle to which the message should be sent. */
		int whandle;
		/** The icon handle to which the message should be sent. */
		int ihandle;
	};

	/** The RISC OS task handle. */
	int _handle;

	/** The task name. */
	string _name;

	/** A map from window handles to window components.
	 * All descendant windows that currently have a RISC OS window handle
	 * are included.
	 */
	std::map<int,basic_window*> _whandles;

	/** A map from icon handles to icon components.
	 * Descendant icons are included if they are not descended from a
	 * window and they currently have a RISC OS window handle.
	 */
	std::map<int,icon*> _ihandles;

	/** A list of child windows.
	 * Windows are listed if they are an immediate child of this
	 * application, but not if they are open as a dialogue box.
	 */
	std::vector<basic_window*> _windows;

	/** The list of child icons.
	 * Icons are listed if they are an immediate child of this
	 * application.
	 */
	std::vector<icon*> _icons;

	/** The list of current child menus.
	 * Menus are listed if they are an immediate child of this
	 * application.  Their position within this vector corresponds
	 * to their depth with in the menu heirarchy (first=top).
	 * It follows that there can be at most one child at each level.
	 * (Note that the parent of a menu should be either the
	 * application component or null.)
	 */
	std::vector<menu*> _menus;

	/** The list of current menu data blocks.
	 * Menu data blocks are listed if there is a possibility that they
	 * are being used by the Wimp.  Their position within this vector
	 * corresponds to their depth with in the menu heirarchy (first=top).
	 * It follows that there can be at most one child at each level.
	 */
	std::vector<util::refcount*> _mdata;

	/** The current dialogue box.
	 */
	basic_window* _dbox;

	/** The current dialogue box level.
	 */
	size_type _dbox_level;

	/** A list of components which need to receive null events. */
	std::vector<component*> _null;

	/** The owner of the current drag action. */
	component* _current_drag;

	/** The drag sprite flag.
	 * True if using DragASprite module for _current_drag, otherwise false.
	 */
	bool _drag_sprite;

	/** The current caret or selection owner, or 0 if none. */
	component* _current_selection;

	/** The current clipboard owner, or 0 if none. */
	component* _current_clipboard;

	/** The current load operation. */
	transfer::load* _current_load;

	/** The current save operation. */
	transfer::save* _current_save;

	/** The outbound message queue.
	 * Messages are added to the back of the queue and taken from the
	 * front.  Transmission occurs automatically once a message has
	 * been put in the queue, but is limited to a maximum of one
	 * message per polling interval
	 */
	std::queue<message> _message_queue;

	/** The mask passed to Wimp_Poll. */
	unsigned int _wimp_mask;

	/** The quit flag.
	 * True if this application should terminate gracefully when control
	 * returns to the main polling loop.
	 */
	bool _quit;
public:
	/** Construct application.
	 * @param name the name of this application
	 */
	application(const string& name);

	/** Destroy application. */
	virtual ~application();

	virtual application* as_application();
	virtual box min_bbox() const;
	virtual void resize() const;
	virtual void reformat(const point& origin,const box& pbbox);
protected:
	virtual void remove_notify(component& c);
public:
	/** Add icon.
	 * When an icon is added to an application it appears on the icon bar.
	 * @param ic the icon to be added
	 */
	application& add(icon& ic);

	/** Add window.
	 * @param w the window to be added
	 * @param p the required initial origin
	 */
	application& add(basic_window& w,const point& p);

	/** Add window as dialogue box.
	 * @param w the window to be added as a dialogue box
	 * @param p the required initial origin
	 * @param level the level within the menu heirarchy (0=top level,
	    add 1 for each level down)
	 */
	application& add(basic_window& w,const point& p,size_type level);

	/** Add menu.
	 * @param m the menu to be added
	 * @param p the required position of the origin
	 * @param level the level within the menu heirarchy (0=top level,
	    add 1 for each level down)
	 */
	application& add(menu& m,const point& p,size_type level=0);

	/** Add load operation.
	 * @param loadop the load operation to be added
	 */
	application& add(transfer::load& loadop);

	/** Add save operation.
	 * @param saveop the save operation to be added
	 */
	application& add(transfer::save& saveop);

	/** Get name.
	 * The name of an application is fixed when it is constructed.
	 * @return the name of this application
	 */
	string& name()
		{ return _name; }

	/** Run this application.
	 * This method contains the main polling loop.  It should be
	 * called once from main().
	 */
	void run();

	/** Terminate this application.
	 * This method causes the application to terminate gracefully
	 * when control returns to the main polling loop.  Take case
	 * not to confuse this method with the global standard library
	 * function terminate(), which performs an immediate non-graceful
	 * termination.
	 */
	void terminate();

	/** Default handler for quit events.
	 * If this handler remains registered then the application
	 * will terminate gracefully when it receives a Message_Quit.
	 * Well-behaved applications are obliged to do this, so it
	 * should not be necessary to override this handler.  Any
	 * required clean-up actions can (and should) be performed by
	 * the relevant destructors.
	 * @param ev the quit event
	 */
	virtual void handle_event(events::quit& ev);

	/** Default handler for datasave events.
	 * @param ev the datasave event
	 */
	virtual void handle_event(events::datasave& ev);

	/** Default handler for dataload events.
	 * @param ev the dataload event
	 */
	virtual void handle_event(events::dataload& ev);

	/** Default handler for dataopen events.
	 * @param ev the dataopen event
	 */
	virtual void handle_event(events::dataopen& ev);

	/** Default handler for reopen-menu events.
	 * If this handler remains registered then the application
	 * will re-open the current menu tree when a menu selection
	 * is made using the adjust button.  This behaviour can be
	 * disabled by setting the auto-reopen attribute of the menu
	 * to false.
	 * @param ev the reopen_menu event
	 */
	virtual void handle_event(events::reopen_menu& ev);

	/** Deliver Wimp event block.
	 * @internal
	 * The event block is converted into a suitable event object and
	 * posted to the appropriate target.
	 * @param wimpcode the Wimp event code
	 * @param wimpblock the Wimp event block
	 */
	void deliver_wimp_block(int wimpcode,os::wimp_block& wimpblock);

	/** Deliver Wimp message.
	 * @internal
	 * The event block (which should contain a message) is converted
	 * into a suitable event object and posted to the appropriate target.
	 * @param wimpcode the Wimp event code
	 * @param wimpblock the Wimp event block
	 */
	void deliver_message(int wimpcode,os::wimp_block& wimpblock);

	/** Deliver Wimp message acknowledgement.
	 * @internal
	 * The event block (which should contain a message) is converted
	 * into a suitable event object and posted to the appropriate target.
	 * @param wimpcode the Wimp event code
	 * @param wimpblock the Wimp event block
	 */
	void deliver_message_ack(int wimpcode,os::wimp_block& wimpblock);

	/** Send Wimp message.
	 * The event block must be dynamically allocated on the heap.
	 * It is automatically deleted once the message has been sent.
	 * @param wimpcode the Wimp event code
	 * @param wimpblock the Wimp event block
	 * @param whandle the window or task handle to which the message
	 *  should be sent (-2 for icon bar)
	 * @param ihandle the icon handle to which the message should be
	 *  send (if whandle==-2)
	 */
	void send_message(int wimpcode,os::wimp_block& wimpblock,int whandle,
		int ihandle=0);

	/** Get load operation for filetype.
	 * The default behaviour is not to handle any filetype,
	 * @param filetype the filetype
	 * @return the load operation for that filetype, or 0 if none
	 */
	virtual transfer::load* auto_load(unsigned int filetype);

	/** Get current selection owner.
	 * @return the current selection owner
	 */
	component* current_selection() const;

	/** Get current clipboard owner.
	 * @return the current clipboard owner
	 */
	component* current_clipboard() const;

	/** Register window.
	 * A window should be registered while it has a Wimp window handle
	 * in its possession.  This allows it to receive events from the
	 * Wimp.  Windows should register with the application from which
	 * they are descended (even if they are more closely descended from
	 * another window).
	 * @param w the window to be registered
	 */
	void register_window(basic_window& w);

	/** Register icon.
	 * An icon should be registered while it has a Wimp icon handle in
	 * its possession.  This allows it to receive events from the Wimp.
	 * Icons should register with the application or window from which
	 * they are most closely descended.
	 * @param ic the icon to be registered.
	 */
	void register_icon(icon& ic);

	/** Register menu data.
	 * A menu data block should be registered whenever it is passed
	 * to the Wimp.  The application object is responsible for
	 * de-registering it, and will not do so while there is a
	 * possibility that the Wimp is still using it.
	 *
	 * The reference count of the data block is incremented on
	 * registration and decremented on de-registration.
	 * @param mdata the menu data block to be registered
	 * @param level the level within the menu heirarchy (0=top level,
	    add 1 for each level down)
	 */
	void register_menu_data(util::refcount* mdata,unsigned int level);

	/** Register null action.
	 * @param c the component to be registered to receive null events
	 */
	void register_null(component& c);

	/** Register drag action.
	 * @param c the component to be registered as the owner of the
	 *  current drag action
	 * @param sprite true if using DragASprite module, otherwise false
	 */
	void register_drag(component& c,bool sprite=false);

	/** Register caret or selection owner.
	 * @param c the component to be registered as the caret or selection
	 *  owner
	 */
	void register_selection(component& c);

	/** Register clipboard owner.
	 * @param c the component to be registered as the clipboard owner
	 */
	void register_clipboard(component& c);

	/** Deregister window.
	 * This must be done while w->handle() returns the same value as
	 * when the window was registered.
	 * @param w the window to be deregistered
	 */
	void deregister_window(basic_window& w);

	/** Deregister icon.
	 * This must be done while ic->handle() returns the same value as
	 * when the icon was registered.
	 * @param ic the icon to be deregistered
	 */
	void deregister_icon(icon& ic);

	/** Deregister null action.
	 * @param c the component to be reregistered
	 */
	void deregister_null(component& c);

	/** Deregister drag action.
	 * @param c the component to be deregistered
	 */
	void deregister_drag(component& c);

	/** Deregister caret or selection owner.
	 * @param c the component to be deregistered
	 */
	void deregister_selection(component& c);

	/** Deregister clipboard owner.
	 * @param c the component to be deregistered
	 */
	void deregister_clipboard(component& c);

	/** Find window.
	 * @param handle the window handle
	 * @return the window, or 0 if not found
	 */
	basic_window* find_window(int handle) const;

	/** Find icon.
	 * @param handle the icon handle
	 * @return the icon, or 0 if not found
	 */
	icon* find_icon(int handle) const;

	/** Find menu.
	 * At present, only the top-level menu is found by this function.
	 * @param handle the address of the menu data
	 * @return the menu, or 0 if not found
	 */
	menu* find_menu(int handle) const;

	/** Get RISC OS task handle.
	 * @return the RISC OS task handle
	 */
	int handle()
		{ return _handle; }
private:
	/** Remove menus and dialogue boxes at or below given level.
	 * @param the uppermost level to be removed (0=top level,
	 *  add 1 for each level down)
	 */
	void remove_menus(size_type level);

	/** Remove menu data for menus at or below given level.
	 * @param the uppermost level to be removed (0=top level,
	 *  add 1 for each level down)
	 */
	void remove_menu_data(size_type level);
};

}; /* namespace desktop */
}; /* namespace rtk */

#endif
