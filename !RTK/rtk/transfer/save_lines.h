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
	/** An abstract base class to represent a sequence of lines.
	 * Also included is an iterator into the sequence.
	 */
	class basic_sequence
	{
	public:
		/** Get next line from sequence.
		 * The iterator is incremented by this function.
		 * @return the next line from the sequence
		 */
		virtual string operator()()=0;

		/** Test whether iterator is at end of sequence.
		 * @return true if at end, otherwise false
		 */
		virtual bool eof() const=0;

		/** Reset iterator to start of sequence. */
		virtual void reset()=0;

		/** Get estimated size of sequence in bytes. */
		virtual size_type estsize() const=0;
	};

	/** A class to represent a sequence of lines, using a given iterator type.
	 */
	template<class iterator>
	class sequence:
		public basic_sequence
	{
	private:
		/** The first member of the sequence. */
		iterator _first;

		/** The one past the last member of the sequence. */
		iterator _last;

		/** An iterator into the sequence. */
		iterator _pos;
	public:
		/** Construct sequence.
		 * @param first the first member of the sequence
		 * @param last the last member of the sequence
		 */
		sequence(iterator first,iterator last);

		virtual string operator()();
		virtual bool eof() const;
		virtual void reset();
		virtual size_type estsize() const;
	};

	/** A class to represent the null sequence. */
	class null_sequence:
		public basic_sequence
	{
	public:
		virtual string operator()();
		virtual bool eof() const;
		virtual void reset();
		virtual size_type estsize() const;
	};

	/** The sequence of lines. */
	basic_sequence* _lines;

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
	/** Set lines to be saved.
	 * The sequence is specified using a pair of iterators, which may be
	 * of any iterator type.
	 * @param first the first line to be saved
	 * @param last the last line to be saved
	 */
	template<class iterator>
	save_lines& lines(iterator first,iterator last);

	/** Clear lines to be saved. */
	save_lines& clear();

	/** Get final newline flag.
	 * @return true if the final line is terminated by a newline character,
	 *  otherwise false
	 */
	bool final_newline() const;

	/** Set final newline flag.
	 * @param true if the final line is terminated by a newline character,
	 *  otherwise false
	 */
	save_lines& final_newline(bool value);
};

template<class iterator>
save_lines::sequence<iterator>::sequence(iterator first,iterator last):
	_first(first),
	_last(last),
	_pos(last)
{}

template<class iterator>
string save_lines::sequence<iterator>::operator()()
{
	return (_pos!=_last)?*_pos++:string();
}

template<class iterator>
bool save_lines::sequence<iterator>::eof() const
{
	return _pos==_last;
}

template<class iterator>
void save_lines::sequence<iterator>::reset()
{
	_pos=_first;
}

template<class iterator>
save_lines::size_type save_lines::sequence<iterator>::estsize() const
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
	sequence<iterator>* new_lines=new sequence<iterator>(first,last);
	delete _lines;
	_lines=new_lines;
	return *this;
}

}; /* namespace transfer */
}; /* namespace rtk */

#endif
