// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_UTIL_VERSION
#define _RTK_UTIL_VERSION

namespace rtk {
namespace util {

/** Get RTK version number.
 * The version number is returned as an array of three unsigned integers.
 *
 * The first component is the major version number.  It indicates
 * a major change to the existing interfaces.
 *
 * The second component is the minor version number.  It indicates
 * an addition or minor change to the existing interfaces.
 *
 * The third component is the patch level.  It indicates that a
 * fault has been corrected, and does not add to or change the existing
 * interfaces.
 *
 * A change to the minor version number or patch level should not
 * usually break existing client software.  A change to the major
 * version number is likely to.
 * @return the RTK version number
 */
const unsigned int* version();

/** Set RTK version number.
 * The value passed to this function should be the latest RTK version
 * number that the client program is aware of (or wishes to be aware of).
 *
 * Providing this information is not compulsory, but doing so may improve
 * the extent to which the RTK is able to maintain backwards compatibility
 * in the future.
 *
 * This function does not affect the value returned when the version
 * number is read (which is always the actual version number).
 *
 * The version number is passed as an array of three unsigned integers.
 * @param version the RTK version number
 */
void version(const unsigned int* version);

} /* namespace util */
} /* namespace rtk */

#endif
