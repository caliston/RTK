// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003-2005 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_TRANSFER_SAVE
#define _RTK_TRANSFER_SAVE

#include <string>

#include "rtk/desktop/component.h"
#include "rtk/events/datasaveack.h"
#include "rtk/events/dataloadack.h"
#include "rtk/events/ramfetch.h"
#include "rtk/events/save_to_app.h"
#include "rtk/events/save_to_file.h"
#include "rtk/events/datarequest.h"
#include "rtk/events/redirection.h"

namespace rtk {
namespace os {

union wimp_block;

}; /* namespace os */

namespace transfer {

using std::string;

/** An abstract class to represent a save operation using the
 * data transfer protocol.
 * Implementations must override start(), get(), finish() and estsize()
 * to define the source of the data.
 */
class save:
	public desktop::component,
	public events::datasaveack::handler,
	public events::dataloadack::handler,
	public events::ramfetch::handler,
	public events::save_to_app::handler,
	public events::save_to_file::handler,
	public events::datarequest::handler,
	public events::redirection
{
public:
	/** A type for representing byte counts. */
	typedef unsigned int size_type;

	/** A null value for use in place of a byte count. */
	static const size_type npos=static_cast<size_type>(-1);
private:
	/** An enumeration to represent the current state of the
	 * save operation. */
	enum state_type
	{
		/** The state in which the save operation is waiting for an RTK
		 * save_to_file or save_to_app event. */
		state_idle,
		/** The state in which the save operation has sent a Message_DataSave
		 * and is waiting for either Message_RAMFetch or Message_DataSaveAck.
		 */
		state_datasave,
		/** The state in which the save operation has sent a Message_DataLoad
		 * and is waiting for a Message_DataLoadAck. */
		state_dataload,
		/** The state in which the save operation has sent an initial or
		 * intermediate Message_RAMTransmit and is waiting for a
		 * Message_RAMFetch. */
		state_ramtransmit
	};

	/** The current state of the save operation. */
	state_type _state;

	/** The local task handle. */
	int _thandle;

	/** The filetype of the data to be saved. */
	unsigned int _filetype;

	/** The RAM transfer allowed flag. */
	bool _allow_ram_transfer;

	/** The pathname to which the data was most recently saved. */
	string _pathname;

	/** The data secure flag. */
	bool _secure;

	/** The current local data buffer. */
	const void* _ldata;

	/** The current local data buffer size. */
	size_type _lsize;
public:
	/** Construct save object. */
	save();

	/** Destroy save object. */
	virtual ~save();

	virtual graphics::box min_bbox() const;

	virtual void handle_event(events::datasaveack& ev);
	virtual void handle_event(events::dataloadack& ev);
	virtual void handle_event(events::ramfetch& ev);
	virtual void handle_event(events::save_to_app& ev);
	virtual void handle_event(events::save_to_file& ev);
	virtual void handle_event(events::datarequest& ev);

	/** Get filetype.
	 * @return the filetype of the data to be saved
	 */
	unsigned int filetype() const
		{ return _filetype; }

	/** Get RAM transfer allowed flag.
	 * @return true if RAM transfers are allowed, otherwise false.
	 */
	bool allow_ram_transfer() const
		{ return _allow_ram_transfer; }

	/** Get pathname.
	 * This is set to the empty string if the data has not been saved,
	 * or if the most recent save operation used the RAM transfer
	 * protocol.
	 * @return the pathname to which the data was most recently saved
	 */
	const string& pathname() const
		{ return _pathname; }

	/** Get data secure flag.
	 * The date is secure if it has been written to a parmanent file
	 * (as opposed to a temporary file such as <Wimp$Scrap>).
	 * @return the data secure flag
	 */
	bool secure() const
		{ return _secure; }

	/** Set filetype.
	 * @param filetype the filetype of the data to be saved.
	 */
	save& filetype(unsigned int filetype);

	/** Set RAM transfer allowed flag.
	 * Applications are not obliged to support RAM transfers, but nor is
	 * any extra work needed to implement them when using this class.
	 * They should be enabled unless there is a good reason not to. 
	 * @param value true if RAM transfers are allowed, otherwise false.
	 * @return a reference to this
	 */
	save& allow_ram_transfer(bool value);

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
protected:
	/** Start new save operation.
	 * If start() is called before a previous operation has been completed
	 * then the previous operation should be aborted.
	 */
	virtual void start()=0;

	/** Get data as memory block.
	 * A memory block is returned containing some or all of the remaining
	 * data.  To obtain all of the data, repeat until the returned count
	 * is zero.  
	 * @param data the buffer for the returned block pointer
	 * @param count a buffer for the returned block length in bytes
	 */
	virtual void get_block(const void** data,size_type* count)=0;

	/** Finish save operation. */
	virtual void finish()=0;

	/** Get estimated size.
	 * @return the estimated size of the data
	 */
	virtual size_type estsize()=0;

	/** Get data as file.
	 * All data is copied to the specified pathname.
	 * The default implementation of this method uses read() to
	 * fetch the data and OS_GBPB to write it to disc.
	 * It can be overridden if a more efficient method is known,
	 * but there is no obligation to do so.
	 * @param pathname the pathname to which the data should be copied
	 */
	virtual void get_file(const string& pathname);
};

}; /* namespace transfer */
}; /* namespace rtk */

#endif
