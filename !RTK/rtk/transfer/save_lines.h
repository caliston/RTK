// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2004 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_TRANSFER_SAVE_LINES
#define _RTK_TRANSFER_SAVE_LINES

#include <string>

#include "rtk/transfer/save.h"

namespace rtk {
namespace transfer {

/** A class for saving lines of text.
 * The line sequence is specified as a pair of iterators, therefore this
 * class may be used to save lines from any standard container.  Note
 * that the container must not change while the save operation is in
 * progress (or at least, not in a way that might invalidate iterators
 * that point into the specified sequence.)
 */
class save_lines:
	public save
{
private:
	/** An abstract base class to represent a source of lines. */
	class basic_source
	{
	public:
		/** Get next line from source.
		 * @return the next line from the source
		 */
		virtual string operator()()=0;

		/** Test whether more lines are available.
		 * @return true if at end, otherwise false
		 */
		virtual bool eof() const=0;

		/** Reset to start of source. */
		virtual void reset()=0;

		/** Estimate total number of bytes. */
		virtual size_type estsize() const=0;
	};

	/** A class to represent a sequence as a line source. */
	template<class iterator>
	class sequence_source:
		public basic_source
	{
	private:
		/** The first member of the sequence. */
		iterator _first;

		/** The one past the last member of the sequence. */
		iterator _last;

		/** An iterator into the sequence. */
		iterator _pos;
	public:
		/** Construct sequence_source.
		 * @param first the first member of the sequence
		 * @param last the last member of the sequence
		 */
		sequence_source(iterator first,iterator last);
		virtual ~sequence_source();

		virtual string operator()();
		virtual bool eof() const;
		virtual void reset();
		virtual size_type estsize() const;
	};

	/** A class to represent the null line source. */
	class null_source:
		public basic_source
	{
	public:
		virtual string operator()();
		virtual bool eof() const;
		virtual void reset();
		virtual size_type estsize() const;
	};

	/** The sequence of lines. */
	basic_source* _source;

	/** The end-of-line flag.
	 * When true, a line of text has been saved but the associated newline
	 * character has not.
	 */
	bool _eol;

	/** The final newline flag.
	 * True if the final line is terminated by a newline character,
	 * otherwise false
	 */
	bool _final_newline;
public:
	/** Construct save_lines operation.
	 * By default, the final line is terminated by a newline character.
	 */
	save_lines();

	/** Destroy save_lines operation. */
	virtual ~save_lines();
protected:
	virtual void start();
	virtual void get_block(const void** data,size_type* count);
	virtual void finish();
	virtual size_type estsize();
public:
	/** Set line source.
	 * The source is a sequence of std::string, which may be specified
	 * using any iterator type.
	 * @param first the first line to be saved
	 * @param last the last line to be saved
	 */
	template<class iterator>
	save_lines& lines(iterator first,iterator last);

	/** Clear line source. */
	save_lines& clear();

	/** Get final newline flag.
	 * @return true if the final line is terminated by a newline character,
	 *  otherwise false
	 */
	bool final_newline() const;

	/** Set final newline flag.
	 * @param value true if the final line is terminated by a newline
	 *  character, otherwise false
	 */
	save_lines& final_newline(bool value);
};

template<class iterator>
save_lines::sequence_source<iterator>::sequence_source(
	iterator first,iterator last):
	_first(first),
	_last(last),
	_pos(last)
{}

template<class iterator>
save_lines::sequence_source<iterator>::~sequence_source()
{}

template<class iterator>
string save_lines::sequence_source<iterator>::operator()()
{
	return (_pos!=_last)?*_pos++:string();
}

template<class iterator>
bool save_lines::sequence_source<iterator>::eof() const
{
	return _pos==_last;
}

template<class iterator>
void save_lines::sequence_source<iterator>::reset()
{
	_pos=_first;
}

template<class iterator>
save_lines::size_type save_lines::sequence_source<iterator>::estsize() const
{
	size_type size=0;
	for (iterator i=_first;i!=_last;++i)
	{
		const string& s=*i;
		size+=s.length()+1;
	}
	return size;
}

template<class iterator>
save_lines& save_lines::lines(iterator first,iterator last)
{
	sequence_source<iterator>* new_source=
		new sequence_source<iterator>(first,last);
	delete _source;
	_source=new_source;
	return *this;
}

}; /* namespace transfer */
}; /* namespace rtk */

#endif
