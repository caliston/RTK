// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2004 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_DESKTOP_DCS_DBOX
#define _RTK_DESKTOP_DCS_DBOX

#include "rtk/desktop/row_layout.h"
#include "rtk/desktop/column_layout.h"
#include "rtk/desktop/label.h"
#include "rtk/desktop/action_button.h"
#include "rtk/desktop/default_button.h"
#include "rtk/desktop/window.h"
#include "rtk/events/redirection.h"
#include "rtk/events/mouse_click.h"
#include "rtk/events/key_pressed.h"

namespace rtk {
namespace desktop {

class dcs_dbox:
	public window,
	public events::redirection,
	public events::mouse_click::handler,
	public events::key_pressed::handler
{
private:
	/** The class from which this one is derived. */
	typedef window inherited;

	/** The message stating what is to be discarded or saved. */
	label _message;

	/** The discard button. */
	mutable action_button _discard_button;

	/** The cancel button. */
	mutable action_button _cancel_button;

	/** The save button. */
	mutable default_button _save_button;

	/** The row containing the buttons. */
	row_layout _row;

	/** The column containing everything. */
	column_layout _column;
public:
	/** Construct discard-cancel-save dialogue box. */
	dcs_dbox();

	/** Destroy discard-cancel-save dialogue box. */
	virtual ~dcs_dbox();

	virtual void resize() const;
	virtual void handle_event(events::mouse_click& ev);
	virtual void handle_event(events::key_pressed& ev);

private:
	/** Handle discard request.
	 * This results from the user clicking on the discard button.
	 */
	void handle_discard();

	/** Handle cancel request.
	 * This results from the user clicking on the cancel button or
	 * pressing the escape key.
	 */
	void handle_cancel();

	/** Handle save request.
	 * This results from the user clicking on the save button or
	 * pressing the enter key.
	 */
	void handle_save();
};

}; /* namespace desktop */
}; /* namespace rtk */

#endif
