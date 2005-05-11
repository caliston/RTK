// This file is part of the RISC OS Toolkit (RTK).
// Copyright � 2003-2004 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_DESKTOP_COMPONENT
#define _RTK_DESKTOP_COMPONENT

#include <string>

#include "rtk/graphics/point.h"
#include "rtk/graphics/box.h"

namespace rtk {
namespace graphics {

class gcontext;

}; /* namespace graphics */

namespace os {

struct sprite_area;

}; /* namespace sprite_area */

namespace events {

class mouse_click;

}; /* namespace events */

namespace desktop {

class basic_window;
class application;
class menu;

using std::string;
using rtk::graphics::point;
using rtk::graphics::box;
using rtk::graphics::gcontext;

/** An abstract class from which all RTK desktop components are derived.
 * A component can have a parent and a location with respect to that
 * parent.  It can also be reformatted and redrawn.
 *
 * Some types of component correspond directly to RISC OS windows, icons
 * or menus.  Other make themselves visible by means of the redraw
 * mechanism, or have no visible effect themselves and exist only to
 * control the position of other components.
 *
 * In order to avoid unnecessary work, component layout is postponed until
 * the end of each iteration of the main polling loop.  Layout occurs in
 * two stages: resizing (to determine the minimum amount of space that the
 * component needs) and reformatting (to fit it into the possibly greater
 * amount of space that is available).
 *
 * Not all components are visited during a given instance of the layout
 * process.  If it is apparent that nothing has changed since the
 * previous iteration then a component can be omitted by its parent.
 * A component indicates that it wishes to be visited by invalidating
 * itself.
 *
 * Coordinates are normally specified in OS units with respect to the
 * origin of the component in question.  Each component declares what
 * its origin represents with respect to a set of pre-defined baselines.
 * Components that use the same baseline are aligned with each other
 * when placed in a row or column.  It is possible to switch between
 * baselines to change how a component is aligned.
 *
 * Communication between components is performed using events.  An event
 * can originate from the Wimp, or it can be internally generated by
 * this library, or it can be application-specific.  In all cases an
 * event is associated with a particular target, which is usually the
 * lowest point in the component heirarchy which can be said to have
 * contained it.  The event is then passed upwards, and can be handled
 * by any or all of the target component and its ancestors.
 */
class component
{
public:
	/** An enumeration for specifying a vertical baseline type. */
	enum xbaseline_type
	{
		/** A vertical baseline aligned with the left of the component. */
		xbaseline_left,
		/** A vertical baseline aligned with the right of the component. */
		xbaseline_right,
		/** A vertical baseline aligned with the centre of the component. */
		xbaseline_centre,
		/** A vertical baseline for use by Latin, Greek or Cyrillic text. */
		xbaseline_text
	};
	/** An enumeration for specifying a horizontal baseline type. */
	enum ybaseline_type
	{
		/** A horizontal baseline aligned with the bottom of the component. */
		ybaseline_bottom,
		/** A horizontal baseline aligned with the top of the component. */
		ybaseline_top,
		/** A horizontal baseline aligned with the centre of the component. */
		ybaseline_centre,
		/** A horizontal baseline for use by Latin, Greek or Cyrillic text. */
		ybaseline_text
	};
	enum
	{
		/** The number of allowed vertical baseline types. */
		xbaseline_max=xbaseline_centre+1,
		/** The number of allowed horizontal baseline types. */
		ybaseline_max=ybaseline_centre+1
	};
	class xbaseline_set;
	class ybaseline_set;
private:
	/** The parent pointer. */
	component* _parent;

	/** The origin of this component, with respect to its parent. */
	point _origin;

	/** The vertical baseline of this component. */
	xbaseline_type _xbaseline:8;

	/** The horizontal baseline of this component. */
	ybaseline_type _ybaseline:8;

	/** The size-valid flag.
	 * True if resize() has been called since the last time this
	 * component or any of its descendants was invalidated, otherwise
	 * false.
	 */
	mutable bool _size_valid:1;

	/** The layout-valid flag.
	 * True if reformat() has been called since the last time this
	 * component or any of its descendants was invalidated, otherwise
	 * false.  (Must be false if _size_valid is false.)
	 */
	bool _layout_valid:1;

	/** The no-redirect flag.
	 * True if it has been determined that this object does not support
	 * event redirection, otherwise false.
	 * (This flag defaults to false.  It is changed to true if and when
	 * a dynamic cast to component::redirect fails.)
	 */
	mutable bool _no_redirect:1;

	/** The redraw-forced flag.
	 * True if a redraw has been forced for the whole of this component
	 * and no part of it has been redrawn.  
	 */
	bool _forced_redraw:1;

	/** The horizontal fit-to-parent flag.
	 * If true, and if the component is capable of doing so, the bounding
	 * box should expand horizontally to fill the space available.
	 */
	bool _xfit:1;

	/** The vertical fit-to-parent flag.
	 * If true, and if the component is capable of doing so, the bounding
	 * box should expand vertically to fill the space available.
	 */
	bool _yfit:1;

	/** The horizontal fit-to-content flag.
	 * This flag is provided for use by sizeable_component.  It is
	 * declared here so that it can be stored as part of a larger
	 * set of bitfields, thereby reducing memory usage.
	 * If true then the x-component of the minimum bounding box is
	 * calculated automatically using auto_bbox().
	 */
	bool _xauto:1;

	/** The vertical fit-to-content flag.
	 * This flag is provided for use by sizeable_component.  It is
	 * declared here so that it can be stored as part of a larger
	 * set of bitfields, thereby reducing memory usage.
	 * If true then the y-component of the minimum bounding box is
	 * calculated automatically using auto_bbox().
	 */
	bool _yauto:1;
public:
	/** Construct component.
	 * @param origin the initial origin of the component, with respect
	 *  to its parent
	 */
	component(const point& origin=point());

	/** Destroy component. */
	virtual ~component();

	/** Get parent.
	 * @return the parent of this component if it has one, otherwise 0
	 */
	component* parent() const
		{ return _parent; }

	/** Get parent after redirection.
	 * @return the parent of this component, after redirection and if it
	 *  has one, otherwise 0
	 */
	inline component* redirected_parent() const
		{ return (_no_redirect)?_parent:_redirected_parent(); }

	/** Get the window that owns the work area for this component.
	 * @return the window if there is one, otherwise 0
	 */
	basic_window* parent_work_area() const;

	/** Get the window that owns the work area for this component.
	 * Additionally, translate the point p from the coordinate space
	 * of this component to that of the work area (or the desktop if
	 * no work area is found).
	 * @param p the point to be translated
	 * @return the window if there is one, otherwise 0
	 */
	basic_window* parent_work_area(point& p) const;

	/** Get the application from which this component is descended.
	 * If this component is an application then return this.
	 * @return the application if there is one, otherwise 0
	 */
	application* parent_application();

	/** Get the application from which this component is descended.
	 * Additionally, translate the point p from the coordinate space
	 * of this component to that of the desktop.
	 * If this component is an application then return this.
	 * @param p the point to be translated
	 * @return the application if there is one, otherwise 0
	 */
	application* parent_application(point& p);

	/** Determine whether this component is a window.
	 * This function is equivalent to dynamic_cast<window*>, but probably
	 * much faster to execute.
	 * @return this if component is a window, otherwise 0
	 */
	virtual basic_window* as_window();

	/** Determine whether this component is an application.
	 * This function is equivalent to dynamic_cast<application*>, but
	 * probably much faster to execute.
	 * @return this if component is an application, otherwise 0
	 */
	virtual application* as_application();

	/** Find child at given position.
	 * If this function is not overridden then it will return 0, which
	 * is the correct behaviour for a component with no children.
	 * @param p the position at which to look, with respect to the
	 *  origin of this component
	 * @return the child at that position if there is one, otherwise 0
	 */
	virtual component* find(const point& p) const;

	/** Remove this component from its parent.
	 * If this component has no parent then no action is taken.
	 */
	void remove();

	/** Get vertical baseline.
	 * The origin of a component must lie on one of the allowed
	 * vertical baselines.  This function indicates which one.
	 * @return the vertical baseline
	 */
	xbaseline_type xbaseline() const
		{ return _xbaseline; }

	/** Get horizontal baseline.
	 * The origin of a component must lie on one of the allowed
	 * horizontal baselines.  This function indicates which one.
	 * @return the horizontal baseline
	 */
	ybaseline_type ybaseline() const
		{ return _ybaseline; }

	/** Set vertical baseline.
	 * The origin of a component can be switched from one vertical
	 * baseline to another.  Doing so may affect how it is aligned
	 * with respect to other components.
	 */
	void xbaseline(xbaseline_type xbaseline);

	/** Set horizontal baseline.
	 * The origin of a component can be switched from one horizontal
	 * baseline to another.  Doing so may affect how it is aligned
	 * with respect to other components.
	 */
	void ybaseline(ybaseline_type ybaseline);

	/** Get horizontal fit-to-parent flag.
	 * If true, and if the component is capable of doing so, the bounding
	 * box should expand horizontally to fill the space available.
	 * @return the horizontal fit-to-parent flag
	 */
	bool xfit() const
		{ return _xfit; }

	/** Get vertical fit-to-parent flag.
	 * If true, and if the component is capable of doing so, the bounding
	 * box should expand vertically to fill the space available.
	 * @return the vertical fit-to-parent flag
	 */
	bool yfit() const
		{ return _yfit; }

	/** Set horizontal fit-to-parent flag.
	 * If true, and if the component is capable of doing so, the bounding
	 * box should expand horizontally to fill the space available.
	 * @param xfit the required horizontal fit-to-parent flag
	 */
	void xfit(bool xfit);

	/** Set vertical fit-to-parent flag.
	 * If true, and if the component is capable of doing so, the bounding
	 * box should expand vertically to fill the space available.
	 * @param yfit the required vertical fit-to-parent flag
	 */
	void yfit(bool yfit);

	/** Get origin.
	 * @return the origin of this component, with respect to the origin
	 *  of its parent
	 */
	const point& origin() const
		{ return _origin; }

	/** Set origin.
	 * For this to have a useful effect the parent must allow its
	 * children to move freely.  (Most types of parent do not, and
	 * will immediately countermand an instruction to change the
	 * position of a child.)
	 * @param origin the required origin of this component, with
	 *  respect to the origin of its parent
	 * @param suppress_redraw true to prevent this function from
	 *  causing a redraw, otherwise false
	 */
	component& origin(const point& origin,bool suppress_redraw=false);

	/** Get current bounding box.
	 * If this function is not overridden then it will return the
	 * same value as min_bbox(), which is the correct behaviour if
	 * the component does not expand to fill the space available.
	 * @return the current bounding box of this component with respect
	 *  to its origin
	 */
	virtual box bbox() const;

	/** Get minimum bounding box.
	 * This is the smallest bounding box into which the component will fit.
	 * The result should be cached if the cost of recalculation is high.
	 * It must take account of the current values of xbaseline() and
	 * ybaseline().  In some cases this function will ensure that
	 * size_valid() is true, but it is not required to, and min_bbox()
	 * should not be used in place of an explicit call to resize().
	 *
	 * Where a component supports automatic line wrapping, this function
	 * gives the minimum bounding box without wrapping.
	 * @return the minimum bounding box with respect to the origin
	 */
	virtual box min_bbox() const=0;

	/** Get minimum bounding box with line wrapping.
	 * A preferred width and a preferred height are specified, however
	 * at most one of these will be relevant to any given component:
	 * the other should be disregarded.
	 *
	 * By default this function returns the same as min_bbox(), which is
	 * the correct result for a component which does not perform automatic
	 * line wrapping.
	 * @param wbox a box specifying the preferred width and/or height
	 */
	virtual box min_wrap_bbox(const box& wbox) const;

	/** Determine whether the size of this component is valid.
	 * The size is valid if resize() has been called since the last
	 * time this component or any of its descendants was invalidated.
	 * Note that the underlying instance variable inspected by this
	 * function is mutable.
	 * @return true if size valid, otherwise false
	 */
	bool size_valid() const
		{ return _size_valid; }

	/** Determine whether the layout of this component is valid.
	 * The layout is valid if reformat() has been called since the
	 * last time this component or any of its descendants was
	 * invalidated.
	 * @return true if layout valid, otherwise false
	 */
	bool layout_valid() const
		{ return _layout_valid; }

	/** Invalidate this component and its ancestors.
	 * Invalidating a component ensures that resize() and reformat()
	 * will be called before the application next calls Wimp_Poll.
	 * A component must invalidate itself if its minimum bounding
	 * box has changed.  It may invalidate itself if it would like
	 * reformat() to be called for other reasons.
	 */
	void invalidate();

	/** Resize component.
	 * This function should be called when size_valid() is false.
	 * (It is not an error to call it at other times, but there is no
	 * reason to do so.)  It must call resize() for any children for
	 * which size_valid() is false, and must set size_valid() for this
	 * component to true.  (The default implementation is to perform
	 * the latter action, which is the correct behaviour for a
	 * component with no children and no requirement to cache
	 * information about its size.)
	 */
	virtual void resize() const;

	/** Reformat component.
	 * This function should be called when the required bounding
	 * box of the component has changed or when layout_valid() is
	 * false.  (It is not an error to call it at other times, but
	 * there is no reason to do so.)  It must call reformat() for
	 * any children for which layout_valid() is false, and must set
	 * layout_valid() for this component to true, and must move the
	 * origin to the requested location.  (The default implementation
	 * is to perform the latter two actions, which is the correct
	 * behaviour for a component with no children and no requirement
	 * to create any RISC OS windows, icons or menus.)
	 *
	 * If a component has moved with respect to its parent then this
	 * function should force both its old and new bounding boxes to
	 * be redrawn.  (It is not necessary to detect whether the parent
	 * has moved, because in that case the parent will already have
	 * forced a redraw.)  The requested bounding box passed to this
	 * function should be no smaller than the minimum bounding box.
	 * @param origin the new origin of this component, with respect to
	 *  its parent
	 * @param pbbox the proposed bounding box for this component, with
	 *  respect to its own origin
	 */
	virtual void reformat(const point& origin,const box& pbbox);

	/** Unformat component.
	 * This function should be called when a component ceases to be
	 * a descendent of an application object.  It should delete
	 * (or at least hide and deregister) any RISC OS windows or
	 * icons that it has created.
	 */
	virtual void unformat();

	/** Redraw component.
	 * On entry, the origin of the graphics context must be
	 * coincident with the origin of the component.  The graphics
	 * context origin may be moved while within this function,
	 * but must be restored to its original value before returning.
	 * (This avoids the need to make copies of the graphics
	 * context, and is the reason why that argument has not been
	 * declared const.)
	 *
	 * It is not an error if the specified clip box lies partly
	 * or wholly outside the bounding box of the component,
	 * however components are expected to be selective about which
	 * children they redraw.
	 * @param context the graphics context within which the
	 *  redraw should take place
	 * @param clip the bounding box of the region to be redrawn,
	 *  with respect to the origin of this component
	 */
	virtual void redraw(gcontext& context,const box& clip);

	/** Force redraw.
	 * Instruct the Wimp to redraw the area currently occupied by
	 * this component.
	 * This function should be called both before and after any
	 * change to either the origin or the bounding box of the
	 * component.
	 * @param suppress_window true to suppress redraw if this component
	 *  is a window, otherwise false
	 */
	void force_redraw(bool suppress_window=false);

	/** Force redraw of given area.
	 * Instruct the Wimp to redraw a given area within this component.
	 * @param rbox the bounding box of the region to be redrawn,
	 *  with respect to the origin of this component
	 */
	void force_redraw(const box& clip);

	/** Set caret position.
	 * @param p the required position of the caret
	 * @param height the required height of the caret
	 * @param index the required index into string
	 * @todo the caret position is not set correctly if the component
	 *  is an icon.
	 */
	void set_caret_position(point p,int height,int index);

	/** Claim selection.
	 * If the selection was previously owned by a component within
	 * this application then a claim_entity event is sent internally
	 * to that previous owner, otherwise a Message_ClaimEntity is
	 * broadcast.
	 *
	 * Components that use this method should normally call
	 * application::deregister_selection in their unformat method.
	 */
	void claim_selection();

	/** Claim clipboard.
	 * If the clipboard was previously owned by a component within
	 * this application then a claim_entity event is sent internally
	 * to that previous owner, otherwise a Message_ClaimEntity is
	 * broadcast.
	 *
	 * Components that use this method should normally call
	 * application::deregister_clipboard in their unformat method.
	 */
	void claim_clipboard();

	/** Begin drag action within specifed bounding box.
	 * User drag types (8-11) are not currently allowed.
	 * @param dbox the box to be dragged
	 * @param bbox the bounding box
	 * @param drag_type the drag type (default=7: invisible)
	 */
	void drag(const box& dbox,const box& bbox,int drag_type=7);

	/** Begin drag action to anywhere on the desktop.
	 * User drag types (8-11) are not currently allowed.
	 * @param dbox the box to be dragged
	 * @param drag_type the drag type (default=7: invisible)
	 */                  
	void drag(const box& dbox,int drag_type=7);

	/** Begin drag action.
	 * @deprecated superceded by component::drag(const box&,int)
	 * @param dbox the box to be dragged
	 */
	void drag_box(const box& dbox);

	/** Begin drag sprite action.
	 * @param dbox the box to be dragged
	 * @param area the sprite area, or 0 for the wimp sprite area
	 * @param name the sprite name
	 */
	void drag_sprite(const box& dbox,os::sprite_area* area,
		const string& name);
protected:
	/** Link parent pointer of child to this component.
	 * This function provides restricted access to the private function
	 * set_parent().  It should be called by a container component
	 * whenever a child is added.
	 * @param c the child for which this is the new parent
	 */
	void link_child(component& c);

	/** Notify parent that child is about to be removed.
	 * This function is called by component::set_parent() immediately
	 * before _parent is changed.  It should not be called for any other
	 * reason.  It should be overridden by container components to remove
	 * the child from whatever data structure(s) that container uses to
	 * track its children.  It does not normally need to do anything to
	 * the child, because the required actions should already be in hand.
	 * The default behavior is to do nothing.
	 * @param c the child about to be removed
	 */
	virtual void remove_notify(component& c);

	/** Notify component that its baselines may have been changed.
	 * The default behaviour is to add the specified offset to the origin
	 * of the component, so that its absolute position remains fixed even
	 * though its baselines have changed.  If a component records other
	 * information (such as its bounding box) that is not automatically
	 * recalculated by reformat() then this should be modified too.  
	 * @param offset the positional offset from the old baselines to the
	 *  new baselines
	 */
	virtual void baseline_notify(const point& offset);

	bool xauto() const
		{ return _xauto; }

	bool yauto() const
		{ return _yauto; }

	void set_xauto(bool xauto)
		{ _xauto=xauto; }

	void set_yauto(bool yauto)
		{ _yauto=yauto; }

	/** Get location of internal origin with respect to external origin.
	 * This function can be used, in conjunction with external_origin(),
	 * to establish an internal origin which is permanently fixed to a
	 * given pair of baselines.
	 * @param bbox the bounding box with respect to the internal origin
	 * @param ixbaseline the internal vertical baseline
	 * @param iybaseline the internal horizontal baseline
	 * @return the internal origin with respect to the external origin
	 */
	point internal_origin(const box& bbox,xbaseline_type ixbaseline,
		ybaseline_type iybaseline) const;

	/** Get location of external origin with respect to internal origin.
	 * This function can be used, in conjunction with internal_origin(),
	 * to establish an internal origin which is permanently fixed to a
	 * given pair of baselines.
	 * @param bbox the bounding box with respect to the internal origin
	 * @param ixbaseline the internal vertical baseline
	 * @param iybaseline the internal horizontal baseline
	 * @return the external origin with respect to the internal origin
	 */
	point external_origin(const box& bbox,xbaseline_type ixbaseline,
		ybaseline_type iybaseline) const;

	box fit(const box& pbbox) const;
private:
	/** Set the parent of this component.
	 * This function is called when one component takes owenership of
	 * another.
	 * @param p the required parent
	 * @param mc true if the child is being linked as a menu or dialogue
	 *  box, otherwise false
	 * @postcondition parent()==p
	 */
	void set_parent(component* p);

	/** Get parent after redirection.
	 * This is a non-optimised version of redirected_parent(), for use
	 * by that function when _no_redirect is false.
	 */
	component* _redirected_parent() const;
};

/** A class for representing the relationship between a set of
 * vertical baselines.
 */
class component::xbaseline_set
{
private:
	/** The minimum x-coordinate with respect to xbaseline_text. */
	int _xmin;
	/** The maximum x-coordinate with respect to xbaseline_text. */
	int _xmax;
	/** The maximum width for xbaseline_left, right or centre. */
	int _xsize;
public:
	/** Construct vertical baseline set. */
	xbaseline_set();
	/** Add component to baseline set.
	 * The baselines are moved as necessary to accommodate the component.
	 * @param bbox the bounding box of the component
	 * @param xbaseline the vertical baseline of the component
	 * @return a reference to this
	 */
	xbaseline_set& add(const box& bbox,xbaseline_type xbaseline);
	/** Get offset between two baselines.
	 * @param src_xbaseline the source baseline
	 * @param dst_xbaseline the destination baseline
	 * @param xsize an optional width to be filled by the baseline set
	 * @return the offset from the source to the destination.
	 */
	int offset(xbaseline_type src_xbaseline,xbaseline_type dst_xbaseline,
		int xsize=0) const;
	/** Get minimum width.
	 * This is the minimum width required to accommodate the baseline set.
	 */
	int xsize() const;
};

/** A class for representing the relationship between a set of
 * horizontal baselines.
 */
class component::ybaseline_set
{
private:
	/** The minimum y-coordinate with respect to ybaseline_text. */
	int _ymin;
	/** The maximum y-coordinate with respect to ybaseline_text. */
	int _ymax;
	/** The maximum width for ybaseline_bottom, top or centre. */
	int _ysize;
public:
	/** Construct horizontal baseline set. */
	ybaseline_set();
	/** Add component to baseline set.
	 * The baselines are moved as necessary to accommodate the component.
	 * @param bbox the bounding box of the component
	 * @param ybaseline the horizontal baseline of the component
	 * @return a reference to this
	 */
	ybaseline_set& add(const box& bbox,ybaseline_type ybaseline);
	/** Get offset between two baselines.
	 * @param src_ybaseline the source baseline
	 * @param dst_ybaseline the destination baseline
	 * @param ysize an optional height to be filled by the baseline set
	 * @return the offset from the source to the destination.
	 */
	int offset(ybaseline_type src_ybaseline,ybaseline_type dst_ybaseline,
		int ysize=0) const;
	/** Get minimum height.
	 * This is the minimum height required to accommodate the baseline set.
	 */
	int ysize() const;
};

}; /* namespace desktop */
}; /* namespace rtk */

#endif
