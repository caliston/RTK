// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2006 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_UTIL_LEXICAL_CAST
#define _RTK_UTIL_LEXICAL_CAST

#include <string>
#include <sstream>

namespace rtk {
namespace util {

/** Perform a lexical cast.
 * A value of the input type is converted to the output type according to
 * its representation as a character string.  Any characters which cannot
 * be converted are discarded.
 * @param value the value to be converted
 * @return the converted value
 */
template<typename output_type,typename input_type>
output_type lexical_cast(const input_type& value)
{
	output_type result=output_type();
	std::stringstream buffer;
	buffer << value;
	buffer >> result;
	return result;
}

} /* namespace util */
} /* namespace rtk */

#endif
