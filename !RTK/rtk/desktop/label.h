// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_DESKTOP_LABEL
#define _RTK_DESKTOP_LABEL

#include <string>

#include "rtk/desktop/component.h"

namespace rtk {
namespace desktop {

/** A class to represent a line of text.
 * The text is displayed using the default desktop font.
 */
class label:
	public component
{
private:
	/** The label text. */
	string _text;
public:
	/** Construct empty label. */
	label();

	/** Construct label with specified text.
	 * @param text the required label text
	 */
	label(const string& text);

	/** Destroy label. */
	virtual ~label();

	virtual box min_bbox() const;
	virtual void redraw(gcontext& context,const box& clip);

	/** Get label text.
	 * @return the label text
	 */
	const string& text() const
		{ return _text; }

	/** Set label text.
	 * @param text the required label text
	 * @return a reference to this
	 */
	label& text(const string& text);
private:
	/** Get bounding box with respect to internal baselines.
	 * The internal baselines are xbaseline_text and ybaseline_text.
	 * @return the bounding box with respect to internal baselines
	 */
	box internal_bbox() const;
};

}; /* namespace desktop */
}; /* namespace rtk */

#endif
