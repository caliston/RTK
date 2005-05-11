// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003-2004 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_GRAPHICS_FONT
#define _RTK_GRAPHICS_FONT

#include <string>

namespace rtk {
namespace graphics {

class point;

using std::string;

class font
{
public:
	/** An enumation for specifying one of the default fonts. */
	enum default_font_type
	{
		/** An enumeration constant to represent the current desktop font. */
		font_desktop,
		/** An enumeration constant to represent the current symbol font. */
		font_symbol
	};
private:
	/** A reference-counted class for sharing font data. */
	struct basic_font;

	/** A subclass of basic_font to represent the current desktop font. */
	struct desktop_font;

	/** A subclass of basic_font to represent the current symbol font. */
	struct symbol_font;

	/** A subclass of basic_font to represent a font specified by ID
	 * and size. */
	struct custom_font;

	/** A pointer to the shared font data. */
	basic_font* _f;
public:
	/** Construct font object.
	 * @param id the font identifier
	 * @param xsize the required font width (16 units = 1 point)
	 * @param ysize the required font height (16 units = 1 point)
	 */
	font(const string& id,int xsize,int ysize);

	/** Construct font object, referring to a default font
	 * @param default_font a default font type (desktop or symbol)
	 */
	font(default_font_type default_font);

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
	 * @param length the length of the string to plot
	 * @return the required width (in OS units)
	 */
	int width(const char* s,unsigned int length) const;

	/** Get width required to plot string using font.
	 * @param s the string to plot
	 * @return the required width (in OS units)
	 */
	int width(const string& s) const
		{ return width(s.c_str(),s.length()); }

	/** Get font handle.
	 * @return the handle for this font
	 */
	int handle() const;
};

}; /* namespace graphics */
}; /* namespace rtk */

#endif
