// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2004 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_UTIL_ARRAY_SIZE
#define _RTK_UTIL_ARRAY_SIZE

#include <stddef.h>

/** A template function to find the number of elements in a built-in array.
 * Only arrays are accepted (not pointers or iterators).  With moderate
 * optimisation the run-time cost of this function should be zero.
 * @param array the array
 * @return the number of elements in an array
 */
template<class value_type,size_t size>
inline size_t array_size(value_type (&array)[size])
{
	return size;
}

#endif
