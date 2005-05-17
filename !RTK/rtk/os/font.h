// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003-2005 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_OS_FONT
#define _RTK_OS_FONT

#include "rtk/graphics/point.h"
#include "rtk/graphics/box.h"

namespace rtk {
namespace os {

using rtk::graphics::point;
using rtk::graphics::box;

struct coord_block_paint
{
	point space_offset;
	point letter_offset;
	box rubout;
};

struct coord_block_scanstring
{
	point space_offset;
	point letter_offset;
	int split_char;
	box bbox;
};

/** Obtain handle for font.
 * @param id the font identifier (null terminated)
 * @param xsize the required font width (16 units = 1 point)
 * @param ysize the required font height (16 units = 1 point)
 * @param xres the required horizontal resolution (dots per inch,
 *  or 0 to use default, or -1 to use current)
 * @param yres the required vertical resolution (dot per inch,
 *  or 0 to use default, or -1 to use current)
 * @param _handle a buffer for the returned font handle
 * @param _xres a buffer for the returned horizontal resolution
 * @param _yres a buffer for the returned vertical resolution
 */
void Font_FindFont(const char* id,int xsize,int ysize,int xres,int yres,
	int* _handle,int* _xres,int* _yres);

/** Release handle for font.
 * @param handle the handle to be released
 */
void Font_LoseFont(int handle);

/** Read font definition (get buffer length)
 * @param handle the font handle
 * @param _buffer_size a buffer to hold the returned buffer size
 */
void Font_ReadDefn(int handle,int* _buffer_size);

/** Read font definition.
 * @param handle the font handle
 * @param _id a buffer for the returned font id
 * @param _xsize a buffer for the returned font width (16 units = 1 point)
 * @param _ysize a buffer for the returned font height (16 units = 1 point)
 * @param _xres a buffer for the returned horizontal resolution (dots per inch)
 * @param _yres a buffer for the returned vertical resolution (dots per inch)
 * @param _age a buffer for the returned age of the font
 * @param _usage_count a buffer for the returned usage count of the font
 */
void Font_ReadDefn(int handle,char* _id,int* _xsize,int* _ysize,
	int* _xres,int* _yres,int* _age,int* _usage_count);

/** Get bounding box of a character.
 * @param handle the font handle
 * @param code the character code
 * @param flags
 * @param _bbox a buffer for the returned bounding box
 */
void Font_CharBBox(int handle,int code,int flags,box* _bbox);

/** Plot string to screen using font.
 * Termination of the string is desirable whether or not flag bit 7 is set
 * because the byte after the last printable character must be readable.
 * It must be terminated if bit 7 is clear.
 * @param handle the font handle
 * @param s the string to plot (normally null terminated)
 * @param plot the plot method flags
 * @param p the point at which to begin plotting (OS coordinates or
 *  millipoints, depending on flag bit 4)
 * @param coord the coordinate block (if flag bit 5 set)
 * @param trans the transformation block (if flag bit 6 set)
 * @param length the length of the string
 */
void Font_Paint(int handle,const char* s,int plot,const point& p,void* coord,
	void* trans,int length);

/** Return information about how string would be plotted using font.
 * @param handle the font handle
 * @param s the string to plot (normally null terminated)
 * @param plot the plot method flags
 * @param p the offset to mouse click (if flag bit 17 set), else
 *  the offset to split point
 * @param coord the coordinate block (if flag bit 5 and/or 18 set)
 * @param trans the transformation block (if flag bit 6 and/or 19 set)
 * @param length the length of the string
 * @param _split a buffer for the returned character offset to caret position
 *  (if flag bit 17 set), else to split point
 * @param _p a buffer for the returned spatial offset to caret position
 *  (if flag bit 17 set), else to split point
 * @param _length the number of split characters encountered (if flag
 *  bit 20 set), else length of string
 */
void Font_ScanString(int handle,const char* s,int plot,const point& p,
	void* coord,void* trans,int length,const char** _split,point* _p,
	int* _length);

}; /* namespace os */
}; /* namespace rtk */

#endif
