// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_GRAPHICS_FONT
#define _RTK_GRAPHICS_FONT

#include <string>

namespace rtk {
namespace graphics {

class point;

class font
{
private:
	/** A reference-counted structure for sharing font data. */
	struct _font;
	/** A pointer to the shared font data. */
	_font* _f;
public:
	/** Construct font object.
	 * @param id the font identifier
	 * @param xsize the required font width (16 units = 1 point)
	 * @param ysize the required font height (16 units = 1 point)
	 */
	font(const string& id,int xsize,int ysize);

	/** Construct copy of font object.
	 * Font objects may be freely copied.  The underlying font handle
	 * is reference-counted and shared.
	 * @param f the font to be copied
	 */
	font(const font& f);

	/** Destroy font object.
	 */
	~font();

	/** Assign font object.
	 * Font objects may be freely copied.  The underlying font handle
	 * is reference-counted and shared.
	 * @param f the font to be copied
	 */
	font& operator=(const font& f);

	/** Plot string to screen using font.
	 * @param s the string to plot
	 * @param p the point at which to begin plotting (in OS units)
	 */
	void paint(const string& s,const point& p) const;

	/** Get width required to plot string using font.
	 * @param s the string to plot
	 * @return the required width (in OS units)
	 */
	int width(const string& s) const;
};

}; /* namespace graphics */
}; /* namespace rtk */

#endif
