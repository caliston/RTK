// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2005 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#include "rtk/os/os.h"
#include "rtk/os/pdriver.h"

#include "rtk/graphics/point.h"
#include "rtk/graphics/box.h"
#include "rtk/graphics/print_gcontext.h"
#include "rtk/graphics/print_job.h"

#include "rtk/desktop/component.h"

namespace rtk {
namespace graphics {

namespace {

/** A class for selecting a print job.
 * The print job remains selected while an instance of this class
 * is in scope.  Once that instance is destroyed, the previously
 * selected print job is restored.
 */
class select_print_job
{
private:
	/** The file handle for the previously selected print job. */
	int _phandle;
public:
	/** Select print job.
	 * The title should be specified only on the first occasion that
	 * a given print job is selected, and even then it is optional.
	 * @param handle the file handle for the print job to be selected
	 * @param title the optional title of the print job
	 */
	explicit select_print_job(int handle,const char* title=0);

	/** Deselect print job. */
	~select_print_job();
};

select_print_job::select_print_job(int handle,const char* title):
	_phandle(0)
{
	rtk::os::PDriver_SelectJob(handle,title,&_phandle);
}

select_print_job::~select_print_job()
{
	rtk::os::PDriver_SelectJob(_phandle,0,0);
}

/** Open file to which printout will be sent.
 * @param pathname the pathname to be opened
 * @return the file handle
 */
int open_print_file(const string& pathname)
{
	int handle=0;
	rtk::os::OS_Find(0x8c,pathname.c_str(),0,&handle);
	return handle;
}

}; /* anonymous namespace */

class print_job::rectangle
{
private:
	/** The content of the rectangle. */
	rtk::desktop::component* _content;
public:
	/** Construct rectangle.
	 * @param c the component which provides the content of the rectangle
	 */
	explicit rectangle(rtk::desktop::component& c);
	/** Redraw rectangle
	 * @param context the graphics context in which to draw
	 * @param clip the required clipping box
	 */
	void redraw(gcontext& context,const box& clip);
};

print_job::print_job(string jobname,string pathname):
	_handle(open_print_file(pathname))
{
	select_print_job pj(_handle,jobname.c_str());
}

print_job::~print_job()
{
	rtk::os::PDriver_EndJob(_handle);
}

void print_job::add(rtk::desktop::component& c,const point& p,
	const linear_transformation& t,int bcolour)
{
	select_print_job pj(_handle);
	_rectangles.push_back(rectangle(c));
	int id=_rectangles.size()-1;
	rtk::os::PDriver_GiveRectangle(id,c.bbox(),t,p,bcolour);
}

void print_job::print_page()
{
	select_print_job pj(_handle);
	box clip;
	int more=0;
	unsigned int id=0;
	rtk::os::PDriver_DrawPage(1,&clip,0,0,&more,&id);
	while (more)
	{
		print_gcontext context(point(0,0),_handle);
		if (id<_rectangles.size())
		{
			_rectangles[id].redraw(context,clip);
		}
		rtk::os::PDriver_GetRectangle(&clip,&more,&id);
	}
	_rectangles.clear();
}

print_job::rectangle::rectangle(rtk::desktop::component& c):
	_content(&c)
{}

void print_job::rectangle::redraw(gcontext& context,const box& clip)
{
	_content->redraw(context,clip);
}

}; /* namespace graphics */
}; /* namespace rtk */
