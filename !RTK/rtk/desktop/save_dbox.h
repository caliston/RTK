// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_DESKTOP_SAVE_DBOX
#define _RTK_DESKTOP_SAVE_DBOX

#include "rtk/desktop/column_layout.h"
#include "rtk/desktop/row_layout.h"
#include "rtk/desktop/icon.h"
#include "rtk/desktop/action_button.h"
#include "rtk/desktop/default_button.h"
#include "rtk/desktop/option_button.h"
#include "rtk/desktop/writable_field.h"
#include "rtk/desktop/window.h"
#include "rtk/events/mouse_click.h"
#include "rtk/events/user_drag_box.h"
#include "rtk/events/key_pressed.h"

namespace rtk {
namespace transfer {

class save;

}; /* namespace transfer */

namespace desktop {

/** A class to represent a save-as dialogue box. */
class save_dbox:
	public window,
	public events::mouse_click::handler,
	public events::user_drag_box::handler,
	public events::key_pressed::handler
{
private:
	/** The class from which this one is derived. */
	typedef window inherited;

	/** The filetype of the file to be saved. */
	unsigned int _filetype;

	/** The icon containing the filetype sprite. */
	icon _filetype_icon;

	/** The field containing the pathname/leafname. */
	writable_field _pathname_field;

	/** The cancel button. */
	action_button _cancel_button;

	/** The save button. */
	default_button _save_button;

	/** The selection button. */
	option_button _selection_button;

	/** The column containing everything. */
	column_layout _column;

	/** The row containing the cancel and save buttons. */
	row_layout _row;

	/** The attached save operation. */
	transfer::save* _saveop;
public:
	/** Construct save-as dialogue box.
	 * By default, a save-as dialogue box:
	 * - has a default filename of "Untitled"; and
	 * - does not have a selection button.
	 */
	save_dbox();

	/** Destroy save-as dialogue box. */
	virtual ~save_dbox();

	virtual void handle_event(events::mouse_click& ev);
	virtual void handle_event(events::user_drag_box& ev);
	virtual void handle_event(events::key_pressed& ev);

	/** Get filetype.
	 * @return the filetype
	 */
	unsigned int filetype() const
		{ return _filetype; }

	/** Get pathname.
	 * @return the pathname
	 */
	string pathname() const;

	/** Get leafname.
	 * This is the final component of the pathname.
	 * @return the leafname
	 */
	string leafname() const;

	/** Get selection button present flag.
	 * @return true if the dialogue box has a selection button,
	 * otherwise false
	 */
	bool selection_button_present() const;

	/** Get selection button enabled flag.
	 * If false then the selection button (if present) is greyed out.
	 * @return true if the selection button is enabled, otherwise false
	 */
	bool selection_button_enabled() const;

	/** Get selection button state.
	 * @return true if selection button active, otherwise false
	 */
	bool selection_button_state() const;

	/** Get save operation.
	 * @return the attached save operation, or 0 if none
	 */
	transfer::save* save() const
		{ return _saveop; }

	/** Set filetype.
	 * @param filetype the required filetype
	 */
	save_dbox& filetype(unsigned int filetype);

	/** Set pathname.
	 * @param pathname the required pathname
	 */
	save_dbox& pathname(const string& pathname);

	/** Set selection button present flag.
	 * @param value true if the dialogue box is to have a selection button,
	 *  otherwise false
	 * @return a reference to this
	 */
	save_dbox& selection_button_present(bool value);

	/** Set selection button enabled flag.
	 * @param value true if the selection button is to be enabled,
	 *  otherwise false
	 * @return a reference to this
	 */
	save_dbox& selection_button_enabled(bool value);

	/** Set selection button state.
	 * @param value true if the selection button is to be active,
	 *  otherwise false
	 * @return a reference to this
	 */
	save_dbox& selection_button_state(bool value);

	/** Attach save operation.
	 * If a save operation is attached, it will be used to save the data
	 * when requested by the user.
	 * @param sa the required save action
	 */
	save_dbox& attach_save(transfer::save& saveop);

	/** Detach save operaton. */
	save_dbox& detach_save();
private:
	/** Handle save request.
	 * This results from the user clicking on the save button or
	 * pressing the return key.
	 */
	void handle_save();

	/** Handle cancel request.
	 * This results from the user clicking on the cancel button or
	 * pressing the escape key.
	 */
	void handle_cancel();

	/** Get the sprite name that corresponds to a filetype.
	 * For a filetype of 0x123 the sprite would be 'file_123' if that
	 * name exists in the Wimp sprite pool, otherwise 'file_xxx'.
	 * @param filetype the filetype
	 * @return the sprite name that corresponds to the filetype
	 */
	static string sprite_name(unsigned int filetype);
};

}; /* namespace desktop */
}; /* namespace rtk */

#endif
