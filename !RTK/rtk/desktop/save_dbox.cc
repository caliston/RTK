// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003-2004 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#include <sstream>
#include <iomanip>

#include "rtk/os/exception.h"
#include "rtk/os/os.h"
#include "rtk/os/wimp.h"
#include "rtk/desktop/save_dbox.h"
#include "rtk/desktop/application.h"
#include "rtk/events/save_to_file.h"
#include "rtk/events/save_to_app.h"
#include "rtk/transfer/save.h"

namespace {

/** Choose sprite name for save dialogue.
 * If no preferred sprite name is supplied then one is chosen to match
 * the filetype.  If the preferred or chosen sprite name does not exist
 * then one of 'directory', 'applicaton' or 'file_xxx' is used instead.
 * @param filetype the filetype
 * @param sprname the preferred sprite name
 * @return the sprite name to be used by the save dialogue
 */
string sprite_name(unsigned int filetype,const string& sprname)
{
	string name(sprname);
	if (!name.length())
	{
		if (filetype==0x1000)
		{
			name="directory";
		}
		else if (filetype==0x2000)
		{
			name="application";
		}
		else
		{
			ostringstream out;
			out << "file_" << hex << setw(3) << filetype;
			name=out.str();
		}
	}
	try
	{
		rtk::os::Wimp_SpriteOp40(name.c_str(),0,0,0,0);
	}
	catch (rtk::os::exception& ex)
	{
		if (filetype==0x1000)
		{
			name="directory";
		}
		else if (filetype==0x2000)
		{
			name="application";
		}
		else
		{
			name="file_xxx";
		}
	}
	return name;
}

}; /* anonymous namespace */

namespace rtk {
namespace desktop {

save_dbox::save_dbox():
	_filetype(0xffd),
	_saveop(0)
{
	title("Save as");
	back_icon(false);
	close_icon(false);

	_filetype_icon.sprite_name(sprite_name(_filetype,_sprname));
	_filetype_icon.button(6);
	_filetype_icon.xfit(false);
	_filetype_icon.yfit(false);
	_filetype_icon.xbaseline(xbaseline_centre);

	_pathname_field.text("Untitled",255);
	_pathname_field.validation(_pathname_field.validation()+";A~ ");

	_cancel_button.text("Cancel");
	_save_button.text("Save");

	_selection_button.text("Selection");
	_selection_button.text_and_sprite(true);
	_selection_button.enabled(false);

	_row.add(_cancel_button);
	_row.add(_save_button);
	_row.xgap(16);

	_column.add(_filetype_icon,0);
	_column.add(_pathname_field,1);
	_column.add(_row,3);
	_column.ygap(8);
	_column.margin(16);

	inherited::add(_column);
}

save_dbox::~save_dbox()
{
	remove();
}

void save_dbox::handle_event(events::mouse_click& ev)
{
	if ((ev.target()==&_filetype_icon)&&(ev.buttons()&0x50))
	{
		// Dragging filetype icon.
		int flags;
		os::OS_Byte161(28,&flags);
		if (flags&0x02)
		{
			// Solid drags enabled.
			_filetype_icon.drag_sprite(_filetype_icon.bbox(),
				(os::sprite_area*)1,sprite_name(_filetype,_sprname));
		}
		else
		{
			// Solid drags disabled.
			_filetype_icon.drag_box(_filetype_icon.bbox());
		}
	}
	else if (ev.target()==&_save_button)
	{
		handle_save();
	}
	else if (ev.target()==&_cancel_button)
	{
		handle_cancel();
	}
}

void save_dbox::handle_event(events::user_drag_box& ev)
{
	component* target=this;
	if (_saveop)
	{
		if (application* app=parent_application())
		{
			app->add(*_saveop);
		}
		target=_saveop;
	}
	events::save_to_app ev2(*target,leafname());
	ev2.post();
}

void save_dbox::handle_event(events::key_pressed& ev)
{
	switch (ev.code())
	{
	case 0x0d:
		handle_save();
		ev.processed(true);
		break;
	case 0x1b:
		handle_cancel();
		ev.processed(true);
		break;
	}
}

string save_dbox::pathname() const
{
	return _pathname_field.text();
}

string save_dbox::leafname() const
{
	string pathname=_pathname_field.text();
	string::size_type f=pathname.rfind('.');
	if (f==string::npos) f=0; else ++f;
	return pathname.substr(f);
}

bool save_dbox::selection_button_present() const
{
	return _selection_button.parent();
}

bool save_dbox::selection_button_enabled() const
{
	return _selection_button.enabled();
}

bool save_dbox::selection_button_state() const
{
	return _selection_button.selected();
}

save_dbox& save_dbox::filetype(unsigned int filetype)
{
	_filetype=filetype;
	_filetype_icon.sprite_name(sprite_name(_filetype,_sprname));
	return *this;
}

save_dbox& save_dbox::spritename(const string& sprname)
{
	_sprname=sprname;
	_filetype_icon.sprite_name(sprite_name(_filetype,_sprname));
	return *this;
}

save_dbox& save_dbox::pathname(const string& pathname)
{
	_pathname_field.text(pathname);
	return *this;
}

save_dbox& save_dbox::selection_button_present(bool value)
{
	if (value)
	{
		_column.add(_selection_button,2);
	}
	else
	{
		_selection_button.remove();
	}
	return *this;
}

save_dbox& save_dbox::selection_button_enabled(bool value)
{
	_selection_button.enabled(value);
	return *this;
}

save_dbox& save_dbox::selection_button_state(bool value)
{
	_selection_button.selected(value);
	return *this;
}

save_dbox& save_dbox::attach_save(transfer::save& saveop)
{
	_saveop=&saveop;
	return *this;
}

save_dbox& save_dbox::detach_save()
{
	_saveop=0;
	return *this;
}

void save_dbox::handle_save()
{
	string pathname=_pathname_field.text();
	string::size_type f=pathname.find_first_of(".:<");
	if (f!=string::npos)
	{
		component* target=_saveop;
		if (!target) target=this;
		events::save_to_file ev(*target,_pathname_field.text(),
			_selection_button.selected());
		ev.post();
	}
	else
	{
		application* app=parent_application();
		const char* message="To save, drag the icon to a directory display";
		const char* title=(app)?app->name().c_str():"Unknown";
		os::Wimp_ReportError(0,message,title,0,0);
	}
}

void save_dbox::handle_cancel()
{
	remove();
}

}; /* namespace desktop */
}; /* namespace rtk */
