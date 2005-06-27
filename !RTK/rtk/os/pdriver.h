// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2005 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_OS_PDRIVER
#define _RTK_OS_PDRIVER

#include "rtk/graphics/point.h"
#include "rtk/graphics/box.h"

namespace rtk {
namespace os {

using rtk::graphics::point;
using rtk::graphics::box;

/** Get information about printer driver.
 * @param _type a buffer for the version number (bits 0-15) and
 *  driver number (bits 16-31)
 * @param _xres a buffer for the horizontal resolution (dpi)
 * @param _yres a buffer for the vertical resolution (dpi)
 * @param _features a buffer for the features word
 * @param _name a buffer for a pointer to the printer name
 * @param _xhres a buffer for the horizontal halftone resolution (dpi)
 * @param _yhres a buffer for the vertical halftone resolution (dpi)
 * @param _pnum a buffer for the printer number
 */
void PDriver_Info(int* _type,int* _xres,int* _yres,int* _features,
	const char** _name,int* _xhres,int* _yhres,int* _pnum);

/** Check printer features.
 * @param mask the bits to be checked
 * @param features the desired features
 */
void PDriver_CheckFeatures(int mask,int features);

/** Get paper size.
 * The origin of the page is the bottom left-hand corner.
 * @param _size a buffer for the size of the paper, in millipoints
 * @param _bbox a buffer for the printable area, in millipoints
 */
void PDriver_PageSize(point* _size,box* _bbox);

/** Select current print job.
 * @param handle the file handle of the print job to be selected
 * @param title the title of the print job, or 0 if none
 * @param _handle a buffer for the file handle of the previously
 *  selected print job
 */
void PDriver_SelectJob(int handle,const char* title,int* _handle);

/** Get file handle of current print job.
 * @param _handle a buffer for the file handle of the currently
 *  selected print job
 */
void PDriver_CurrentJob(int* _handle);

/** End current print job.
 * @param handle the file handle of the print job to be ended
 */
void PDriver_EndJob(int handle);

/** End current print job.
 * @param handle the file handle of the print job to be ended
 */
void PDriver_AbortJob(int handle);

/** Specify a rectangle to be printed.
 * @param id the rectangle identification word
 * @param bbox the rectangle to be printed (in OS units)
 * @param trans the transformation table
 * @param pos the position of the bottom left-hand corner (millipoints)
 * @param bcolour the background colour (palette entry)
 */
void PDriver_GiveRectangle(unsigned int id,const box& bbox,const int* trans,
	const point& pos,int bcolour);

/** Get first rectangle to print.
 * @param copies the number of copies to print
 * @param clip a buffer for the returned rectangle to be drawn
 * @param page_num the page sequence number, or 0 if unspecified
 * @param page_str the page number string, or 0 if unspecified
 * @param _more a buffer for the returned control flag (true if more to do,
 *  otherwise false)
 * @param _id a buffer for the returned rectangle identification word
 */
void PDriver_DrawPage(int copies,box* clip,int page_num,
	const char* page_str,int* _more,unsigned int* _id);

/** Get subsequent rectangle to print.
 * @param clip a buffer for the returned rectangle to be drawn
 * @param _more a buffer for the returned control flag (true if more to do,
 *  otherwise false)
 * @param _id a buffer for the returned rectangle identification word
 */
void PDriver_GetRectangle(box* clip,int* _more,unsigned int* _id);

}; /* namespace os */
}; /* namespace rtk */

#endif
