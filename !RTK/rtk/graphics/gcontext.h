// This file is part of the RISC OS Toolkit (RTK).
// Copyright � 2003 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_GRAPHICS_GCONTEXT
#define _RTK_GRAPHICS_GCONTEXT

#include <string>

#include "rtk/graphics/point.h"

namespace rtk {
namespace graphics {

class font;

/** An abstract class to represent an abstract graphics context.
 * Using this class it is possible to issue graphical output instructions
 * without knowing to where the output is being directed.
 */
class gcontext
{
private:
	/** The origin of this graphics context, with respect to the screen. */
	point _origin;
	/** The current foreground colour.
	 * This is one of the 16 standard Wimp colours.
	 */
	int _fcolour;
	/** The current background colour.
	 * This is one of the 16 standard Wimp colours.
	 */
	int _bcolour;
public:
	/** Construct graphics context.
	 * @param origin the initial origin
	 */
	gcontext(const point& origin);

	/** Destroy graphics context.
	 */
	virtual ~gcontext();

	/** Get origin.
	 * @return the origin
	 */
	const point& origin() const
		{ return _origin; }

	/** Add offset to origin.
	 * @param offset the required offset
	 * @return a reference to this
	 */
	gcontext& operator+=(const point& offset)
		{ _origin+=offset; return *this; }

	/** Subtract offset from origin.
	 * @param offset the required offset
	 * @return a reference to this
	 */
	gcontext& operator-=(const point& offset)
		{ _origin-=offset; return *this; }

	/** Plot to graphics context.
	 * This method is equivalent to OS_Plot.
	 * @param code the RISC OS plot action code
	 * @param p the point to plot (with respect to the origin of this
	 *  graphics context)
	 */
	virtual void plot(int code,const point& p)=0;

	/** Draw string to graphics context using desktop font.
	 * @param s the string to be drawn
	 * @param p the point at which to begin
	 */
	virtual void draw(const string& s,const point& p)=0;

	/** Draw string to graphics context using specified font.
	 * @param f the font to be used
	 * @param s the string to be drawn
	 * @param p the point at which to begin
	 */
	virtual void draw(const font& f,const string& s,const point& p)=0;

	/** Get current foreground colour.
	 * This is one of the 16 standard Wimp colours.
	 * @return the current foreground colour
	 */
	int fcolour() const
		{ return _fcolour; }

	/** Get current background colour.
	 * This is one of the 16 standard Wimp colours.
	 * @return the current background colour
	 */
	int bcolour() const
		{ return _bcolour; }

	/** Set current foreground colour.
	 * This is one of the 16 standard Wimp colours.
	 * @param fcolour the required foreground colour
	 */
	void fcolour(int fcolour);

	/** Set current background colour.
	 * This is one of the 16 standard Wimp colours.
	 * @param bcolour the required background colour
	 */
	void bcolour(int bcolour);
protected:
	/** Notify subclass that foreground colour has changed.
	 * @param fcolour the new foreground colour
	 */
	virtual void fcolour_notify(int fcolour);

	/** Notify subclass that background colour has changed.
	 * @param bcolour the new background colour
	 */
	virtual void bcolour_notify(int bcolour);
};

}; /* namespace graphics */
}; /* namespace rtk */

#endif