// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003-2005 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_TRANSFER_BASIC_LOAD
#define _RTK_TRANSFER_BASIC_LOAD

#include <string>

#include "rtk/os/wimp.h"
#include "rtk/desktop/component.h"
#include "rtk/events/datasave.h"
#include "rtk/events/dataload.h"
#include "rtk/events/dataopen.h"
#include "rtk/events/redirection.h"

namespace rtk {
namespace transfer {

using std::string;

/** A abstract class to represent a load operation using the
 * data transfer protocol (file transfer only).
 * This class should be used when there is a requirement for the data
 * to be presented as a file (and therefore support for RAM transfer
 * would be counterproductive).  Otherwise, rtk::transfer::load should
 * be used instead.
 *
 * Implementations must override the function put_file, which defines
 * what is done with the data once it is available as a file.
 *
 * Although RAM transfers are not supported by this class, they are
 * not precluded and they may be implemented by a subclass.
 */
class basic_load:
	public desktop::component,
	public events::datasave::handler,
	public events::dataload::handler,
	public events::dataopen::handler,
	public events::redirection
{
public:
	/** A type for representing byte counts. */
	typedef unsigned int size_type;

	/** A null value for use in place of a byte count. */
	static const size_type npos=static_cast<size_type>(-1);
private:
	/** The local task handle. */
	int _thandle;

	/** The filetype of the data loaded. */
	unsigned int _filetype;
public:
	/** Construct basic_load object. */
	basic_load();

	/** Destroy basic_load object. */
	virtual ~basic_load();

	virtual graphics::box min_bbox() const;

	virtual void handle_event(events::datasave& ev);
	virtual void handle_event(events::dataload& ev);
	virtual void handle_event(events::dataopen& ev);

	/** Get filetype of data loaded
	 * @return the filetype of the data
	 */
	unsigned int filetype() const
		{ return _filetype; }

	/** Deliver Wimp event block.
	 * @internal
	 * @param wimpcode the Wimp event code
	 * @param wimpblock the Wimp event block
	 */
	void deliver_wimp_block(int wimpcode,os::wimp_block& wimpblock);

	/** Deliver Wimp message.
	 * @internal
	 * @param wimpcode the Wimp event code
	 * @param wimpblock the Wimp event block
	 */
	void deliver_message(int wimpcode,os::wimp_block& wimpblock);

	/** Get preferred temporary pathname for transfer.
	 * This is normally <Wimp$Scrap>, but an implementation class
	 * may choose to override this setting.
	 * @return the preferred temporary pathname
	 */
	virtual string temp_pathname() const;
protected:
	/** Put data from file.
	 * All data is copied from the specified pathname (which may or
	 * may not be equal to temp_pathname()).
	 * @param pathname the pathname from which the data should be copied
	 * @param estsize the estimated file size in bytes
	 */
	virtual void put_file(const string& pathname,size_type estsize)=0;
};

} /* namespace transfer */
} /* namespace rtk */

#endif
