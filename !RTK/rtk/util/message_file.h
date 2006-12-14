// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2004-2005 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_UTIL_MESSAGE_FILE
#define _RTK_UTIL_MESSAGE_FILE

#include <string>

namespace rtk {
namespace util {

using std::string;

/** A class to represent a RISC OS message file.
 */
class message_file
{
private:
	/** The message file descriptor. */
	int _descriptor;

	/** The buffer used to hold the message file. */
	char* _buffer;
public:
	/** Open message file.
	 * @param pathname the pathname of the message file.
	 */
	message_file(const string& pathname);

	/** Close message file. */
	~message_file();

public:
	/** Look up token.
	 * @param token the token
	 * @return the message
	 */
	string lookup(const string& token);
private:
	/** Look up token with specified number of arguments.
	 * @param token the token
	 * @param arg0 argument 0 (if args>0)
	 * @param arg1 argument 1 (if args>1)
	 * @param arg2 argument 2 (if args>2)
	 * @param arg3 argument 3 (if args>3)
	 * @param args the number of arguments
	 * @return the message
	 */
	string lookup(const string& token,const string& arg0,const string& arg1,
		const string& arg2,const string& arg3,unsigned int args);
public:
	/** Look up token with one argument.
	 * @param token the token
	 * @param arg0 argument 0
	 * @return the message
	 */
	string lookup(const string& token,const string& arg0);

	/** Look up token with two arguments.
	 * @param token the token
	 * @param arg0 argument 0
	 * @param arg1 argument 1
	 * @return the message
	 */
	string lookup(const string& token,const string& arg0,const string& arg1);

	/** Look up token with three arguments.
	 * @param token the token
	 * @param arg0 argument 0
	 * @param arg1 argument 1
	 * @param arg2 argument 2
	 * @return the message
	 */
	string lookup(const string& token,const string& arg0,const string& arg1,
		const string& arg2);

	/** Look up token with four arguments.
	 * @param token the token
	 * @param arg0 argument 0
	 * @param arg1 argument 1
	 * @param arg2 argument 2
	 * @param arg3 argument 3
	 * @return the message
	 */
	string lookup(const string& token,const string& arg0,const string& arg1,
		const string& arg2,const string& arg3);
};

} /* namespace util */
} /* namespace rtk */

#endif
