// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2005 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_UTIL_AUTO_FILEHANDLE
#define _RTK_UTIL_AUTO_FILEHANDLE

namespace rtk {
namespace util {

/** A class to represent a RISC OS file handle.
 * The underlying file handle is automatically closed when
 * the auto_filehandle is destroyed.
 */
class auto_filehandle
{
private:
	/** The underlying RISC OS file handle. */
	int _handle;
public:
	/** Create auto_filehandle.
	 * @param handle the underlying RISC OS file handle
	 */
	explicit auto_filehandle(int handle);

	/** Destroy auto_filehandle.
	 * The underlying RISC OS file handle is closed.
	 */
	~auto_filehandle();

	/** Get underlying RISC OS file handle.
	 * @return the underlying file handle
	 */
	operator int() const
		{ return _handle; }
};

} /* namespace util */
} /* namespace rtk */

#endif
