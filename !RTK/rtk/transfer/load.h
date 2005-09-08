// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003-2005 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_TRANSFER_LOAD
#define _RTK_TRANSFER_LOAD

#include <string>

#include "rtk/events/ramfetch.h"
#include "rtk/events/ramtransmit.h"
#include "rtk/transfer/basic_load.h"

namespace rtk {
namespace transfer {

using std::string;

/** A abstract class to represent a load operation using the
 * data transfer protocol (file or RAM transfer).
 * Implementations must override the functions start, get, put and
 * finish to define what is done with the data.  They may additionally
 * override put_file if it is advantageous to handle the data
 * differently should it be transferred as a file.
 */
class load:
	public rtk::transfer::basic_load,
	public rtk::events::ramfetch::handler,
	public rtk::events::ramtransmit::handler
{
private:
	/** The class from which this one is derived. */
	typedef rtk::transfer::basic_load inherited;

	/** An enumeration to represent the state of the load operation. */
	enum state_type
	{
		/** The state in which the load operation is waiting for a
		 * Message_DataSave or Message_DataLoad. */
		state_idle,
		/** The state in which the load operation has sent an initial
		 * Message_RAMFetch and is waiting for either Message_RAMTransmit
		 * or for the Message_RAMFetch to be returned. */
		state_ramfetch_first,
		/** The state in which the load operation has sent an intermediate
		 * Message_RAMFetch and is waiting for a Message_RAMTransmit. */
		state_ramfetch
	};

	/** The current state of the load operation. */
	state_type _state;

	/** The RAM transfer allowed flag. */
	bool _allow_ram_transfer;

	/** The current local data buffer. */
	void* _ldata;

	/** The current local data buffer size. */
	size_type _lsize;

	/** A copy of the Message_DataSave message block.
	 * This is needed to produce the Message_DataSaveAck that is sent if
	 * the Message_RAMFetch is negatively acknowledged.
	 */
	os::wimp_block _datasave_block;
public:
	/** Construct load object.
	 * By default, RAM transfers are allowed.
	 */
	load();

	/** Destroy load object. */
	virtual ~load();

	virtual void handle_event(events::datasave& ev);
	virtual void handle_event(events::dataload& ev);
	virtual void handle_event(events::dataopen& ev);
	virtual void handle_event(events::ramfetch& ev);
	virtual void handle_event(events::ramtransmit& ev);

	/** Get RAM transfer allowed flag.
	 * @return true if RAM transfers are allowed, otherwise false
	 */
	bool allow_ram_transfer() const
		{ return _allow_ram_transfer; }

	/** Set RAM transfer allowed flag.
	 * Applications are not obliged to support RAM transfers, but nor is
	 * any extra work needed to implement them when using this class.
	 * They should be left enabled unless there is a good reason not to.
	 * @param value true if RAM transfers are allowed, otherwise false
	 * @return a reference to this
	 */
	load& allow_ram_transfer(bool value);
protected:
	/** Start new load operation.
	 * If start is called before a previous operation has been completed
	 * then the previous operation should be aborted.
	 * @param estsize the estimated file size in bytes
	 */
	virtual void start(size_type estsize)=0;

	/** Get block.
	 * The callee is free to offer any size of block (except zero),
	 * and may offer a different size each time it is called.
	 * @param data a buffer for the returned block pointer
	 * @param size a buffer for the returned block size in bytes
	 */
	virtual void get_block(void** data,size_type* size)=0;

	/** Put data from block.
	 * The caller is not required to fill the buffer supplied by
	 * get_block, but should at least partially fill it.
	 * @param count the block length in bytes
	 */
	virtual void put_block(size_type count)=0;

	/** Finish load operation. */
	virtual void finish()=0;

	/** Put data from file.
	 * All data is copied from the specified pathname.
	 * The default implementation of this method uses OS_GBPB to fetch
	 * the data from disc and write() to send it to the application.
	 * It can be overridden if a more efficient method is known,
	 * but there is no obligation to do so.
	 * @param pathname the pathname from which the data should be copied
	 * @param estsize the estimated file size in bytes
	 */
	virtual void put_file(const string& pathname,size_type estsize);
};

}; /* namespace transfer */
}; /* namespace rtk */

#endif
