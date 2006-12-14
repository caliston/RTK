// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#include "rtk/desktop/display_field.h"
#include "rtk/desktop/prog_info_dbox.h"

namespace rtk {
namespace desktop {

prog_info_dbox::prog_info_dbox()
{
	title("About this program");
	close_icon(false);

	_grid.xgap(8).ygap(8);
	_grid.margin(8);
	inherited::add(_grid);
}

prog_info_dbox::~prog_info_dbox()
{
	remove();
}

prog_info_dbox& prog_info_dbox::add(const string& name,const string& value)
{
	int ycells=_grid.ycells();

	icon& name_icon=*new icon();
	name_icon.text(name);
	name_icon.rjustify(true);
	name_icon.xbaseline(xbaseline_right);

	display_field& value_icon=*new display_field();
	value_icon.text(value);

	_grid.add(name_icon,0,ycells).add(value_icon,1,ycells);
	return *this;
}

} /* namespace desktop */
} /* namespace rtk */
