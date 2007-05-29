// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2007 Alex Waugh.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_DESKTOP_NUMBER_RANGE
#define _RTK_DESKTOP_NUMBER_RANGE

#include "rtk/desktop/sizeable_component.h"
#include "rtk/desktop/icon.h"
#include "rtk/desktop/selection_field.h"
#include "rtk/desktop/adjuster_arrow.h"
#include "rtk/util/linear_sequence.h"

namespace rtk {
namespace desktop {

/** A class to allow a selection from a number range.
 * This consists of an optional text label, the value display
 * (which may optionally be writable), up and down adjuster arrows,
 * and optionally a units label.
 * The value_type template parameter specifies the type of the value range.
 * It will typically be an int, but can be anything that a linear_sequence
 * will accept.
 */
template<class value_type>
class number_range:
	public sizeable_component
{
private:
	/** The class from which this one is derived. */
	typedef sizeable_component inherited;

	/** The bounding box. */
	box _bbox;

	/** The icon used as the text label. */
	icon _label;

	/** The down arrow. */
	adjuster_arrow _down;

	/** The up arrow. */
	adjuster_arrow _up;

	/** The icon used as the units label. */
	icon _units;

	/** The sequence that generates the numbers */
	rtk::util::linear_sequence<value_type> _seq;

	/** The type of a sequence iterator */
	typedef typename rtk::util::linear_sequence<value_type>::const_iterator seq_iterator;

	/** The value display field */
	selection_field<seq_iterator> _value;

public:
	/** Construct number range.
	 * @param min the minimum value of the sequence
	 * @param max the maximum value of the sequence
	 * @param step the step size of the sequence
	*/
	number_range(value_type min,value_type max,value_type step=1);

	/** Destroy number range. */
	virtual ~number_range()
		{ remove(); }

	virtual box auto_bbox() const;
	virtual box bbox() const;
	virtual void resize() const;
	virtual void reformat(const point& origin,const box& pbbox);
	virtual void unformat();
	virtual void redraw(gcontext& context,const box& clip);

	/** Get the currently selected value.
	 * @return the current value
	 */
	value_type value() const
		{ return _value.value(); }

	/** Get label icon text.
	 * @return the label icon text
	 */
	string label() const
		{ return _label.text(); }

	/** Get units icon text.
	 * @return the units icon text
	 */
	string units() const
		{ return _units.text(); }

	/** Get the minimum value of the range.
	 * @return the minimum value
	 */
	value_type min() const
		{ return _seq.min(); }

	/** Get the maximum value of the range.
	 * @return the maximum value
	 */
	value_type max() const
		{ return _seq.max(); }

	/** Get the step value of the range.
	 * @return the step value
	 */
	value_type step() const
		{ return _seq.step(); }

	/** Get writable flag.
	 * @return true if the field content is directly editable by the user,
	 *  otherwise false
	 */
	bool writable() const
		{ return _value.writable(); }

	/** Set the current value.
	 * @param value the value to set
	 * @return a reference to this
	 */
	number_range& value(value_type value)
		{ _value.value(value); return *this; }

	/** Set label icon text.
	 * @param text the required label text
	 * @param capacity the required capacity (defaults to length of
	 *  specified icon text)
	 * @return a reference to this
	 */
	number_range& label(const string& text,icon::size_type capacity=0)
		{ _label.text(text,capacity); return *this; }

	/** Set units icon text.
	 * @param text the required units text
	 * @param capacity the required capacity (defaults to length of
	 *  specified icon text)
	 * @return a reference to this
	 */
	number_range& units(const string& text,icon::size_type capacity=0)
		{ _units.text(text,capacity); return *this; }

	/** Snap to an allowed value.
	 * This function ensures that the displayed field value is a member
	 * of the sequence of allowed values.
	 * @return a reference to this
	 */
	number_range& snap();

	/** Set writable flag.
	 * @param writable true if the field is required to be directly
	 *  writable by the user, otherwise false
	 * @return a reference to this
	 */
	number_range& writable(bool writable)
		{ _value.writable(writable); return *this; }

};


template<class value_type> number_range<value_type>::number_range(value_type min,value_type max,value_type step):
	_down(adjuster_arrow::arrow_down),
	_up(adjuster_arrow::arrow_up),
	_seq(min,max,step),
	_value(_seq.begin(),_seq.end())
{
	_down.redirect(&_value);
	_up.redirect(&_value);

	_label.xbaseline(xbaseline_left);
	_value.xbaseline(xbaseline_left);
	_value.ybaseline(ybaseline_centre);
	_down.xbaseline(xbaseline_left);
	_up.xbaseline(xbaseline_left);
	_units.xbaseline(xbaseline_left);

	_value.writable(false);

	link_child(_label);
	link_child(_units);
	link_child(_up);
	link_child(_down);
	link_child(_value);
}

template<class value_type> box number_range<value_type>::auto_bbox() const
{
	box lbbox=_label.min_bbox();
	box vbbox=_value.min_bbox();
	box dbbox=_down.min_bbox();
	box ubbox=_up.min_bbox();
	box nbbox=_units.min_bbox();

	ybaseline_set ybs;
	ybs.add(lbbox,_label.ybaseline());
	ybs.add(vbbox,_value.ybaseline());
	ybs.add(dbbox,_down.ybaseline());
	ybs.add(ubbox,_up.ybaseline());
	ybs.add(nbbox,_units.ybaseline());

	box abbox;
	abbox.ymax(ybs.ysize());
	// The width should be enough to include all components, plus a gap
	// of 8 units between the value and the adjuster arrows
	abbox.xmax(lbbox.xsize()+vbbox.xsize()+8+ubbox.xsize()+dbbox.xsize()+nbbox.xsize());

	// Translate to external origin and return.
	abbox-=external_origin(abbox,xbaseline_left,ybaseline_bottom);
	return abbox;
}

template<class value_type> box number_range<value_type>::bbox() const
{
	return _bbox;
}

template<class value_type> void number_range<value_type>::resize() const
{
	_label.resize();
	_value.resize();
	_down.resize();
	_up.resize();
	_units.resize();

	inherited::resize();
}

template<class value_type> void number_range<value_type>::reformat(const point& origin,const box& pbbox)
{
	// Fit bounding box to parent.
	box bbox=fit(pbbox);

	// Update origin and bounding box of this component, force redraw
	// if necessary.  (This must happen before reformat() is called for
	// the children.)
	bool moved=(origin!=this->origin())||(bbox!=this->bbox());
	if (moved) force_redraw(true);
	_bbox=bbox;
	inherited::reformat(origin,bbox);
	if (moved) force_redraw(true);

	box lbbox=_label.min_bbox();
	box vbbox=_value.min_bbox();
	box dbbox=_down.min_bbox();
	box ubbox=_up.min_bbox();
	box nbbox=_units.min_bbox();

	// The label, down, up and units icons should remain minimum size
	// The value icon should fill all remaining space
	vbbox.xmax(vbbox.xmin()+bbox.xsize()-(lbbox.xsize()+dbbox.xsize()+ubbox.xsize()+nbbox.xsize()));

	int offset=bbox.xmin();
	_label.reformat(point(offset,0),lbbox);
	offset+=lbbox.xsize();
	_value.reformat(point(offset,0),vbbox);
	offset+=vbbox.xsize()+8;
	_down.reformat(point(offset,0),dbbox);
	offset+=dbbox.xsize();
	_up.reformat(point(offset,0),ubbox);
	offset+=ubbox.xsize();
	_units.reformat(point(offset,0),nbbox);
}

template<class value_type> void number_range<value_type>::unformat()
{
	_label.unformat();
	_value.unformat();
	_down.unformat();
	_up.unformat();
	_units.unformat();
	inherited::unformat();
}

template<class value_type> void number_range<value_type>::redraw(gcontext& context,const box& clip)
{
	_label.redraw(context,clip);
	_value.redraw(context,clip);
	_down.redraw(context,clip);
	_up.redraw(context,clip);
	_units.redraw(context,clip);
	inherited::redraw(context,clip);
}

template<class value_type> number_range<value_type>& number_range<value_type>::snap()
{
	if (_value.value() > max()) _value.value(max());
	if (_value.value() < min()) _value.value(min());
	_value.snap();
	return *this;
}

} /* namespace desktop */
} /* namespace rtk */


#endif
