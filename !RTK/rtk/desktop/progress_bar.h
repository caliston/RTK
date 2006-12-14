// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_DESKTOP_PROGRESS_BAR
#define _RTK_DESKTOP_PROGRESS_BAR

#include "rtk/desktop/sizeable_component.h"
#include "rtk/desktop/icon.h"

namespace rtk {
namespace desktop {

/** A class to represent a progress bar.
 * The length of the bar, as a fraction of the space available, is equal
 * to the numerator divided by the denominator.
 */
class progress_bar:
	public sizeable_component
{
private:
	/** The class from which this one is derived. */
	typedef sizeable_component inherited;

	/** The bounding box. */
	box _bbox;

	/** The icon used as the background and border. */
	icon _background;

	/** The icon used as the progress bar. */
	icon _bar;

	/** The current numerator. */
	unsigned int _numerator;

	/** The current denominator. */
	unsigned int _denominator;
public:
	/** Construct progress bar.
	 * By default, the numerator is zero and the denominator is one.
	 */
	progress_bar();

	/** Destroy progress bar. */
	virtual ~progress_bar();

	virtual box auto_bbox() const;
	virtual box bbox() const;
	virtual void resize() const;
	virtual void reformat(const point& origin,const box& pbbox);
	virtual void unformat();
	virtual void redraw(gcontext& context,const box& clip);

	/** Get numerator.
	 * @return the numerator
	 */
	unsigned int numerator() const
		{ return _numerator; }

	/** Get denominator.
	 * @return the denominator
	 */
	unsigned int denominator() const
		{ return _denominator; }

	/** Get foreground colour.
	 * This is one of the 16 standard Wimp colours.
	 * @return the foreground colour.
	 */
	int fcolour() const
		{ return _bar.fcolour(); }

	/** Get background colour.
	 * This is one of the 16 standard Wimp colours.
	 * @return the background colour.
	 */
	int bcolour() const
		{ return _background.bcolour(); }

	/** Set numerator.
	 * @param numerator the required numerator
	 * @return a reference to this
	 */
	progress_bar& numerator(unsigned int numerator);

	/** Set denominator.
	 * @param denominator the required denominator
	 * @return a reference to this
	 */
	progress_bar& denominator(unsigned int denominator);

	/** Set foreground colour.
	 * This is one of the 16 standard Wimp colours.
	 * @param fcolour the required foreground colour
	 * @return a reference to this
	 */
	progress_bar& fcolour(int fcolour);

	/** Set background colour.
	 * This is one of the 16 standard Wimp colours.
	 * @param bcolour the required background colour
	 * @return a reference to this
	 */
	progress_bar& bcolour(int bcolour);
};

} /* namespace desktop */
} /* namespace rtk */

#endif
