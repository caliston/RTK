// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_OS_COLOURTRANS
#define _RTK_OS_COLOURTRANS

namespace rtk {
namespace os {

/** Set colours for use by font manager.
 * @param handle the font handle, or 0 for the current font
 * @param bcolour the background palette entry
 * @param fcolour the foreground palette entry
 * @param max the maximum foreground colour offset (0-14)
 * @param _bcolour a buffer for the returned background logical colour
 * @param _fcolour a buffer for the returned foreground logical colour
 * @param _max a buffer for the returned maximum sensible colour offset
 */
void ColourTrans_SetFontColours(int handle,int bcolour,int fcolour,int max,
	int* _bcolour,int* _fcolour,int* _max);

}; /* namespace os */
}; /* namespace rtk */

#endif
