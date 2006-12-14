// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_UTIL_REFCOUNT
#define _RTK_UTIL_REFCOUNT

namespace rtk {
namespace util {

/** A base class for reference-counted memory management.
 */
class refcount
{
private:
	/** The reference count. */
	unsigned int _count;
public:
	/** Construct reference count.
	 * The count begins at zero.
	 */
	refcount();

	/** Destroy reference count. */
	virtual ~refcount();

	/** Increment reference count. */
	void inc_count();

	/** Decrement reference count.
	 * The object automatically deletes itself when the count reaches zero.
	 */
	void dec_count();
};

} /* namespace util */
} /* namespace rtk */

#endif
