// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_TRANSFER_LOAD_LINELIST
#define _RTK_TRANSFER_LOAD_LINELIST

#include <list>
#include <string>

#include "rtk/transfer/load.h"

namespace rtk {
namespace transfer {

using std::string;

/** A class for loading lines of text.
 * @deprecated Use transfer::load_lines in preference to this class. 
 */
class load_linelist:
	public load
{
private:
	/** A pointer to the list of lines that have been loaded.
	 * This always points to a list of string: it can never be null.
	 */
	std::list<string>* _linelist;

	/** The default value for _linelist. */
	std::list<string> _default_linelist;

	/** The buffer used to receive data before it is parsed into lines. */
	char* _buffer;

	/** The size of _buffer. */
	size_type _buffer_size;

	/** The newline flag.
	 * True if the next character parsed should begin a new line,
	 * false if it should be appended to the previous line.
	 */
	bool _newline;
public:
	/** Construct load_linelist object. */
	load_linelist(size_type buffer_size=0x400);

	/** Destroy load_linelist object. */
	virtual ~load_linelist();
protected:
	virtual void start(size_type estsize);
	virtual void get_block(void** data,size_type* size);
	virtual void put_block(size_type count);
	virtual void finish();
public:
	/** Get line list.
	 * The returned list reference is writable, so it is possible to swap
	 * the list content into an external list.
	 * @return a reference to the data that has been loaded
	 */
	std::list<string>& linelist()
		{ return *_linelist; }

	/** Set line list.
	 * The default list may be reactivated by specifying default_linelist()
	 * as the argument to this function.
	 * @param linelist the list into which the data is loaded
	 */
	load_linelist& linelist(std::list<string>& linelist);

	/** Get default line list.
	 * This is the list that is used if an external list has not been
	 * specified.  It is the default value of linelist().
	 * @return a reference to the default line list
	 */
	std::list<string>& default_linelist()
		{ return _default_linelist; }
};

}; /* namespace transfer */
}; /* namespace rtk */

#endif
