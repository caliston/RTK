// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2004-2005 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_DESKTOP_TEXT_AREA
#define _RTK_DESKTOP_TEXT_AREA

#include <string>

#if defined(__GNUC__) && (__GNUC__<3)
#include <rope>
#define __gnu_cxx
#else
#include <ext/rope>
#endif

#include "rtk/util/cumulative_sum.h"

#include "rtk/os/font.h"

#include "rtk/graphics/font.h"

#include "rtk/desktop/component.h"
#include "rtk/desktop/icon.h"

#include "rtk/events/null_reason.h"
#include "rtk/events/mouse_click.h"
#include "rtk/events/key_pressed.h"
#include "rtk/events/user_drag_box.h"
#include "rtk/events/datasave.h"
#include "rtk/events/dataload.h"
#include "rtk/events/claim_entity.h"
#include "rtk/events/datarequest.h"
#include "rtk/events/loaded.h"

#include "rtk/transfer/save_lines.h"
#include "rtk/transfer/load_lines.h"

namespace rtk {
namespace desktop {

using std::string;

/** A class to represent a multi-line text field.
 * A text area will often be embedded within a dedicated subwindow
 * in order to provide features such as a background or a vertical
 * scroll bar.  Whether or not this is the case, the containing
 * window must have a work area button type of 10.
 */
class text_area:
	public component,
	public events::null_reason::handler,
	public events::mouse_click::handler,
	public events::key_pressed::handler,
	public events::user_drag_box::handler,
	public events::datasave::handler,
	public events::dataload::handler,
	public events::claim_entity::handler,
	public events::datarequest::handler,
	public events::loaded::handler
{
private:
	/** The class from which this one is derived. */
	typedef component inherited;
public:
	/** An enumeration type to represent the selection model to be used. */
	enum selection_model_type
	{
		/** The traditional selection model.
		 * In this model the caret and selection may be set independently.
		 */
		model_traditional,
		/** The standard selection model.
		 * In this model the caret and selection are mutually exclusive.
		 * This is the behaviour specified by the style guide.
		 */
		model_standard
	};

	/** An enumeration type to represent the line-wrapping method. */
	enum wrap_method_type
	{
		/** Do not perform line wrapping. */
		wrap_none,
		/** Wrap lines, breaking at a character boundary. */
		wrap_char,
		/** Wrap lines, breaking at a word boundary. */
		wrap_word
	};

	/** A type to represent a sequence of paragraphs.
	 * The only guarantee made about this type is that it is a
	 * container of std::string which functions begin() const
	 * and end() const which behave in the customary manner.
	 * At present it is implemented as a std::rope<std::string>,
	 * but this is subject to change without notice.
	 */
	typedef __gnu_cxx::rope<string> text_type;

	/** A class to represent a location within the text. */
	class basic_mark
	{
	protected:
		/** The text to which the mark refers. */
		const text_type* _text;

		/** The paragraph number to which the mark refers. */
		unsigned int _para;

		/** The byte index into the paragraph to which the mark refers. */
		unsigned int _index_para;
	public:
		/** Construct basic mark.
		 * @param text the text to which the mark refers
		 * @param para the paragraph number to which the mark refers
		 * @param index_para the byte index into the paragraph
		 *  to which the mark refers
		 */
		basic_mark(const text_type& text,unsigned int para,
			unsigned int index_para);

		char operator*() const;

		/** Get text.
		 * @return the text to which the mark refers.
		 */
		const text_type& text() const
			{ return *_text; }

		/** Get paragraph number.
		 * @return the paragraph number to which the mark refers
		 */
		unsigned int para() const
			{ return _para; }

		/** Get byte index into paragraph.
		 * @return the byte index into the paragrah to which
		 *  the mark refers
		 */
		unsigned int index_para() const
			{ return _index_para; }
	};

	/** A class for iterating through the text. */
	class mark:
		public basic_mark
	{
	private:
		using basic_mark::_text;
		using basic_mark::_para;
		using basic_mark::_index_para;
	public:
		/** Construct mark.
		 * @param text the text to which the mark refers
		 * @param para the paragraph number to which the mark refers
		 * @param index_para the byte index into the paragraph
		 *  to which the mark refers
		 */
		mark(const text_type& text,unsigned int para,
			unsigned int index_para);

		mark operator++();
		mark operator--();

		/** Get mark at start of next word.
		 * @return a mark at the start of the next word
		 */
		mark next_word() const;

		/** Get mark at start of previous word.
		 * @return a mark at the start of the previous word
		 */
		mark prev_word() const;
	};
	friend class mark;

	/** A class to represent a location within the text and
	 * its position within the work area.
	 */
	class fixed_mark:
		public basic_mark
	{
	private:
		using basic_mark::_text;
		using basic_mark::_para;
		using basic_mark::_index_para;

		/** The line number to which the mark refers. */
		unsigned int _line;

		/** The byte index into the line to which the mark refers. */
		unsigned int _index_line;

		/** The position of the mark within the text area.
		 * This is given with respect to the top left-hand corner
		 * of the bounding box. */
		point _position;
	public:
		/** Construct fixed mark.
		 * @param area the text area to which the mark refers
		 * @param mk a basic mark which specifies the required
		 *  position within the text
		 */
		fixed_mark(const text_area& area,const basic_mark& mk);

		/** Get line number.
		 * @return the line number to which the mark refers
		 */
		unsigned int line() const
			{ return _line; }

		/** Get byte index into line.
		 * @return the byte index into the line to which the mark refers
		 */
		unsigned int index_line() const
			{ return _index_line; }

		/** Get position of mark within text area.
		 * This is given with respect to the top left-hand corner
		 * of the bounding box.
		 * @return the position of the mark within the text area
		 */
		const point& position() const
			{ return _position; }
	};
	friend class fixed_mark;
private:
	/** The text broken into paragraphs. */
	__gnu_cxx::rope<string> _text;

	/** The accumulated line counts for each paragraph. */
	mutable util::cumulative_sum<unsigned int> _lines;

	/** The font in which the text is displayed. */
	graphics::font _font;

	/** The foreground colour.
	 * This is one of the 16 standard Wimp colours.
	 */
	int _fcolour;

	/** The background colour.
	 * This is one of the 16 standard Wimp colours.
	 */
	int _bcolour;

	/** The colour for control characters.
	 * This is one of the 16 standard Wimp colours.
	 */
	int _ccolour;

	/** The line height. */
	int _line_height;

	/** The baseline offset. */
	int _baseline_offset;

	/** True if _line_height is varied automatically to match _font,
	 * otherwise false. */
	bool _auto_line_height;

	/** True if _baseline_offset is varied automatically to match _font,
	 * otherwise false. */
	bool _auto_baseline_offset;

	/** The line wrapping method. */
	wrap_method_type _wrap_method;

	/** The bounding box of this component. */
	box _bbox;

	/** The cached non-line-wrapped minimum bounding box of this component.
	 * This is valid iff _min_bbox_valid&&size_valid() is true.
	 */
	mutable box _min_bbox;

	/** A cached line-wrapped minimum bounding box of this component.
	 * This is valid iff _min_wrap_bbox_valid&&size_valid() is true.
	 * Note that there will be many different but valid values it can
	 * take, corresponding to different possible widths.  The width
	 * at any given moment will be the one most recently requested
	 * via min_wrap_bbox(const box&), which is not necessarily equal
	 * to the width of bbox().
	 */
	mutable box _min_wrap_bbox;

	/** The valid flag for _min_bbox.
	 * True if _min_bbox is valid, otherwise false.  This is a necessary
	 * but not a sufficient condition: size_valid() must be true too.
	 */
	mutable bool _min_bbox_valid;

	/** The valid flag for _min_wrap_bbox.
	 * True if _min_wrap_bbox is valid, otherwise false.  This is a necessary
	 * but not a sufficient condition: size_valid() must be true too.
	 */
	mutable bool _min_wrap_bbox_valid;

	/** The preferred x-coordinate of the caret.
	 * This is the horizontal position that the caret should attempt
	 * to move to if it is moved up or down using the cursor keys.
	 * In most cases it will not be able to achieve exactly that
	 * position because it does not correspond to a character boundary,
	 * in which case it will move to the nearest allowed position.
	 */
	int _caret_pref_x;

	/** The read-only flag.
	 * True if the text is read-only, otherwise false.
	 */
	bool _read_only;

	/** The selection model. */
	selection_model_type _selection_model;

	/** True if this component currently owns the caret or selection. */
	bool _has_focus;

	/** The start of the caret.
	 * Under the traditional selection model, _caret_first==_caret_last.
	 * Under the standard model, _caret_first==_select_first.
	 */
	mark _caret_first;

	/** The end of the caret.
	 * Under the traditional selection model, _caret_last==_caret_first.
	 * Under the standard model, _caret_last==_select_last.
	 */
	mark _caret_last;

	/** The start of the current selection.
	 * Under the standard selection model, _select_first==_caret_first.
	 */
	mark _select_first;

	/** The end of the current selection.
	 * Under the standard selection model, _select_last==_caret_last.
	 */
	mark _select_last;

	/** True if a drag is in progress, otherwise false. */
	bool _dragging;

	/** The fixed end of the selection during a drag. */
	mark _dragref;

	/** The save operation used for clipboard transfers. */
	transfer::save_lines _saveop;

	/** The load operation used for clipboard transfers. */
	transfer::load_lines _loadop;

	/** The outbound clipboard. */
	__gnu_cxx::rope<string> _oclipboard;

	/** The inbound clipboard. */
	__gnu_cxx::rope<string> _iclipboard;

	/** A font coordinate block.
	 * This is needed to specify the split character. */
	mutable os::coord_block_scanstring _font_coord_block;
public:
	/** Construct text area.
	 * By default:
	 * - the text is black;
	 * - the text assumes a white background;
	 * - control characters are red;
	 * - line wrap occurs at word boundaries;
	 * - the text is read-write;
	 * - the standard selection model is used.
	 */
	text_area();

	/** Destroy text area. */
	virtual ~text_area();

	virtual box bbox() const;
	virtual box min_bbox() const;
	virtual box min_wrap_bbox(const box& wbox) const;
	virtual void resize() const;
	virtual void reformat(const point& origin,const box& pbbox);
	virtual void unformat();
	virtual void redraw(gcontext& context,const box& clip);

	virtual void handle_event(events::null_reason& ev);
	virtual void handle_event(events::mouse_click& ev);
	virtual void handle_event(events::key_pressed& ev);
	virtual void handle_event(events::user_drag_box& ev);
	virtual void handle_event(events::datasave& ev);
	virtual void handle_event(events::dataload& ev);
	virtual void handle_event(events::claim_entity& ev);
	virtual void handle_event(events::datarequest& ev);
	virtual void handle_event(events::loaded& ev);

	/** Get text.
	 * The return value is guaranteed to be a const instance of
	 * of text_type.  That in turn is guaranteed to behave in
	 * certain respects like a standard container.  Within these
	 * bounds, the behaviour of the return value may change without
	 * notice.  It is guaranteed to remain valid until but not
	 * beyond the next non-const call to the text_area instance
	 * from which it was obtained.
	 * @return the text, as a text_type
	 */
	text_type text() const;

	/** Get selection.
	 * The return value is guaranteed to be a const instance of
	 * of text_type.  That in turn is guaranteed to behave in
	 * certain respects like a standard container.  Within these
	 * bounds, the behaviour of the return value may change without
	 * notice.  It is guaranteed to remain valid until but not
	 * beyond the next non-const call to the text_area instance
	 * from which it was obtained.
	 * @return the selection, as a text_type
	 */
	text_type selection() const;

	/** Set text.
	 * @param text the replacement text
	 * @return a reference to this
	 */
	text_area& text(const text_type& text);

	/** Set selection.
	 * @param selection the replacement text
	 * @return a reference to this
	 */
	text_area& selection(const text_type& selection);

	/** Test whether there is a non-empty selection.
	 * @return true if there is a non-empty selection, otherwise false
	 */
	bool has_selection() const;

	/** Get mark for start of text.
	 * @return a mark referring to the start of the text
	 */
	mark begin() const;

	/** Get mark for end of text.
	 * @return a mark referring to the end of the text
	 */
	mark end() const;

	/** Move caret one character to the left. */
	void handle_left_char();

	/** Move caret one character to the right. */
	void handle_right_char();

	/** Move caret one word to the left. */
	void handle_left_word();

	/** Move caret one word to the right. */
	void handle_right_word();

	/** Move caret downwards.
	 * If diff is negative then the caret moves upwards.
	 * @param diff the number of lines by which to move
	 */
	void handle_down_lines(int diff=1);

	/** Move caret upwards.
	 * If diff is negative then the caret moves downwards.
	 * @param diff the number of lines by which to move
	 */
	void handle_up_lines(int diff=1)
		{ handle_down_lines(-diff); }

	/** Move caret to start of line. */
	void handle_start_of_line();

	/** Move caret to end of line. */
	void handle_end_of_line();

	/** Move caret to start of text. */
	void handle_start_of_text();

	/** Move caret to end of text. */
	void handle_end_of_text();

	/** Insert character before caret.
	 * @param code the character to be inserted
	 */
	void handle_insert(char code);

	/** Insert linefeed before caret. */
	void handle_linefeed();

	/** Delete selection, or character to left of caret. */
	void handle_delete_left();

	/** Delete selection, or character to right of caret. */
	void handle_delete_right();

	/** Copy selection to clipboard. */
	void handle_copy();

	/** Cut selection to clipboard. */
	void handle_cut();

	/** Paste selection to clipboard. */
	void handle_paste();

	/** Get font.
	 * @return the font
	 */
	const graphics::font& font() const
		{ return _font; }

	/** Get foreground colour.
	 * This is one of the 16 standard Wimp colours.
	 * @return the foreground colour
	 */
	int fcolour() const
		{ return _fcolour; }

	/** Get background colour.
	 * This is one of the 16 standard Wimp colours.
	 * @return the background colour
	 */
	int bcolour() const
		{ return _bcolour; }

	/** Get control character colour.
	 * This is one of the 16 standard Wimp colours.
	 * @return the control character colour
	 */
	int ccolour() const
		{ return _ccolour; }

	/** Get line height.
	 * @return the line height
	 */
	int line_height() const
		{ return _line_height; }

	/** Get baseline offset.
	 * @return the baseline offset
	 */
	int baseline_offset() const
		{ return _baseline_offset; }

	/** Get line wrap method.
	 * @return the line wrap method
	 */
	wrap_method_type wrap_method() const
		{ return _wrap_method; }

	/** Get read-only flag.
	 * @return true if the text area is read-only, otherwise false
	 */
	bool read_only() const
		{ return _read_only; }

	/** Get selection model.
	 * @return the selection model
	 */
	selection_model_type selection_model() const
		{ return _selection_model; }

	/** Set font.
	 * @param font the required font
	 */
	text_area& font(const graphics::font& font);

	/** Set foreground colour.
	 * @param fcolour the required foreground colour
	 */
	text_area& fcolour(int fcolour);

	/** Set background colour.
	 * Note that the background of a text area is not filled.  To obtain
	 * a particular background, it must be embedded within a window.
	 * However, this attribute should be set to the required background
	 * colour in order to ensure correct anti-aliasing.
	 * @param bcolour the required background colour
	 */
	text_area& bcolour(int bcolour);

	/** Set control character colour.
	 * @param ccolour the required control character colour
	 */
	text_area& ccolour(int ccolour);

	/** Set line height.
	 * This is the distance between adjacent lines.
	 * @param line_height the required line height
	 */
	text_area& line_height(int line_height);

	/** Set baseline offset.
	 * This determines the height of the horizontal baseline.
	 * It is normally equal to about a quarter of the line height.
	 * @param baseline_offset the required baseline offset
	 */
	text_area& baseline_offset(int baseline_offset);

	/** Set line wrap method.
	 * @param wrap_method the required line wrap method
	 * @return a reference to this
	 */
	text_area& wrap_method(wrap_method_type wrap_method);

	/** Set read-only flag.
	 * @param read_only true to make the text area read-only, otherwise false
	 */
	text_area& read_only(bool read_only);

	/** Set selection model.
	 * @param selection_model the required selection model
	 * @return a reference to this
	 */
	text_area& selection_model(selection_model_type selection_model);
private:
	/** Automatically calculate line height for font.
	 * This function does not itself alter _line_height.
	 * @return the calculated line height
	 */
	int auto_line_height() const;

	/** Automatically calculate baseline offset for font.
	 * This function does not itself alter _baseline_offset.
	 * It depends on _line_height having been set correctly.
	 * @return the calculated baseline offset.
	 */
	int auto_baseline_offset() const;

	/** Render line of text.
	 * Note that the paragraph text is logically but not physically const.
	 * (It is temporarily modified, then restored to its original value,
	 * in order to pass string fragments to RISC OS without copying.)
	 * @param context the graphics context
	 * @param text the paragraph
	 * @param index the starting index into the paragraph
	 * @param count the number of characters to paint
	 * @param p the point at which to start
	 */
	void render_line(gcontext& context,const string& text,unsigned int index,
		unsigned int count,const point& p) const;

	/** Get width of line of text.
	 * Note that the paragraph text is logically but not physically const.
	 * (It is temporarily modified, then restored to its original value,
	 * in order to pass string fragments to RISC OS without copying.)
	 * @param text the paragraph
	 * @param index the starting index into the paragraph
	 * @param count the number of characters to include
	 * @return the width of the line
	 */
	int line_width(const string& text,unsigned int index,unsigned int count)
		const;

	/** Find character corresponding to given caret x-coordinate.
	 * Note that the paragraph text is logically but not physically const.
	 * (It is temporarily modified, then restored to its original value,
	 * in order to pass string fragments to RISC OS without copying.)
	 * @param text the paragraph
	 * @param index the starting index into the paragraph
	 * @param x the required x coordinate
	 * @return the number of characters past the starting index
	 *  at which the caret should be placed
	 */
	unsigned int find_index(const string& text,unsigned int index,int x) const;

	/** Find next split point for paragraph.
	 * Note that the paragraph text is logically but not physically const.
	 * (It is temporarily modified, then restored to its original value,
	 * in order to pass string fragments to RISC OS without copying.)
	 * @param text the paragraph
	 * @param index the starting index into the paragraph
	 * @param width the width at which to split
	 * @param include_trailing include trailing spaces in result
	 * @return the number of characters past the starting index
	 *  at which to split
	 */
	unsigned int split_line(const string& text,unsigned int index,int width,
		bool include_trailing=true) const;

	/** Find mark by snapping a point to character grid.
	 * @param p the point to snap
	 * @return a mark corresponding to the point
	 */
	mark snap(const point& p) const;

	/** Find mark by snapping an x-position to the character grid
	 * on a given line.
	 * @param line the required line index
	 * @param x the required x-position
	 * @return a mark corresponding to line and x
	 */
	mark snap(unsigned int line,int x) const;

	/** Reflow text, using existing content of window.
	 * This function updates the content of _lines.
	 * @param old_width the previous width of the text area
	 * @param new_width the new width of the text area
	 */
	void reflow(int old_width,int new_width);

	/** Reflow text, without using existing content of window.
	 * This function updates the content of _lines.
	 * @param width the new width of the text area
	 */
	void reflow(int width);

	/** Load text into clipboard.
	 * @param first the start of the region to load
	 * @param last the end of the region to load
	 */
	void load_clipboard(mark first,mark last);

	/** Show caret.
	 * @param mk the required position of the caret
	 * @param set_pref_x true to update _pref_x to match the new caret
	 *  position, otherwise false
	 * @param follow true to issue an auto_scroll event so as to follow
	 *  the caret, otherwise false
	 */
	void show_caret(const mark& mk,bool set_pref_x,bool follow);

	/** Hide the caret.
	 * If the caret is owned by this component then it is made invisible
	 * (otherwise no action is taken).
	 */
	void hide_caret();

	/** Show selection.
	 * The marks are automatically swapped into the correct order if
	 * necessary.
	 * @param first the required start of the selection
	 * @param last the required end of the selection
	 */
	void show_selection(mark first,mark last);

	/** Force redraw between two marks.
	 * @param first the start of the region to be redrawn
	 * @param last the end of the region to be redrawn
	 */
	void force_redraw_between(mark first,mark last);
public:
	/** Replace text.
	 * @param first the start of the region to be replaced
	 * @param last the end of the region to be replaced
	 * @param new_para a single replacement paragraph
	 */
	void replace(const mark& first,const mark& last,const string& new_para);

	/** Replace text.
	 * @param first the start of the region to be replaced
	 * @param last the end of the region to be replaced
	 * @param new_text the replacement paragraphs (at least one)
	 */
	void replace(mark first,mark last,const __gnu_cxx::rope<string>& new_text);
private:
	/** Adjust layout during call to replace().
	 * This function adjusts the content of _lines, and invalidates
	 * or copies any affected parts of the work area.  It does not
	 * alter _text, nor does it change the position of any marks.
	 * It must be called before the text itself is adjusted.
	 * @param first the start of the region to be replaced
	 * @param last the end of the region to be replaced
	 * @param new_text the replacement paragraphs (at least one)
	 */
	void adjust_layout(const mark& first,const mark& last,
		const __gnu_cxx::rope<string>& new_text);

	/** Adjust text during call to replace().
	 * This function only alters the content of _text.  It does not
	 * adjust the layout, nor does it change the position of any marks.
	 * @param first the start of the region to be replaced
	 * @param last the end of the region to be replaced
	 * @param new_text the replacement paragraphs (at least one)
	 */
	void adjust_text(const mark& first,const mark& last,
		__gnu_cxx::rope<string> new_text);

	/** Adjust mark during call to replace().
	 * This function must not be called until after the layout and
	 * the text have been adjusted.
	 * @param mk the mark to be adjusted
	 * @param first the start of the region to be replaced
	 * @param last the end of the region to be replaced
	 * @param new_text the replacement paragraphs (at least one)
	 * @return the adjusted mark
	 */
	mark adjust_mark(mark mk,const mark& first,const mark& last,
		const __gnu_cxx::rope<string>& new_text) const;
};

inline bool operator==(const text_area::basic_mark& lhs,
	const text_area::basic_mark& rhs)
{
	// If paragraph numbers differ then compare paragraph numbers,
	// otherwise compare character indices.
	if (lhs.index_para()!=rhs.index_para()) return false;
	return lhs.index_para()==rhs.index_para();
}

inline bool operator!=(const text_area::basic_mark& lhs,
	const text_area::basic_mark& rhs)
{
	// If paragraph numbers differ then compare paragraph numbers,
	// otherwise compare character indices.
	if (lhs.para()!=rhs.para()) return true;
	return lhs.index_para()!=rhs.index_para();
}

inline bool operator<(const text_area::basic_mark& lhs,
	const text_area::basic_mark& rhs)
{
	// If paragraph numbers differ then compare paragraph numbers,
	// otherwise compare character indices.
	if (lhs.para()!=rhs.para()) return lhs.para()<rhs.para();
	return lhs.index_para()<rhs.index_para();
}

inline bool operator>=(const text_area::basic_mark& lhs,
	const text_area::basic_mark& rhs)
{
	// If paragraph numbers differ then compare paragraph numbers,
	// otherwise compare character indices.
	if (lhs.para()!=rhs.para()) return lhs.para()>rhs.para();
	return lhs.index_para()>=rhs.index_para();
}

int operator-(const text_area::basic_mark& lhs,
	const text_area::basic_mark& rhs);

}; /* namespace desktop */
}; /* namespace rtk */

#endif
