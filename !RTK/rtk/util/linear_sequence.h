// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2006 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_UTIL_LINEAR_SEQUENCE
#define _RTK_UTIL_LINEAR_SEQUENCE

#include <algorithm>
#include <limits>
#include <iterator>

namespace rtk {
namespace util {
namespace _linear_sequence {

template<class _value_type>
class const_iterator;

template<class _value_type>
bool operator==(
	const const_iterator<_value_type>& lhs,
	const const_iterator<_value_type>& rhs);

template<class _value_type>
bool operator!=(
	const const_iterator<_value_type>& lhs,
	const const_iterator<_value_type>& rhs);

template<class _value_type>
class const_iterator
{
	friend bool operator==<>(
		const const_iterator& lhs,
		const const_iterator& rhs);
	friend bool operator!=<>(
		const const_iterator& lhs,
		const const_iterator& rhs);
public:
	typedef std::random_access_iterator_tag iterator_category;
	typedef _value_type value_type;
	typedef _value_type difference_type;
	typedef _value_type* pointer;
	typedef const _value_type* const_pointer;
	typedef _value_type& reference;
	typedef const _value_type& const_reference;
private:
	value_type _value;
	value_type _step;
	value_type _overflow;
public:
	const_iterator():
		_value(0),
		_step(1),
		_overflow(false)
		{}

	const_iterator(const_reference value,const_reference step):
		_value(value),
		_step(step),
		_overflow(false)
		{}

	const_reference operator*() const
		{ return _value; }

	const_pointer* operator->() const
		{ return &_value; }

	difference_type operator-(const const_iterator& rhs) const
		{ return _value-rhs._value; }

	const_iterator& operator++();
	const_iterator& operator--();
	const_iterator& operator+=(_value_type count);
	const_iterator& operator-=(_value_type count);

	value_type step() const
		{ return _step; }
};

template<class _value_type>
const_iterator<_value_type>& const_iterator<_value_type>::operator++()
{
	_overflow=(_value>std::numeric_limits<_value_type>::max()-_step);
	if (!_overflow) _value+=_step;
	return *this;
}

template<class _value_type>
const_iterator<_value_type>& const_iterator<_value_type>::operator--()
{
	if (!_overflow) _value-=_step;
	_overflow=false;
	return *this;
}

template<class _value_type>
const_iterator<_value_type>&
const_iterator<_value_type>::operator+=(_value_type count)
{
	if (count>0)
	{
		_value+=(count-1)*_step;
		_overflow=(_value>std::numeric_limits<_value_type>::max()-_step);
		if (!_overflow) _value+=_step;
	}
	else if (count<0)
	{
		if (_overflow) ++count;
		_overflow=false;
		_value+=count*_step;
	}
	return *this;
}

template<class _value_type>
const_iterator<_value_type>&
const_iterator<_value_type>::operator-=(_value_type count)
{
	if (count<0)
	{
		_value-=(count+1)*_step;
		_overflow=(_value>std::numeric_limits<_value_type>::max()-_step);
		if (!_overflow) _value+=_step;
	}
	else if (count>0)
	{
		if (_overflow) --count;
		_overflow=false;
		_value-=count*_step;
	}
	return *this;
}

template<class _value_type>
bool operator==(
	const const_iterator<_value_type>& lhs,
	const const_iterator<_value_type>& rhs)
{
	return lhs._value==rhs._value&&
		lhs._overflow==rhs._overflow;
}

template<class _value_type>
bool operator!=(
	const const_iterator<_value_type>& lhs,
	const const_iterator<_value_type>& rhs)
{
	return lhs._value!=rhs._value||
		lhs._overflow!=rhs._overflow;
}

}; /* namespace _linear_sequence */

/** A class to represent a linear numeric sequence.
 * The sequence behaves as if it were a read-only standard container.
 */
template<class _value_type>
class linear_sequence
{
public:
	/** The type of a member of the sequence. */
	typedef _value_type value_type;

	/** The type of a constant iterator over the sequence. */
	typedef _linear_sequence::const_iterator<_value_type> const_iterator;
private:
	/** The lowest member of the sequence. */
	value_type _min;

	/** The highest member of the sequence.
	 * The values of _min and _max are such that _max-_min is a multiple
	 * of _step.
	 */
	value_type _max;

	/** The step size.
	 * This is the difference between adjacent members of the sequence.
	 */
	value_type _step;
public:
	/** Create linear sequence.
	 * By default, a linear sequence is the sequence of all integers
	 * that can be represented by a value_type.
	 */
	linear_sequence():
		_min(std::numeric_limits<value_type>::min()),
		_max(std::numeric_limits<value_type>::max()),
		_step(1)
		{}

	/** Create linear sequence.
	 * @param min the lowest member of the sequence
	 * @param max the highest (potential) member of the sequence
	 * @param step the step size
	 * If max-min is not a multiple of step then max is adjusted downwards
	 * to make it a multiple.  By default the step size is one.
	 */
	linear_sequence(value_type min,value_type max,value_type step=1):
		_min(min),
		_max(max),
		_step(step)
	{
		// Correct _max, taking care not to cause an overflow,
		// and allowing value_type to be either signed or unsigned.
		value_type dmin=min%step;
		value_type dmax=max%step;
		if (dmin<0) dmin+=step;
		if (dmax<0) dmax+=step;
		_max-=(dmax<dmin)?step-(dmin-dmax):dmax-dmin;
	}

	/** Get an iterator to the first member of the sequence.
	 * @return an iterator to the first member of the sequence
	 */
	const_iterator begin() const
		{ return const_iterator(_min,_step); }

	/** Get an iterator to the last member of the sequence.
	 * @return an iterator to the last member of the sequence
	 */
	const_iterator end() const
		{ return ++const_iterator(_max,_step); }

	/** Get the lowest member of the sequence.
	 * @return the lowest member of the sequence
	 */
	value_type min() const
		{ return _min; }

	/** Get the highest member of the sequence.
	 * @return the highest member of the sequence
	 * This will have been adjusted to ensure that max()-min() is
	 * a multiple of step().
	 */
	value_type max() const
		{ return _max; }

	/** Get the step size.
	 * @return the step size
	 */
	value_type step() const
		{ return _step; }
};

}; /* namespace util */
}; /* namespace rtk */

namespace std {

template<class _value_type>
rtk::util::_linear_sequence::const_iterator<_value_type>
	lower_bound(
	rtk::util::_linear_sequence::const_iterator<_value_type> first,
	rtk::util::_linear_sequence::const_iterator<_value_type> last,
	const _value_type& value)
{
	// Correct value, taking care not to cause an overflow,
	// and allowing value_type to be either signed or unsigned.
	_value_type step=first.step();
	_value_type dmin=*first%step;
	_value_type dval=value%step;
	if (dmin<0) dmin+=step;
	if (dval<0) dval+=step;
	_value_type _value=value-((dval<dmin)?step-(dmin-dval):dval-dmin);
	return rtk::util::_linear_sequence::const_iterator<_value_type>(
		_value,step);
}

}; /* namespace std */

#endif
