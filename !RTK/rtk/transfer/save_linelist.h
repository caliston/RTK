// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003-2005 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_TRANSFER_SAVE_LINELIST
#define _RTK_TRANSFER_SAVE_LINELIST

#include <list>
#include <string>

#include "rtk/transfer/save.h"

namespace rtk {
namespace transfer {

/** A class for saving lines of text.
 * @deprecated Use transfer::save_lines in preference to this class.
 */
class save_linelist:
	public save
{
private:
	/** A pointer to the line list to be saved.
	 * This always points to a list<string>: it can never be null.
	 */
	std::list<string>* _linelist;

	/** The current position within _linelist. */
	std::list<string>::const_iterator _pos;

	/** The default value for _linelist. */
	std::list<string> _default_linelist;

	/** The end-of-line flag.
	 * When true, a line of text has been saved but the associated newline
	 * character has not.
	 */
	bool _eol;
public:
	/** Construct save_linelist operation. */
	save_linelist();

	/** Destroy save_linelist operation. */
	virtual ~save_linelist();
protected:
	virtual void start();
	virtual void get_block(const void** data,size_type* count);
	virtual void finish();
	virtual size_type estsize();
public:
	/** Get line list to be saved.
	 * The returned list reference is writable, so it is possible to add
	 * strings to the default list and save that (as opposed to specifying
	 * an external list).
	 * @return a reference to the data to be saved
	 */
	std::list<string>& linelist()
		{ return *_linelist; }

	/** Set line list to be saved.
	 * The default list may be reactivated by specifying default_linelist()
	 * as the argument to this function.
	 * @param linelist the data to be saved
	 */
	save_linelist& linelist(std::list<string>& linelist);

	/** Get default line list.
	 * This is the list that is used if an external list has not been
	 * specified.  It is the default value of linelist().
	 * @return a reference to the default line list
	 */
	std::list<string>& default_linelist()
		{ return _default_linelist; }
};

}; /* namespace transfer */
}; /* namespace rtk */

#endif
