// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_OS_OS
#define _RTK_OS_OS

#include "rtk/graphics/point.h"
#include "rtk/graphics/box.h"

namespace rtk {
namespace os {

using rtk::graphics::point;
using rtk::graphics::box;

struct sprite;
struct sprite_area;

/** Read byte from CMOS RAM.
 * @param address the address to read
 * @param _value a buffer for the returned value
 */
void OS_Byte161(int address,int* _value);

/** Read EOF status.
 * @param handle the file handle
 * @param _eof a buffer for the returned EOF status (true=EOF)
 */
void OS_Args5(int handle,bool* _eof);

/** Open file.
 * @param code the reason code
 * @param name the object name
 * @param path the path (if any)
 * @param _handle a buffer for the returned file handle
 */
void OS_Find(int code,const char* name,const char* path,int* _handle);

/** Close file.
 * @param handle the file handle
 */
void OS_Find0(int handle);

/** Write bytes to file.
 * @param handle the file handle
 * @param buffer the data to be written
 * @param count the number of bytes to be written
 * @param _fp a buffer for the returned file pointer
 */
void OS_GBPB2(int handle,const void* buffer,unsigned int count,
	unsigned int* _fp);

/** Read sprite information (given pointer).
 * @param area the sprite area
 * @param sp the sprite pointer
 * @param _xsize a buffer for the returned horizontal size
 * @param _ysize a buffer for the returned vertical size
 * @param _mask a buffer for the returned mask status
 * @param _mode a buffer for the returned screen mode
 */
void OS_SpriteOp40(sprite_area* area,sprite* sp,int* _xsize,int* _ysize,
	int* _mask,int* _mode);

/** Read sprite information (given name).
 * @param area the sprite area
 * @param name the sprite name
 * @param _xsize a buffer for the returned horizontal size
 * @param _ysize a buffer for the returned vertical size
 * @param _mask a buffer for the returned mask status
 * @param _mode a buffer for the returned screen mode
 */
void OS_SpriteOp40(sprite_area* area,const char* name,int* _xsize,int* _ysize,
	int* _mask,int* _mode);

/** Read mode variable.
 * @param index the variable number
 * @param _value a buffer for the returned value
 */
void OS_ReadModeVariable(int index,int* _value);

/** Read monotonic time.
 * @param _time a buffer for the returned time (in centiseconds)
 */
void OS_ReadMonotonicTime(unsigned int* _time);

/** Plot point.
 * @param code the plot action code
 * @param p the point to plot
 */
void OS_Plot(int code,const point& p);

}; /* namespace os */
}; /* namespace rtk */

#endif
