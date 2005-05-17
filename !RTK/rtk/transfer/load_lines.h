// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2004-2005 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_TRANSFER_LOAD_LINES
#define _RTK_TRANSFER_LOAD_LINES

#include <string>

#include "rtk/transfer/load.h"

namespace rtk {
namespace transfer {

/** A class for loading lines of text.
 * Any standard container that supports push_back() and clear()
 * may be used to hold the result.
 */
class load_lines:
	public load
{
private:
	/** An abstract base class to represent a line sink. */
	class basic_sink
	{
	public:
		/** Get final line.
		 * It is an error if no lines have been appended.
		 * @return the final line
		 */
		virtual std::string back() const=0;

		/** Push line at back
		 * @param line the line to be pushed
		 */
		virtual void push_back(const std::string& line)=0;

		/** Pop line at back. */
		virtual void pop_back()=0;

		/** Erase all lines. */
		virtual void clear()=0;
	};

	/** A class to represent a container as a line sink. */
	template<class container>
	class push_back_sink:
		public basic_sink
	{
	private:
		/** The container to which lines are to be appended. */
		container& _lines;
	public:
		/** Construct push-back line sink.
		 * @param lines a standard container of std::string to which
		 *  lines are to be appended
		 */
		push_back_sink(container& lines);

		virtual std::string back() const;
		virtual void push_back(const std::string& line);
		virtual void pop_back();
		virtual void clear();
	};

	/** A class to represent the null line sink. */
	class null_sink:
		public basic_sink
	{
	public:
		virtual std::string back() const;
		virtual void push_back(const std::string& line);
		virtual void pop_back();
		virtual void clear();
	};

	/** A pointer to the current line sink.
	 * This is never null.
	 */
	basic_sink* _sink;

	/** The buffer used to receive data before it is parsed into lines. */
	char* _buffer;

	/** The size of _buffer. */
	size_type _buffer_size;

	/** The newline flag.
	 * True if the next character parsed should begin a new line,
	 * false if it should be appended to the previous line.
	 */
	bool _newline;

	/** The final newline flag.
	 * True if the final line is terminated by a newline character,
	 * otherwise false
	 */
	bool _final_newline;
public:
	/** Construct load_lines object. */
	load_lines(size_type buffer_size=0x400);

	/** Destroy load_lines object. */
	virtual ~load_lines();
protected:
	virtual void start(size_type estsize);
	virtual void get_block(void** data,size_type* size);
	virtual void put_block(size_type count);
	virtual void finish();
public:
	/** Set line destination.
	 * @param lines a standard container of std::string to which
	 *  lines are to be appended
	 */
	template<class container>
	load_lines& lines(container& lines);

	/** Clear line destination. */
	load_lines& clear();

	/** Get final newline flag.
	 * @return true if the final line is terminated by a newline character,
	 *  otherwise false
	 */
	bool final_newline() const;

	/** Set final newline flag.
	 * @param value true if the final line is terminated by a newline
	 *  character, otherwise false
	 */
	load_lines& final_newline(bool value);
};

template<class container>
load_lines::push_back_sink<container>::push_back_sink(container& lines):
	_lines(lines)
{}

template<class container>
std::string load_lines::push_back_sink<container>::back() const
{
	return _lines.back();
}

template<class container>
void load_lines::push_back_sink<container>::push_back(const std::string& line)
{
	_lines.push_back(line);
}

template<class container>
void load_lines::push_back_sink<container>::pop_back()
{
	_lines.pop_back();
}

template<class container>
void load_lines::push_back_sink<container>::clear()
{
	_lines=container();
}

template<class container>
load_lines& load_lines::lines(container& lines)
{
	push_back_sink<container>* new_sink=
		new push_back_sink<container>(lines);
	delete _sink;
	_sink=new_sink;
	return *this;
}

}; /* namespace transfer */
}; /* namespace rtk */

#endif
