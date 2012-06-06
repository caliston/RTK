// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2004-2005 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_UTIL_CUMULATIVE_SUM
#define _RTK_UTIL_CUMULATIVE_SUM

#include <stdexcept>
#include <vector>

namespace rtk {
namespace util {

/** A class for quickly calculating cumulative sums.
 * Elements may be indexed using operator[].  It is also possible to
 * obtain the cumulative sum up to a given index.  Both of these
 * operations execute in O(log n) time.
 */
template<class value_type>
class cumulative_sum
{
public:
	/** The type of the index of an element. */
	typedef unsigned int index_type;
private:
	/** A vector of partial sums.
	 * If a given index, written in binary, ends in m trailing ones,
	 * then it is a sum of 2**m elements ending at the given index.
	 */
	std::vector<value_type> _values;

	class reference_type;
public:
	/** Construct cumulative sum object. */
	cumulative_sum();

	/** Index into cumulative sum.
	 * Note that the result is a helper object, as opposed to a
	 * true reference.
	 */
	reference_type operator[](index_type index);

	/** Get cumulative sum up to a given index.
	 * A std::out_of_range exception is thrown if index>size().
	 * @param index the index
	 * @return the cumulative sum up to but excluding that index
	 */
	value_type sum(index_type index) const;

	/** Erase elements.
	 * @param first the first element to be erased
	 * @param last the last element plus one to be erased
	 */
	void erase(unsigned int first,unsigned int last);

	/** Insert elements.
	 * @param pos the index at which to insert
	 * @param count the number of elements to insert
	 * @param value the value of the elements to insert
	 */
	void insert(unsigned int pos,unsigned int count,value_type value);

	/** Find the index after which a given cumulative sum is exceeded.
	 * @param value the cumulative sum
	 * @return the index at which that cumulative sum is reached
	 */
	index_type find(value_type value) const;

	/** Get number of elements.
	 * @return the number of elements
	 */
	index_type size() const
		{ return _values.size(); }

	/** Set number of elements.
	 * @param size the required number of elements
	 */
	void resize(index_type size);

	/** Get capacity.
	 * @return the capacity
	 */
	index_type capacity() const
		{ return _values.capacity(); }

	/** Set capacity.
	 * @param capacity the required capacity
	 */
	void reserve(index_type capacity)
		{ _values.reserve(capacity); }
};

/** A helper class for use by cumulative_sum<>::operator[]. */
template<class value_type>
class cumulative_sum<value_type>::reference_type
{
private:
	/** A reference to the vector of partial sums. */
	std::vector<value_type>& _values;

	/** The index of the element to which this object refers. */
	index_type _index;
public:
	/** Create helper object.
	 * @param values the vector of partial sums
	 * @param idnex the index of the element to which this object refers
	 */
	reference_type(std::vector<value_type>& values,index_type index);

	/** Set value of element.
	 * @param value the required value of the element
	 */
	reference_type& operator=(const value_type& value);

	/** Get value of element.
	 * @return the value of the element
	 */
	operator value_type() const;
};

template<class value_type>
cumulative_sum<value_type>::cumulative_sum()
{}

template<class value_type>
class cumulative_sum<value_type>::reference_type
cumulative_sum<value_type>::operator[](index_type index)
{
	if (index>=_values.size())
	{
		throw std::out_of_range(
			"index out of range in rtk::util::cumulative_sum");
	}
	return reference_type(_values,index);
}

template<class value_type>
value_type cumulative_sum<value_type>::sum(index_type index) const
{
	if (index>_values.size()) throw std::out_of_range(
		"index out of range in rtk::util::cumulative_sum");
	value_type value=0;
	index_type i=index;
	while (i)
	{
		value+=_values[i-1];
		i&=i-1;
	}
	return value;
}

template<class value_type>
void cumulative_sum<value_type>::erase(unsigned int first,unsigned int last)
{
	if ((first>last)||(last>_values.size())) throw std::out_of_range(
		"index out of range in rtk::util::cumulative_sum");

	for (unsigned int i=first,j=last;j!=_values.size();++i,++j)
	{
		(*this)[i]=value_type((*this)[j]);
	}

	_values.resize(_values.size()-(last-first));
}

template<class value_type>
void cumulative_sum<value_type>::insert(unsigned int pos,unsigned int count,
	value_type value)
{
	_values.resize(_values.size()+count,0);

	for (unsigned int i=_values.size(),j=_values.size()-count;j!=pos;--i,--j)
	{
		(*this)[i-1]=value_type((*this)[j-1]);
	}

	for (unsigned int i=pos+count;i!=pos;--i)
	{
		(*this)[i-1]=value;
	}
}

template<class value_type>
typename cumulative_sum<value_type>::index_type
cumulative_sum<value_type>::find(value_type value) const
{
	index_type index=0;

	index_type i=1;
	while (i<=_values.size()) i<<=1;
	i>>=1;

	while (i)
	{
		unsigned int j=index+i-1;
		if ((j<_values.size())&&(_values[j]<=value))
		{
			value-=_values[j];
			index+=i;
		}
		i>>=1;
	}

	return index;
}

template<class value_type>
void cumulative_sum<value_type>::resize(index_type size)
{
	index_type old_size=size;
	_values.resize(size,0);

	for (unsigned int i=old_size;i!=size;++i)
	{
		(*this)[i]=0;
	}
}

template<class value_type>
cumulative_sum<value_type>::reference_type::reference_type(
	std::vector<value_type>& values,index_type index):
	_values(values),
	_index(index)
{}

template<class value_type>
class cumulative_sum<value_type>::reference_type&
cumulative_sum<value_type>::reference_type::operator=(const value_type& value)
{
	value_type diff=value;

	index_type i=_index+1;
	while (i)
	{
		diff-=_values[i-1];
		i&=i-1;
	}

	index_type j=_index;
	while (j)
	{
		diff+=_values[j-1];
		j&=j-1;
	}

	index_type k=_index;
	while (k<_values.size())
	{
		_values[k]+=diff;
		k|=k+1;
	}

	return *this;
}

template<class value_type>
cumulative_sum<value_type>::reference_type::operator value_type() const
{
	value_type value=0;

	index_type i=_index+1;
	while (i)
	{
		value+=_values[i-1];
		i&=i-1;
	}

	index_type j=_index;
	while (j)
	{
		value-=_values[j-1];
		j&=j-1;
	}

	return value;
}

} /* namespace util */
} /* namespace rtk */

#endif
