// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2004 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_OS_MESSAGETRANS
#define _RTK_OS_MESSAGETRANS

namespace rtk {
namespace os {

/* Read information about message file.
 * @param pathname the pathname of the message file
 * @param _flags a buffer for the returned flags
 * @param _size a buffer for the returned buffer size
 */
void MessageTrans_FileInfo(const char* pathname,int* _flags,
	unsigned int* _size);

/** Open message file.
 * @param descriptor a buffer for the file descriptor
 * @param pathname the pathname of the message file
 * @param buffer a buffer to hold the message file, or 0 to use the RMA
 */
void MessageTrans_OpenFile(int* descriptor,const char* pathname,
	void* buffer);

/** Translate from message token to string
 * @param descriptor the file descriptor, or 0 to use the global message file
 * @param token the token to be translated
 * @param buffer a buffer to hold the translated message, or 0 to use directly
 * @param size the size of the buffer (if there is one)
 * @param arg0 the text for '%0', or 0 for no substitution
 * @param arg1 the text for '%1', or 0 for no substitution
 * @param arg2 the text for '%2', or 0 for no substitution
 * @param arg3 the text for '%3', or 0 for no substitution
 * @param _token a pointer to the token terminator
 * @param _buffer a pointer to the result string
 * @param _size the size of the result string (excluding the terminator)
 */
void MessageTrans_Lookup(int* descriptor,const char* token,char* buffer,
	unsigned int size,const char* arg0,const char* arg1,const char* arg2,
	const char* arg3,const char** _token,const char** _buffer,
	unsigned int* _size);

/** Close message file.
 * @param descriptor the file descriptor
 */
void MessageTrans_CloseFile(int* descriptor);

}; /* namespace os */
}; /* namespace rtk */

#endif
