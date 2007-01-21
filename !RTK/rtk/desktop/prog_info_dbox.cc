// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#include "rtk/desktop/display_field.h"
#include "rtk/desktop/prog_info_dbox.h"
#include "rtk/desktop/min_size.h"

namespace rtk {
namespace desktop {

prog_info_dbox::prog_info_dbox()
{
	title("About this program");
	close_icon(false);
	back_icon(false);

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

	min_size& name_size=*new min_size();
	name_size.add(name_icon);
	name_size.minimum_size(point(136,0));

	display_field& value_icon=*new display_field();
	value_icon.text(value);

	min_size& value_size=*new min_size();
	value_size.add(value_icon);
	value_size.minimum_size(point(504,0));

	_grid.add(name_size,0,ycells).add(value_size,1,ycells);
	return *this;
}

} /* namespace desktop */
} /* namespace rtk */
