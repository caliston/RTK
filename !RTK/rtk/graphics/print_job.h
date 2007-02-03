// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2005 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_GRAPHICS_PRINT_JOB
#define _RTK_GRAPHICS_PRINT_JOB

#include <vector>

#include "rtk/graphics/linear_transformation.h"
#include "rtk/util/auto_filehandle.h"

namespace rtk {
namespace desktop {

class component;

} /* namespace desktop */

namespace graphics {

class box;
class gcontext;

/** A class to represent a print job. */
class print_job
{
private:
	/** A class to represent a rectangle to be printed. */
	class rectangle;

	/** The file handle for this print job. */
	rtk::util::auto_filehandle _handle;

	/** A list of rectangles to be printed on the current page.
	 * The rectangles are indexed by ID.
	 */
	std::vector<rectangle> _rectangles;
public:
	/** Construct print job.
	 * @param jobname the optional name of the print job, defaulting
	 *  to the empty string which indicates that the job is anonymous
	 * @param pathname the pathname to which the printout is to be
	 *  written, defaulting to "printer:" if no pathname is given
	 */
	explicit print_job(string jobname="",string pathname="printer:");

	/** Destroy print job. */
	virtual ~print_job();

	/** Add component to list for next page.
	 * @param c the component to be added
	 * @param p the required position of that component (in millipoints)
	 * @param t an optional transformation matrix (dimensionless)
	 * @param bcolour the required background colour (0xBBGGRR00)
	 */
	void add(rtk::desktop::component& c,const point& p,
		const linear_transformation& t=linear_transformation(),
		int bcolour=0xffffff00);

	/** Print page.
	 * The components to be printed should already have been specified
	 * by calling the add function one or more times.
	 */
	void print_page();
};

} /* namespace graphics */
} /* namespace rtk */

#endif
