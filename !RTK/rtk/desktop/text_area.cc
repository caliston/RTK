// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2004-2005 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#include <memory>
#include <algorithm>

#include "rtk/swi/os.h"
#include "rtk/swi/wimp.h"

#include "rtk/os/os.h"
#include "rtk/os/font.h"
#include "rtk/os/wimp.h"

#include "rtk/graphics/gcontext.h"

#include "rtk/desktop/basic_window.h"
#include "rtk/desktop/application.h"
#include "rtk/desktop/text_area.h"

#include "rtk/events/auto_scroll.h"

namespace rtk {
namespace desktop {

using std::min;
using std::max;

namespace {

/** The character at which words are split. */
char split_char=' ';

/** The flag bit which must be set to make the caret invisible. */
const int caret_invisible=1<<25;

/** The hexadecimal digits. */
static const char* hex="0123456789ABCDEF";

/** A class to format a control character for display.
 * Control character are displayed as "[xx]" where xx is the character
 * code in upper-case hexadecimal.
 */
class control_string
{
private:
	/** The control character as a string formatted for display. */
	char cs[5];
public:
	/** Construct control character string. */
	control_string(char code);

	/** Get control character as a string formatted for display. */
	operator const char*() const
		{ return cs; }
};

control_string::control_string(char code)
{
	cs[0]='[';
	cs[1]=hex[(code>>4)&0x0f];
	cs[2]=hex[(code>>0)&0x0f];
	cs[3]=']';
	cs[4]=0;
}

}; /* anonymous namespace */

int operator-(const text_area::basic_mark& lhs,
	const text_area::basic_mark& rhs)
{
	// Calculate difference assuming that the marks
	// are in the same paragraph.
	int diff=lhs.index_para()-lhs.index_para();

	// Correct for the lengths of any intervening paragraphs.
	unsigned int para=lhs.para();
	while (para<rhs.para())
	{
		diff+=lhs.text()[para++].size()+1;
	}
	while (para>rhs.para())
	{
		diff-=lhs.text()[--para].size()+1;
	}
	return diff;
}

text_area::basic_mark::basic_mark(const text_type& text,unsigned int para,
	unsigned int index_para):
	_text(&text),
	_para(para),
	_index_para(index_para)
{}

char text_area::basic_mark::operator*() const
{
	const string ptext=(*_text)[_para];
	return (_index_para<ptext.size())?ptext[_index_para]:'\n';
}

text_area::mark::mark(const text_type& text,unsigned int para,
	unsigned int index_para):
	basic_mark(text,para,index_para)
{}

text_area::mark text_area::mark::operator++()
{
	if (_index_para<(*_text)[_para].size())
	{
		++_index_para;
	}
	else if (_para<_text->size()-1)
	{
		++_para;
		_index_para=0;
	}
	return *this;
}

text_area::mark text_area::mark::operator--()
{
	if (_index_para)
	{
		--_index_para;
	}
	else if (_para)
	{
		--_para;
		_index_para=(*_text)[_para].size();
	}
	return *this;
}

text_area::mark text_area::mark::next_word() const
{
	unsigned int para=_para;
	unsigned int index_para=_index_para;
	string ptext=(*_text)[para];

	// Search forwards for a space character in this paragraph.
	// If the end of the paragraph is reached then stop there,
	// because that counts as a space character.
	{
		const string cptext=ptext;
		while ((index_para!=ptext.size())&&!isspace(cptext[index_para]))
			++index_para;
	}

	// Search forwards for a non-space character in this paragraph.
	{
		const string cptext=ptext;
		while ((index_para!=ptext.size())&&isspace(cptext[index_para]))
			++index_para;
	}

	// If the end of the paragraph is reached without finding a
	// non-space character then search subsequent paragraphs.
	while ((para+1<_text->size())&&(index_para==ptext.size()))
	{
		++para;
		ptext=(*_text)[para];
		index_para=0;
		const string cptext=ptext;
		while ((index_para!=ptext.size())&&isspace(cptext[index_para]))
			++index_para;
	}

	return mark(*_text,para,index_para);
}

text_area::mark text_area::mark::prev_word() const
{
	unsigned int para=_para;
	unsigned int index_para=_index_para;

	// Search backwards for a non-space character in this paragraph.
	string ptext=(*_text)[para];
	{
		const string cptext=ptext;
		while (index_para&&isspace(cptext[index_para-1]))
			--index_para;
	}

	// If the start of the paragraph is reached without finding a
	// non-space character then search previous paragraphs.
	while (para&&!index_para)
	{
		--para;
		ptext=(*_text)[para];
		index_para=ptext.size();
		const string cptext=ptext;
		while (index_para&&isspace(cptext[index_para-1]))
			--index_para;
	}

	// Search backwards for a space character in this paragraph.
	// If the start of the paragraph is reached then stop there,
	// because that counts as a space character.
	{
		const string cptext=ptext;
		while (index_para&&!isspace(cptext[index_para-1]))
			--index_para;
	}

	return mark(*_text,para,index_para);
}

text_area::fixed_mark::fixed_mark(const text_area& area,const basic_mark& mk):
	basic_mark(mk)
{
	int width=area.bbox().xsize();
	const string ptext=(*_text)[_para];

	// Split paragraph into line, searching for index.
	// Record line number and index into line when found.
	_line=area._lines.sum(_para);
	_index_line=_index_para;
	bool found=false;
	while (!found)
	{
		unsigned int offset=_index_para-_index_line;
		unsigned int count=area.split_line(ptext,offset,width);
		found=(_index_line<count)||
			((_index_line==count)&&(offset+count==ptext.size()));
		if (!found)
		{
			++_line;
			_index_line-=count;
		}
	}

	// Calculate coordinates of mark.
	int x=area.line_width(ptext,_index_para-_index_line,_index_line);
	int y=-(_line+1)*area.line_height();
	_position=point(x,y);
}

text_area::text_area():
	_font(graphics::font::font_desktop),
	_fcolour(7),
	_bcolour(0),
	_ccolour(11),
	_line_height(auto_line_height()),
	_baseline_offset(auto_baseline_offset()),
	_auto_line_height(true),
	_auto_baseline_offset(true),
	_wrap_method(wrap_word),
	_min_bbox_valid(false),
	_min_wrap_bbox_valid(false),
	_caret_pref_x(0),
	_read_only(false),
	_selection_model(model_standard),
	_has_focus(false),
	_caret_first(_text,0,0),
	_caret_last(_text,0,0),
	_select_first(_text,0,0),
	_select_last(_text,0,0),
	_dragging(false),
	_dragref(_text,0,0)
{
	// There must always be at least one paragraph.
	_text.push_back(string());

	// Initialise coordinate block, for the font manager.
	// This is used to specify the split character.
	// (The other fields do not need to be initialised.)
	_font_coord_block.split_char=split_char;

	// Initialise _lines.
	// (The specified width is arbitrary:
	// what matters is that _lines is non-empty.)
	reflow(1024);
}

text_area::~text_area()
{}

box text_area::bbox() const
{
	return _bbox;
}

box text_area::min_bbox() const
{
	// Determine how many OS units are in a pixel (horizontally).
	int xeigfactor=0;
	os::OS_ReadModeVariable(swi::XEigFactor,&xeigfactor);
	unsigned int xpix=1<<xeigfactor;

	// Update _min_bbox_valid.
	if (!size_valid()) resize();

	if (!_min_bbox_valid)
	{
		// Calculate width and height, without line wrap.
		int xsize=0;
		for (unsigned int i=0;i!=_text.size();++i)
		{
			const string text=_text[i];
			xsize=max(xsize,line_width(text,0,text.length()));
		}
		int ysize=_text.size()*line_height();

		// Construct minimum bounding box, with respect to top
		// left-hand corner of text area.
		// (Add one pixel to width, to ensure that there is
		// enough space for line-breaking to work correctly.)
		box abbox(0,-ysize,xsize+xpix,0);

		// Translate to external origin and return.
		abbox-=external_origin(abbox,xbaseline_left,ybaseline_top);
		_min_bbox=abbox;
		_min_bbox_valid=true;
	}

	return _min_bbox;
}

box text_area::min_wrap_bbox(const box& wbox) const
{
	// If line wrap is disabled then wbox is irrelevant
	// and the result is the same as min_bbox(). 
	if (_wrap_method==wrap_none) return min_bbox();

	// Update _min_wrap_bbox_valid.
	if (!size_valid()) resize();

	// If the cached minimum bounding box is valid and matches the
	// requested width then no further action is needed, otherwise
	// the result must be calculated.
	if ((!_min_wrap_bbox_valid)||(_min_wrap_bbox.xsize()!=wbox.xsize()))
	{
		// Calculate width and height, with line wrap.
		int xsize=wbox.xsize();
		int ysize=0;

		for (unsigned int i=0;i!=_text.size();++i)
		{
			// Count number of lines in paragraph.
			unsigned int lines=1;
			const string text=_text[i];
			unsigned int j=split_line(text,0,xsize);
			while (j<text.size())
			{
				j+=split_line(text,j,xsize);
				++lines;
			}

			// Add multiple of line height to bounding box.
			ysize+=lines*line_height();
		}

		// Construct minimum bounding box, with respect to top left-hand
		// corner of text area.
		box abbox(0,-ysize,xsize,0);

		// Translate to external origin and return.
		abbox-=external_origin(abbox,xbaseline_left,ybaseline_top);
		_min_wrap_bbox=abbox;
		_min_wrap_bbox_valid=true;
	}

	return _min_wrap_bbox;
}

void text_area::resize() const
{
	if (!size_valid())
	{
		// Invalidate the cached minimum bounding boxes
		// (to be recalculated if and when they are needed).
		_min_bbox_valid=false;
		_min_wrap_bbox_valid=false;
		inherited::resize();
	}
}

void text_area::reformat(const point& origin,const box& pbbox)
{
	// Fit bounding box to parent.
	box bbox=fit(pbbox);

	// Keep record of old and new width of component, for reflow.
	int old_width=_bbox.xsize();
	int new_width=bbox.xsize();

	// Update origin and bounding box of this component, force redraw
	// or reflow if necessary.
	bool moved=(origin!=this->origin());
	if (moved) force_redraw(true);
	_bbox=bbox;
	inherited::reformat(origin,bbox);
	if (moved) force_redraw(true);
	else if (new_width!=old_width) reflow(old_width,new_width);
}

void text_area::unformat()
{
	// If this component is registered to receive particular types of
	// event then it must be deregistered before it is removed from
	// the component hierarchy (while the application object is still
	// accessible), to ensure that it cannot be destroyed while
	// registered.
	if (application* app=parent_application())
	{
		app->deregister_null(*this);
		app->deregister_selection(*this);
		app->deregister_clipboard(*this);
	}
}

void text_area::redraw(gcontext& context,const box& clip)
{
	// Set background colour.
	context.bcolour(bcolour());

	// Get clip box with respect to top left-hand corner of text area.
	box lclip=clip-bbox().xminymax();

	// Identify first and 1+last line numbers to be displayed.
	unsigned int lmin=max((-lclip.ymax())/line_height(),0);
	unsigned int lmax=max((-lclip.ymin()-1)/line_height()+1,0);

	// Identify first and 1+last paragraph numbers to be displayed.
	unsigned int pmin=_lines.find(lmin);
	unsigned int pmax=_lines.find(lmax?lmax-1:0)+1;
	if (pmax>_lines.size()) pmax=_lines.size();

	// Redraw paragraphs within clip box.
	// Loop over paragraphs.
	int width=bbox().xsize();
	for (unsigned int i=pmin;i!=pmax;++i)
	{
		// Extract paragraph text, determine line number.
		const string text=_text[i];
		unsigned int line=_lines.sum(i);

		// Split paragraph into lines.
		unsigned int j=0;
		while (j<text.size())
		{
			// Calculate coordinates for top and bottom of line.
			int ymin=bbox().ymax()-(line+1)*line_height();
			int ymax=ymin+line_height();

			// Decide where to split line.
			unsigned int k=split_line(text,j,width);

			// Render line.
			point p(bbox().xmin(),ymin+baseline_offset());
			render_line(context,text,j,k,p);

			// Move forwards in paragraph
			// and down one line on screen.
			j+=k;
			++line;
		}
	}

	fixed_mark select_first_pos(*this,_select_first);
	fixed_mark select_last_pos(*this,_select_last);

	// Redraw selection.
	if ((select_first_pos.line()<lmax)&&(select_last_pos.line()>=lmin))
	{
		// Determine how many OS units are in a pixel.
		int xeigfactor=0;
		int yeigfactor=0;
		os::OS_ReadModeVariable(swi::XEigFactor,&xeigfactor);
		os::OS_ReadModeVariable(swi::YEigFactor,&yeigfactor);
		unsigned int xpix=1<<xeigfactor;
		unsigned int ypix=1<<yeigfactor;

		// Loop over lines that are at least partially within the selection.
		for (unsigned int i=select_first_pos.line();i<=select_last_pos.line();++i)
		{
			// Extract paragraph text for line.
			const string text=_text[_lines.find(i)];

			// Initially, assume that the selection covers the whole line.
			int xmin=bbox().xmin();
			int xmax=bbox().xmax();

			// If this is the first line of the selection then it may not
			// extend to the left margin.
			if (i==select_first_pos.line())
			{
				unsigned int index=select_first_pos.index_para()-
					select_first_pos.index_line();
				xmin=bbox().xmin()+
					line_width(text,index,select_first_pos.index_line());
			}

			// If this is the last line of the selection then it may not
			// extend to the right margin.
			if (i==select_last_pos.line())
			{
				unsigned int index=select_last_pos.index_para()-
					select_last_pos.index_line();
				xmax=bbox().xmin()+
					line_width(text,index,select_last_pos.index_line());
			}

			// Provided at least part of the line remains, invert it.
			if (xmax!=xmin)
			{
				int ymin=bbox().ymax()-int(i+1)*line_height();
				int ymax=bbox().ymax()-int(i)*line_height();
				context.plot(68,point(xmin,ymin));
				context.plot(102,point(xmax-xpix,ymax-ypix));
			}
		}
	}

	inherited::redraw(context,clip);
}

void text_area::handle_event(events::null_reason& ev)
{
	// No action unless a drag is in progress.
	if (_dragging)
	{
		// Get pointer location with respect to top left-hand corner
		// of text area, and then as a mark within the text.
		point offset;
		parent_application(offset);
		os::pointer_info_get block;
		os::Wimp_GetPointerInfo(block);
		point lpos=block.p-offset-bbox().xminymax();
		mark mk=snap(lpos);

		// Update preferred caret x-coordinate,
		// but only under standard selection model.
		if (_selection_model==model_standard) _caret_pref_x=lpos.x();

		// Update selection, to run between drag reference and
		// pointer location.
		show_selection(_dragref,mk);
	}
}

void text_area::handle_event(events::mouse_click& ev)
{
	// Get pointer location with respect to top left-hand corner
	// of text area, and then as a mark within the text.
	point lpos=ev.position()-bbox().xminymax();
	mark mk(snap(lpos));

	// Action depends on button stage.
	switch (ev.buttons())
	{
	case 0x400: /* Click select. */
		{
			// Update preferred caret x-coordinate.
			_caret_pref_x=lpos.x();

			if ((mk>=_select_first)&&(mk<_select_last)&&
				(_selection_model==model_standard))
			{
				// If click within existing selection under standard model
				// then reaffirm that selection.
				show_selection(_select_first,_select_last);
			}
			else
			{
				// Otherwise, move caret to pointer
				// (cancelling selection if appropriate).
				show_caret(mk,false,false);
			}

			// Set drag reference in case this click becomes a drag.
			_dragref=mk;
		}
		break;
	case 0x100: /* Click adjust. */
		{
			// Update preferred caret x-coordinate.
			_caret_pref_x=lpos.x();

			// Decide which end of the selection is fixed and which
			// will move.  Set the fixed end as the drag reference,
			// in case this click becomes a drag.
			if (abs(mk-_select_first)<abs(mk-_select_last))
			{
				// If click nearer to start of selection than end
				// then drag start, fix end.
				show_selection(mk,_select_last);
				_dragref=_select_last;
			}
			else
			{
				// If click nearer to end of selection than start
				// then drag end, fix start.
				show_selection(_select_first,mk);
				_dragref=_select_first;
			}
		}
		break;
	case 0x40: /* Drag select. */
		if (application* app=parent_application())
		{
			// Check whether there was a pre-existing selection.
			if ((_select_first!=_select_last)&&(mk>=_select_first)&&
				(mk<_select_last))
			{
				// Pre-existing selection, and mark within that selection:
				// This will begin a drag operation for drag-and-drop.
				// At present it does nothing.
			}
			else
			{
				// No selection:
				// Begin drag operation to adjust the selection.
				drag(box(),7);
				_dragging=true;
				app->register_null(*this);
			}
		}
		break;
	case 0x10: /* Drag adjust. */
		if (application* app=parent_application())
		{
			// Begin drag operation to adjust the selection.
			drag(box(),7);
			_dragging=true;
			app->register_null(*this);
		}
	}
}

void text_area::handle_event(events::key_pressed& ev)
{
	// Handle character codes with special meanings.
	// Otherwise, insert the character if it is printable,
	// ignore it if not.
	bool processed=true;
	int code=ev.code();
	switch (code)
	{
	case 0x003:handle_copy(); break;
	case 0x008:handle_delete_left(); break;
	case 0x00d:handle_linefeed(); break;
	case 0x016:handle_paste(); break;
	case 0x018:handle_cut(); break;
	case 0x01e:handle_start_of_line(); break;
	case 0x07f:handle_delete_right(); break;
	case 0x18b:handle_end_of_line(); break;
	case 0x18c:handle_left_char(); break;
	case 0x18d:handle_right_char(); break;
	case 0x18e:handle_down_lines(); break;
	case 0x18f:handle_up_lines(); break;
	case 0x19c:handle_left_word(); break;
	case 0x19d:handle_right_word(); break;
	case 0x1ab:handle_end_of_text(); break;
	case 0x1ac:handle_start_of_line(); break;
	case 0x1ad:handle_end_of_line(); break;
	case 0x1ae:handle_end_of_text(); break;
	case 0x1af:handle_start_of_text(); break;
	default:
		if ((code>=32&&code<=127)||(code>=128&&code<256))
		{
			handle_insert(ev.code());
		}
		else processed=false;
	}
	if (processed) ev.processed(true);
}

void text_area::handle_event(events::user_drag_box& ev)
{
	// When dragging has finished there is no further need
	// to receive null events.
	if (application* app=parent_application())
	{
		app->deregister_null(*this);
		_dragging=false;
	}
}

void text_area::handle_event(events::datasave& ev)
{
	// A Message_DataSave has been received, either in response
	// to a clipboard request or because the user has explicitly
	// dragged data from another window.
	// In either case the load object is made ready to receive
	// a data transfer.
	if (ev.target()==this)
	{
		if (application* app=parent_application())
		{
			_loadop.lines(_iclipboard);
			_loadop.redirect(this);
			app->add(_loadop);
			events::datasave ev2(_loadop,swi::User_Message,ev.wimpblock());
			ev2.post();
		}
	}
}

void text_area::handle_event(events::dataload& ev)
{
	// A Message_DataLoad has been received, either in response
	// to a clipboard request or because the user has explicitly
	// dragged data from another window.
	// In either case the load object is made ready to receive
	// a data transfer, unless the text area is read-only.
	if ((ev.target()==this)&&!_read_only)
	{
		if (application* app=parent_application())
		{
			_loadop.lines(_iclipboard);
			_loadop.redirect(this);
			app->add(_loadop);
			events::dataload ev2(_loadop,swi::User_Message,ev.wimpblock());
			ev2.post();
		}
	}
}

void text_area::handle_event(events::claim_entity& ev)
{
	if (ev.claim_selection())
	{
		if (_has_focus)
		{
			// If another application (or component of this application)
			// has claimed the caret/selection then note that this
			// component no longer has the focus.  The selection is
			// redrawn, allowing it to change appearance when the focus
			// is lost (if supported by the redraw method).
			_has_focus=false;
			force_redraw_between(_select_first,_select_last);
		}
	}
	if (ev.claim_clipboard())
	{
		// If another application (or component of this application)
		// has claimed the clipboard then clear any locally held
		// clipboard data.
		_saveop.clear();
		_oclipboard=text_type();
	}
}

void text_area::handle_event(events::datarequest& ev)
{
	// A Message_DataRequest has been received, meaning that
	// another application needs the content of the clipboard.
	// The save object is instructed to begin a data transfer.
	if (ev.request_clipboard())
	{
		if (application* app=parent_application())
		{
			app->add(_saveop);
		}
		events::datarequest ev2(_saveop,swi::User_Message,ev.wimpblock());
		ev2.post();
	}
}

void text_area::handle_event(events::loaded& ev)
{
	// A data transfer has been completed, either in response
	// to a clipboard request or because the user has explicitly
	// dragged data from another window.
	// In either case the data is inserted at the caret
	// (or in place of the selection if there is no caret),
	// unless the text area is read-only.
	// The style guide says that the inserted data is automatically
	// selected, but that has the unfortunately effect of preventing
	// multiple paste commands from doing anything.  The behaviour
	// at present is simply to move the caret to the end of the
	// inserted text.
	if (!_read_only)
	{
		if (!_iclipboard.size()) _iclipboard.push_back(string());
		replace(_caret_first,_caret_last,_iclipboard);
	}
	_iclipboard=text_type();
}

text_area::text_type text_area::text() const
{
	return _text;
}

text_area::text_type text_area::selection() const
{
	// Extract minimal sequence of paragraphs that contains the selection.
	text_type selection=_text.substr(_select_first.para(),
		_select_last.para()-_select_first.para()+1);

	// Ensure that selection is at least one paragraph long.
	if (!selection.size()) selection.push_back(string());

	// Remove extraneous text from end.
	string suffix=selection.back().substr(0,_select_last.index_para());
	selection.pop_back();
	selection.push_back(suffix);

	// Remove extraneous text from start.
	string prefix=selection.front().substr(_select_first.index_para(),
		string::npos);
	selection.pop_front();
	selection.push_front(prefix);

	// Return remaining text.
	return selection;
}

text_area& text_area::text(const text_type& text)
{
	if (!_read_only) replace(begin(),end(),text);
	return *this;
}

text_area& text_area::selection(const text_type& text)
{
	if (!_read_only) replace(_select_first,_select_last,text);
	return *this;
}

bool text_area::has_selection() const
{
	return _select_last!=_select_first;
}

text_area::mark text_area::begin() const
{
	return mark(_text,0,0);
}

text_area::mark text_area::end() const
{
	return mark(_text,_text.size()-1,_text.back().size());
}

void text_area::handle_left_char()
{
	// Move caret to previous character
	// (before selection if there is no caret).
	show_caret(--_caret_first,true,true);
}

void text_area::handle_right_char()
{
	// Move caret to next character
	// (after selection if there is no caret).
	show_caret(++_caret_last,true,true);
}

void text_area::handle_left_word()
{
	// Move caret to previous word
	// (before selection if there is no caret).
	show_caret(_caret_first.prev_word(),true,true);
}

void text_area::handle_right_word()
{
	// Move caret to next word
	// (after selection if there is no caret).
	show_caret(_caret_last.next_word(),true,true);
}

void text_area::handle_down_lines(int diff)
{
	// Determine the current line number.
	int line=fixed_mark(*this,_caret_last).line();

	// Determine the number of lines in the text.
	int lines=_lines.sum(_lines.size());

	// Change the line number by the specified amount,
	// without allowing it to go before the start or
	// after the end of the text.
	line+=diff;
	if (line<0) line=0;
	if (line>=lines) line=lines-1;

	// Place the caret as near as possible to its preferred
	// horizontal location on the appropriate line.
	show_caret(snap(line,_caret_pref_x),false,true);
}

void text_area::handle_start_of_line()
{
	// Move the caret to the start of the current line.
	fixed_mark caret_first_pos(*this,_caret_first);
	show_caret(snap(caret_first_pos.line(),0),true,true);
}

void text_area::handle_end_of_line()
{
	// Move the caret to the end of the current line.
	fixed_mark caret_last_pos(*this,_caret_last);
	show_caret(snap(caret_last_pos.line(),bbox().xsize()),true,true);
}

void text_area::handle_start_of_text()
{
	// Move the caret to the start of the text.
	show_caret(begin(),true,true);
}

void text_area::handle_end_of_text()
{
	// Move the caret to the end of the text.
	show_caret(end(),true,true);
}

void text_area::handle_insert(char code)
{
	if (!_read_only)
	{
		// If inserted character replaces a selection
		// then first copy that selection to the clipboard.
		if (_caret_first!=_caret_last) load_clipboard(_caret_first,_caret_last);

		// Insert character at caret
		// (or in place of selection if there is no caret).
		replace(_caret_first,_caret_last,string(1,code));
	}
}

void text_area::handle_linefeed()
{
	if (!_read_only)
	{
		// If inserted linefeed replaces a selection
		// then first copy that selection to the clipboard.
		if (_caret_first!=_caret_last) load_clipboard(_caret_first,_caret_last);

		// Insert linefeed at caret
		// (or in place of selection if there is no caret).
		text_type new_text;
		new_text.push_back(string());
		new_text.push_back(string());
		replace(_caret_first,_caret_last,new_text);
	}
}

void text_area::handle_delete_left()
{
	if (!_read_only)
	{
		// Delete from _caret_first to _caret_last if they differ,
		// otherwise delete the character to the left of _caret_first.
		if (_caret_first==_caret_last) --_caret_first;
		replace(_caret_first,_caret_last,string());
	}
}

void text_area::handle_delete_right()
{
	if (!_read_only)
	{
		// Delete from _caret_first to _caret_last if they differ,
		// otherwise delete the character to the right of _caret_last.
		if (_caret_last==_caret_first) ++_caret_last;
		replace(_caret_first,_caret_last,string());
	}
}

void text_area::handle_copy()
{
	// Only load the clipboard if there is a non-empty selection.
	if (_select_first!=_select_last)
	{
		load_clipboard(_select_first,_select_last);
	}
}

void text_area::handle_cut()
{
	// Only load the clipboard if there is a non-empty selection.
	if ((_select_first!=_select_last)&&!_read_only)
	{
		load_clipboard(_select_first,_select_last);
		replace(_select_first,_select_last,string());
	}
}

void text_area::handle_paste()
{
	// Broadcast Message_DataRequest to request that content of
	// clipboard be sent using the data transfer protocol.
	// Prepare the load operation to receive the data.
	point p(bbox().xminymin());
	if (desktop::application* app=parent_application(p))
	{
		if (basic_window* w=parent_work_area())
		{
			std::auto_ptr<os::wimp_block> block(new os::wimp_block);
			block->word[0]=48;
			block->word[3]=0;
			block->word[4]=swi::Message_DataRequest;
			block->word[5]=w->handle();
			block->word[6]=-1;
			block->word[7]=p.x();
			block->word[8]=p.y();
			block->word[9]=4;
			block->word[10]=0xfff;
			block->word[11]=-1;
			app->send_message(swi::User_Message,*(block.release()),0,0);
			app->add(_loadop);
		}
	}
}

text_area& text_area::font(const graphics::font& font)
{
	_font=font;
	reflow(bbox().xsize());
	invalidate();
	return *this;
}

text_area& text_area::fcolour(int fcolour)
{
	if (fcolour!=_fcolour)
	{
		_fcolour=fcolour;
		force_redraw();
	}
	return *this;
}

text_area& text_area::bcolour(int bcolour)
{
	if (bcolour!=_bcolour)
	{
		_bcolour=bcolour;
		force_redraw();
	}
	return *this;
}

text_area& text_area::ccolour(int ccolour)
{
	if (ccolour!=_ccolour)
	{
		_ccolour=ccolour;
		force_redraw();
	}
	return *this;
}

text_area& text_area::line_height(int line_height)
{
	_auto_line_height=!line_height;
	if (_auto_line_height) _line_height=auto_line_height();
	else _line_height=line_height;
	return *this;
}

text_area& text_area::baseline_offset(int baseline_offset)
{
	_auto_baseline_offset=!baseline_offset;
	if (_auto_baseline_offset) _baseline_offset=auto_baseline_offset();
	else _baseline_offset=baseline_offset;
	return *this;
}

text_area& text_area::wrap_method(wrap_method_type wrap_method)
{
	if (_wrap_method!=wrap_method)
	{
		_wrap_method=wrap_method;
		reflow(bbox().xsize());
		invalidate();
	}
	return *this;
}

text_area& text_area::read_only(bool read_only)
{
	_read_only=read_only;
	return *this;
}

text_area& text_area::selection_model(selection_model_type selection_model)
{
	if (_selection_model!=selection_model)
	{
		// If switching to a combined caret and selection then it
		// is necessary to choose either the existing caret or the
		// existing selection.  Choosing the caret is less likely
		// to have unexpected consequences.
		if (selection_model==model_standard)
		{
			show_selection(_caret_first,_caret_last);
		}

		_selection_model=selection_model;

		// If switching from a selection to a caret and a selection
		// then it is necessary to choose a location for the caret.
		// The start of the selection is as good a choice as any.
		if (_selection_model==model_traditional)
		{
			show_caret(_select_first,true,true);
		}
	}
	return *this;
}

int text_area::auto_line_height() const
{
	// A variable-length buffer is needed to receive the
	// font definition.  First determine how large that
	// buffer need be, then read the font definition,
	// then extract the required information.
	int buffer_size=0;
	os::Font_ReadDefn(_font.handle(),&buffer_size);
	char id[buffer_size];
	int ysize=0;
	os::Font_ReadDefn(_font.handle(),id,0,&ysize,0,0,0,0);
	return (ysize*3)/16;
}

int text_area::auto_baseline_offset() const
{
	// At present, the baseline offset is presumed to be
	// one quarter of the line height.
	return line_height()/4;
}

void text_area::render_line(gcontext& context,const string& ptext,
	unsigned int index,unsigned int count,const point& p) const
{
	// As a precaution, place limits on index and count.
	if (index>ptext.size()) index=ptext.size();
	if (count>ptext.size()-index) count=ptext.size()-index;

	// Initialise pointers: beginning, end and iterator.
	const char* b=ptext.c_str()+index;
	const char* e=b+count;
	const char* t=b;

	int x=p.x();
	int y=p.y();
	while (t!=e)
	{
		// Let f be a non-const pointer into the string.
		// Note that the string must remain logically const,
		// therefore any changes made to it must be undone.
		char* f=const_cast<char*>(t);

		// Move f to the next non-printable character
		// (or the end of the string if that occurs first).
		while ((f!=e)&&isprint(*f)) ++f;

		if (f!=t)
		{
			// If at least one printable character then render those
			// characters (after inserting a temporary terminator).
			char temp=0;
			std::swap(*f,temp);
			context.fcolour(_fcolour);
			context.draw(_font,t,point(x,y));
			x+=_font.width(t,f-t);
			std::swap(*f,temp);
			t=f;
		}
		else
		{
			// If no printable characters then render the first
			// non-printable character (after converting to hex).
			control_string cs(*t++);
			context.fcolour(_ccolour);
			context.draw(_font,cs,point(x,y));
			x+=_font.width(cs,4);
		}
	}
}

int text_area::line_width(const string& ptext,unsigned int index,
	unsigned int count) const
{
	// As a precaution, place limits on index and count.
	if (index>ptext.size()) index=ptext.size();
	if (count>ptext.size()-index) count=ptext.size()-index;

	// Initialise pointers: beginning, end and iterator.
	const char* b=ptext.c_str()+index;
	const char* e=b+count;
	const char* t=b;

	int width=0;
	while (t!=e)
	{
		// Let f be a non-const pointer into the string.
		// Note that the string must remain logically const,
		// therefore any changes made to it must be undone.
		char* f=const_cast<char*>(t);

		// Move f to the next non-printable character
		// (or the end of the string if that occurs first).
		while ((f!=e)&&isprint(*f)) ++f;

		// If at least one non-printable character then measure
		// those characters (after inserting a temporary terminator).
		if (f!=t)
		{
			char temp=0;
			std::swap(*f,temp);
			width+=_font.width(t,f-t);
			std::swap(*f,temp);
			t=f;
		}
		else
		{
			// If no printable characters then measure the first
			// non-printable character (after converting to hex).
			control_string cs(*t++);
			width+=_font.width(cs,4);
		}
	}
	return width;
}

unsigned int text_area::find_index(const string& ptext,unsigned int index,
	int x) const
{
	// As a precaution, place limits on index and count.
	if (index>ptext.size()) index=ptext.size();

	// Initialise pointers: beginning, end and iterator.
	const char* b=ptext.c_str()+index;
	const char* e=b+ptext.size()-index;
	const char* t=b;

	while (x&&t!=e)
	{
		// Let f be a non-const pointer into the string.
		// Note that the string must remain logically const,
		// therefore any changes made to it must be undone.
		char* f=const_cast<char*>(t);

		// Move f to the next non-printable character
		// (or the end of the string if that occurs first).
		while ((f!=e)&&isprint(*f)) ++f;

		if (f!=t)
		{
			// If at least one non-printable character then search for
			// the specified coordinate within those characters (after
			// inserting a temporary terminator).
			char temp=0;
			std::swap(*f,temp);
			point p_mp(x*400,0);
			point bbox;
			const char* q=t;
			os::Font_ScanString(_font.handle(),t,0x20180,p_mp,
				0,0,f-t,&q,&bbox,0);
			std::swap(*f,temp);

			if ((q==f)&&(x>bbox.x()/400))
			{
				// If reached end of text fragment but not specified
				// coordinate then subtract width of fragment and
				// continue searching.
				x-=bbox.x()/400;
			}
			else
			{
				// Otherwise, index has been found.
				x=0;
			}

			t=q;
		}
		else
		{
			// If no printable characters then test for the
			// specified coordinate within the first non-printable
			// character (after converting to hex).
			control_string cs(*t);
			int w=_font.width(cs,4);
			if (w<=x)
			{
				// If specified coordinate is at or beyond end of
				// control character then subtract width, move to
				// next character and continue searching.
				x-=w;
				++t;
			}
			else if (w<=x*2)
			{
				// Otherwise, if coordinate is beyond halfway point
				// then move to next character but stop searching.
				x=0;
				++t;
			}
			else
			{
				// Otherwise, move no further and stop searching.
				x=0;
			}
		}
	}

	return t-b;
}

unsigned int text_area::split_line(const string& ptext,unsigned int index,
	int width,bool include_trailing) const
{
	// As a precaution, place limits on index, count and width.
	if (index>ptext.size()) index=ptext.size();
	if (width<1) width=1;

	// Initialise pointers: beginning, end and iterator.
	const char* b=ptext.c_str()+index;
	const char* e=b+ptext.size()-index;
	const char* t=b;

	// If line wrap method is wrap_none then go immediately to end.
	if (_wrap_method==wrap_none) t=e;

	while (width&&(t!=e))
	{
		// Let f be a non-const pointer into the string.
		// Note that the string must remain logically const,
		// therefore any changes made to it must be undone.
		char* f=const_cast<char*>(t);

		// Move f to the next non-printable character
		// (or the end of the string if that occurs first).
		while ((f!=e)&&isprint(*f)) ++f;

		if (f!=t)
		{
			// Insert temporary terminator.
			char temp=0;
			std::swap(*f,temp);

			point p_mp(width*400,0);
			const char* q=t;
			point bbox;

			// If line wrap method is wrap_word then try to find
			// a break point using space as a split character.
			if (_wrap_method==wrap_word)
			{
				os::Font_ScanString(_font.handle(),t,0x00120,p_mp,
					&_font_coord_block,0,0,&q,&bbox,0);
				if (include_trailing) while (*q==split_char) ++q;
			}

			// If no progress at all has been made (or attempted)
			// then try again without a split character.
			if (q==t)
			{
				os::Font_ScanString(_font.handle(),t,0x00100,p_mp,
					0,0,0,&q,&bbox,0);
				if (include_trailing) while (*q==split_char) ++q;
			}

			// Remove temporary terminator.
			std::swap(*f,temp);

			if ((q==f)&&(width>bbox.x()/400))
			{
				// If reached end of text fragment but not specified
				// width then subtract width of fragment and continue
				// searching.
				width-=bbox.x()/400;
			}
			else
			{
				// Otherwise, split point has been found.
				width=0;
			}

			t=q;
		}
		else
		{
			// If no printable characters then test for a
			// split point following the first non-printable
			// character (after converting to hex).
			control_string cs(*t);
			int w=_font.width(cs,4);
			if (w<=width)
			{
				// If control character extends to or beyond
				// remaining width then subtract width, move to
				// next character and continue searching.
				width-=w;
				++t;
			}
			else
			{
				// Otherwise, stop searching.
				width=0;
			}
		}
	}

	// Ensure that some progress is made.
	if ((t==b)&&(t!=e)) ++t;

	return t-b;
}

text_area::mark text_area::snap(const point& p) const
{
	unsigned int line=max(-p.y(),0)/line_height();
	return snap(line,p.x());
}

text_area::mark text_area::snap(unsigned int line,int x) const
{
	int width=bbox().xsize();

	// Ensure that line does not exceed number of lines in text.
	line=min(line,_lines.sum(_lines.size())-1);

	unsigned int para=_lines.find(line);
	const string ptext=_text[para];

	// Find line within paragraph.
	unsigned int i=line-_lines.sum(para);
	unsigned int j=0;
	while (i>0)
	{
		j+=split_line(ptext,j,width);
		--i;
	}

	// Find specified coordinate within line.
	unsigned int index_line=find_index(ptext,j,x);
	unsigned int index_para=index_line+j;

	// Ensure that index is not past split point.
	unsigned int split=split_line(ptext,index_para-index_line,width,false);
	if (index_line>split)
	{
		unsigned int excess=index_line-split;
		index_para-=excess;
		index_line-=excess;
	}

	return mark(_text,para,index_para);
}

void text_area::reflow(int old_width,int new_width)
{
	fixed_mark caret_first_pos(*this,_caret_first);
	fixed_mark caret_last_pos(*this,_caret_last);

	// Ensure that size of location cache matches number of paragraphs.
	_lines.resize(_text.size());

	// First pass: go to end of text if bounding box is narrowing.
	// (This is so that the second pass can run backwards in that case.)
	unsigned int old_line=0;
	unsigned int new_line=0;
	if (new_width<old_width) for (unsigned int i=0;i!=_text.size();++i)
	{
		const string ptext=_text[i];

		unsigned int old_pos=0;
		unsigned int new_pos=0;
		bool first_line=true;
		while (first_line||(old_pos<ptext.size())||(new_pos<ptext.size()))
		{
			bool has_old_line=first_line||(old_pos<ptext.size());
			bool has_new_line=first_line||(new_pos<ptext.size());

			old_pos+=split_line(ptext,old_pos,old_width);
			new_pos+=split_line(ptext,new_pos,new_width);

			if (has_old_line) ++old_line;
			if (has_new_line) ++new_line;

			first_line=false;
		}
	}

	// Second pass: search for paragraphs that can be copied.
	// This pass runs forwards if widening, backwards if narrowing.
	for (unsigned int i=0;i!=_text.size();++i)
	{
		unsigned int j=(new_width<old_width)?_text.size()-i-1:i;
		const string ptext=_text[j];

		bool can_copy=true;
		unsigned int old_pos=0;
		unsigned int new_pos=0;
		unsigned int old_lines=0;
		unsigned int new_lines=0;
		bool first_line=true;
		while (first_line||(old_pos<ptext.size())||(new_pos<ptext.size()))
		{
			bool has_old_line=first_line||(old_pos<ptext.size());
			bool has_new_line=first_line||(new_pos<ptext.size());

			old_pos+=split_line(ptext,old_pos,old_width);
			new_pos+=split_line(ptext,new_pos,new_width);

			if (new_pos!=old_pos) can_copy=false;

			if (has_old_line) ++old_lines;
			if (has_new_line) ++new_lines;

			first_line=false;
		}

		// Move up (to start of paragraph) if narrowing.
		if (new_width<old_width)
		{
			old_line-=old_lines;
			new_line-=new_lines;
		}

		// If paragraphs have identical line breaks, but different
		// positions, then copy.
		if (can_copy&&(new_line!=old_line))
		{
			int xmin=bbox().xmin();
			int xmax=bbox().xmax();
			int old_ymax=-old_line*line_height();
			int old_ymin=-(old_line+old_lines)*line_height();
			int new_ymin=-(new_line+new_lines)*line_height();
			box src(xmin,old_ymin,xmax,old_ymax);
			point dst(xmin,new_ymin);

			// Hide the caret if it falls within the source or destination
			// of the block copy operation.
			if (((caret_last_pos.line()>=old_line)&&
				(caret_first_pos.line()<old_line+old_lines))||
				((caret_last_pos.line()>=new_line)&&
				(caret_first_pos.line()<new_line+new_lines)))
			{
				hide_caret();
			}

			point offset;
			if (basic_window* w=parent_work_area(offset))
			{
				if (int handle=w->handle())
				{
					os::Wimp_BlockCopy(handle,src+offset,dst+offset);
				}
			}
		}

		// Move down (to start of next paragraph) if widening.
		if (new_width>old_width)
		{
			old_line+=old_lines;
			new_line+=new_lines;
		}
	}

	// Third pass: search for lines to be redrawn.
	old_line=0;
	new_line=0;
	for (unsigned int i=0;i!=_text.size();++i)
	{
		unsigned int new_line_start=new_line;
		const string ptext=_text[i];

		bool can_copy=true;
		bool pending_copy=new_line!=old_line;
		unsigned int old_pos=0;
		unsigned int new_pos=0;
		bool first_line=true;
		while (first_line||(old_pos<ptext.size())||(new_pos<ptext.size()))
		{
			bool has_old_line=first_line||(old_pos<ptext.size());
			bool has_new_line=first_line||(new_pos<ptext.size());

			unsigned int old_start=old_pos;
			unsigned int new_start=new_pos;

			old_pos+=split_line(ptext,old_pos,old_width);
			new_pos+=split_line(ptext,new_pos,new_width);

			// Test whether paragraph so far can be moved by copying.
			// (The copying, if required and allowed, will already have
			// happened.  The purpose of this test is to take alternative
			// action if movement was required but copying not allowed.)
			if (can_copy&&(new_pos!=old_pos))
			{
				// Copying is not allowed.
				if (pending_copy&&(new_line!=new_line_start))
				{
					// Movement is required but copying not allowed.
					// The area in question must be invalidated instead.
					int ymax=-new_line_start*line_height();
					int ymin=-new_line*line_height();
					box rbox(bbox().xmin(),ymin,bbox().xmax(),ymax);
					force_redraw(rbox);
				}
				can_copy=false;
			}

			if (has_new_line)
			{
				int ymax=-new_line*line_height();
				int ymin=-(new_line+1)*line_height();
				if ((new_start!=old_start)||
					((new_line!=old_line)&&!can_copy))
				{
					// If index at start of line has changed then must
					// redraw whole line.  Likewise if line has moved
					// upwards or downwards and has not been copied.
					box rbox(bbox().xmin(),ymin,bbox().xmax(),ymax);
					force_redraw(rbox);
				}
				else if (new_pos!=old_pos)
				{
					// If only end has changed then partial redraw
					// will suffice.
					unsigned int count=min(new_pos,old_pos)-old_start;
					int x=line_width(ptext,old_start,count);
					if (!count) x=0;
					box rbox(bbox().xmin()+x,ymin,bbox().xmax(),ymax);
					force_redraw(rbox);
				}
			}

			if (has_old_line) ++old_line;
			if (has_new_line) ++new_line;

			first_line=false;
		}

		_lines[i]=new_line-new_line_start;
	}

	// If number of lines has fallen then force redraw of
	// region to be vacated at the end of the text.
	if (new_line<old_line)
	{
		int ymax=-new_line*line_height();
		int ymin=-(old_line+1)*line_height();
		force_redraw(box(bbox().xmin(),ymin,bbox().xmax(),ymax));
	}

	// Reposition caret if necessary.
	if (_has_focus&&(_caret_first==_caret_last))
		show_caret(_caret_first,true,true);
}

void text_area::reflow(int width)
{
	// Ensure that size of location cache matches number of paragraphs.
	_lines.resize(_text.size());

	// Calculate number of lines in each paragraph, record in _lines.
	unsigned int line=0;
	for (unsigned int i=0;i!=_text.size();++i)
	{
		unsigned int line_start=line;
		const string ptext=_text[i];
		unsigned int pos=0;
		bool first_line=true;
		while (first_line||(pos<ptext.size()))
		{
			pos+=split_line(ptext,pos,width);
			++line;
			first_line=false;
		}
		_lines[i]=line-line_start;
	}

	// Redraw everything.
	force_redraw();

	// Reposition caret if necessary.
	if (_has_focus&&(_caret_first==_caret_last))
		show_caret(_caret_first,true,true);
}

void text_area::load_clipboard(mark first,mark last)
{
	// Ensure that last>=first.
	if (last<first) std::swap(last,first);

	// Extract minimal sequence of paragraphs that contains the sequence.
	_oclipboard=_text.substr(first.para(),last.para()-first.para()+1);

	// Ensure that sequence is at least one paragraph long.
	if (!_oclipboard.size()) _oclipboard.push_back(string());

	// Remove extraneous text from end.
	string suffix=_oclipboard.back().substr(0,last.index_para());
	_oclipboard.pop_back();
	_oclipboard.push_back(suffix);

	// Remove extraneous text from start.
	string prefix=_oclipboard.front().substr(first.index_para(),string::npos);
	_oclipboard.pop_front();
	_oclipboard.push_front(prefix);

	// Update save operation with new clipboard content.
	_saveop.lines(_oclipboard.begin(),_oclipboard.end());
	_saveop.final_newline(false);

	claim_clipboard();
}

void text_area::show_caret(const mark& mk,bool set_pref_x,bool follow)
{
	// If text is read-only then do nothing.
	if (_read_only) return;

	// If standard selection model then cancel selection.
	if (_selection_model==model_standard)
	{
		force_redraw_between(_select_first,_select_last);
		_select_first=mk;
		_select_last=mk;
	}

	if (!_has_focus)
	{
		claim_selection();
		_has_focus=true;
	}

	_caret_first=mk;
	_caret_last=mk;

	fixed_mark mk_pos(*this,mk);
	point ext_pos(bbox().xminymax()+mk_pos.position());
	set_caret_position(ext_pos,line_height(),-1);
	if (set_pref_x) _caret_pref_x=mk_pos.position().x();

	if (follow)
	{
		events::auto_scroll ev(*this,box(0,0,0,line_height())+ext_pos);
		ev.post();
	}
}

void text_area::hide_caret()
{
	// Take no action unless this component believes that it has the caret.
	if (_has_focus&&(_caret_first==_caret_last))
	{
		if (basic_window* w=parent_work_area())
		{
			// Get caret position from Wimp.
			os::caret_position_get block;
			os::Wimp_GetCaretPosition(block);

			// For safety, take no action unless the caret and this component
			// are located within the same window.  Also, take no action if
			// the caret is already invisible.
			if ((block.whandle==w->handle())&&!(block.height&caret_invisible))
			{
				os::Wimp_SetCaretPosition(block.whandle,block.ihandle,
					block.p,block.height|caret_invisible,block.index);
			}
		}
	}
}

void text_area::show_selection(mark first,mark last)
{
	// Ensure that last>=first.
	if (last<first) std::swap(last,first);

	if (!_has_focus)
	{
		claim_selection();
		_has_focus=true;
	}

	if (_select_first!=first)
	{
		force_redraw_between(_select_first,first);
		_select_first=first;
	}

	if (_select_last!=last)
	{
		force_redraw_between(_select_last,last);
		_select_last=last;
	}

	// If using the standard selection model then the selection
	// and the caret are essentially the same entity, therefore
	// a change to one must be reflected in the other.
	if (_selection_model==model_standard)
	{
		_caret_first=_select_first;
		_caret_last=_select_last;
		
		point ext_pos(bbox().xminymax()+
			fixed_mark(*this,_caret_first).position());
		int flags=(_caret_first==_caret_last)?0:caret_invisible;
		set_caret_position(ext_pos,line_height()|flags,-1);
	}
}

void text_area::force_redraw_between(mark first,mark last)
{
	// Ensure that last>=first.
	if (last<first) std::swap(last,first);

	fixed_mark first_pos(*this,first);
	fixed_mark last_pos(*this,last);

	// Loop over all lines potentially within the region.
	for (unsigned int i=first_pos.line();i<=last_pos.line();++i)
	{
		const string ptext=_text[_lines.find(i)];

		// Assume initially that redraw covers whole region.
		int xmin=bbox().xmin();
		int xmax=bbox().xmax();

		// If this is the first line then adjust the left bound.
		if (i==first_pos.line())
		{
			unsigned int index=first_pos.index_para()-first_pos.index_line();
			xmin=bbox().xmin()+line_width(ptext,index,first_pos.index_line());
		}

		// If this is the last line then adjust the right bound.
		if (i==last_pos.line())
		{
			unsigned int index=last_pos.index_para()-last_pos.index_line();
			xmax=bbox().xmin()+line_width(ptext,index,last_pos.index_line());
		}

		// If there is anything left then redraw between bounds.
		if (xmax!=xmin)
		{
			int ymin=bbox().ymax()-int(i+1)*line_height();
			int ymax=bbox().ymax()-int(i)*line_height();
			force_redraw(box(xmin,ymin,xmax,ymax));
		}
	}
}

void text_area::replace(const mark& first,const mark& last,
	const string& new_para)
{
	text_type new_text;
	new_text.push_back(new_para);
	replace(first,last,new_text);
}

void text_area::replace(mark first,mark last,const text_type& new_text)
{
	// Ensure that last>=first.
	if (last<first) std::swap(last,first);

	adjust_layout(first,last,new_text);
	adjust_text(first,last,new_text);
	_caret_first=adjust_mark(_caret_first,first,last,new_text);
	_caret_last=adjust_mark(_caret_last,first,last,new_text);
	_select_first=adjust_mark(_select_first,first,last,new_text);
	_select_last=adjust_mark(_select_last,first,last,new_text);

	if (_has_focus&&(_caret_first==_caret_last))
	{
		show_caret(_caret_first,true,true);
	}

	invalidate();
}

void text_area::adjust_layout(const mark& first,const mark& last,
	const text_type& new_text)
{
	int width=bbox().xsize();

	// Calculate the number of paragraphs to be replaced (in part
	// or in full), the number of replacement paragraphs, and the
	// existing number of lines.
	unsigned int old_paras=last.para()-first.para()+1;
	unsigned int new_paras=new_text.size();
	unsigned int old_lines=_lines.sum(_lines.size());

	// Adjust size of _lines if necessary.
	if (new_paras>old_paras)
	{
		_lines.insert(last.para()+1,new_paras-old_paras,0);
	}
	if (new_paras<old_paras)
	{
		_lines.erase(first.para()+new_paras,first.para()+old_paras);
	}

	// Maintain a record of the current line number.
	unsigned int line=_lines.sum(first.para());

	// Maintain a record of the number of lines by which the remainder
	// of the text has moved downwards (or upwards if negative).
	int diff=0;

	// Initialise iterators.
	unsigned int old_para=first.para();
	unsigned int new_para=0;
	unsigned int old_index=0;
	unsigned int new_index=0;

	// Process one line at a time, until either the old text or
	// the new text is exhausted.
	while ((old_para!=last.para()+1)&&(new_para!=new_text.size()))
	{
		const string old_ptext=_text[old_para];
		string new_ptext=new_text[new_para];
		if (new_para==0) new_ptext.insert(0,
			_text[first.para()].substr(0,first.index_para()));
		if (new_para==new_text.size()-1) new_ptext.append(
			_text[last.para()].substr(last.index_para(),string::npos));

		unsigned int old_split=split_line(old_ptext,old_index,width);
		unsigned int new_split=split_line(new_ptext,new_index,width);

		unsigned int common=0;
		while ((common!=old_split)&&(common!=new_split)&&
			(old_ptext[old_index+common]==new_ptext[new_index+common]))
		{
			++common;
		}

		unsigned int common_width=line_width(old_ptext,old_index,common);
		force_redraw(box(common_width,-(line+1)*line_height(),
			width,-line*line_height()));

		++line;

		old_index+=old_split;
		if (old_index>=old_ptext.size())
		{
			old_index=0;
			++old_para;
		}

		new_index+=new_split;
		if (new_index>=new_ptext.size())
		{
			unsigned int lines=line-_lines.sum(first.para()+new_para);
			_lines[first.para()+new_para]=lines;
			new_index=0;
			++new_para;
		}
	}

	// Continue processing one line at a time, until the old text
	// is exhausted.
	while (old_para!=last.para()+1)
	{
		const string old_ptext=_text[old_para];
		unsigned int old_split=split_line(old_ptext,old_index,width);

		force_redraw(box(0,-(line+1)*line_height(),
			width,-line*line_height()));

		++line;
		--diff;

		old_index+=old_split;
		if (old_index>=old_ptext.size())
		{
			old_index=0;
			++old_para;
		}
	}

	// Continue processing one line at a time, until the new text
	// is exhausted.
	while (new_para!=new_text.size())
	{
		string new_ptext=new_text[new_para];
		if (new_para==0) new_ptext.insert(0,
			_text[first.para()].substr(0,first.index_para()));
		if (new_para==new_text.size()-1) new_ptext.append(
			_text[last.para()].substr(last.index_para(),string::npos));

		unsigned int new_split=split_line(new_ptext,new_index,width);

		force_redraw(box(0,-(line+1)*line_height(),
			width,-line*line_height()));

		++line;
		++diff;

		new_index+=new_split;
		if (new_index>=new_ptext.size())
		{
			unsigned int lines=line-_lines.sum(first.para()+new_para);
			_lines[first.para()+new_para]=lines;
			new_index=0;
			++new_para;
		}
	}

	// If there has been a net movement upwards or downwards
	// then perform a block copy of the remaining text.
	if (diff)
	{
		unsigned int new_lines=_lines.sum(_lines.size());
		unsigned int max_lines=max(old_lines,new_lines);

		box src(0,-(max_lines-diff)*line_height(),
			width,-(line-diff)*line_height());
		point dst=src.xminymin()+point(0,-diff*line_height());

		point offset;
		if (basic_window* w=parent_work_area(offset))
		{
			if (int handle=w->handle())
			{
				os::Wimp_BlockCopy(handle,src+offset,dst+offset);
			}
		}
	}
}

void text_area::adjust_text(const mark& first,const mark& last,
	text_type new_text)
{
	// Ensure that new_text contains at least one paragraph.
	if (!new_text.size()) new_text.push_back(string());

	// Combine last new paragraph with existing text after mark.
	string suffix(new_text.back());
	new_text.pop_back();
	suffix.append(_text[last.para()].substr(last.index_para(),string::npos));
	new_text.push_back(suffix);

	// Combine first new paragraph with existing text prior to mark.
	string prefix(new_text.front());
	new_text.pop_front();
	prefix.insert(0,_text[first.para()].substr(0,first.index_para()));
	new_text.push_front(prefix);

	// Replace paragraphs.
	_text.erase(first.para(),last.para()-first.para()+1);
	_text.insert(first.para(),new_text);
}

text_area::mark text_area::adjust_mark(mark mk,const mark& first,
	const mark& last,const text_type& new_text) const
{
	if (mk>=last)
	{
		// Mark located after end of replacement region:
		unsigned int para=mk.para();
		unsigned int index_para=mk.index_para();
		para-=last.para()-first.para();
		para+=new_text.size()-1;
		if (mk.para()==last.para())
		{
			// Mark in last affected paragraph, after replacement region:
			index_para-=last.index_para();
			if (new_text.size()==1)
			{
				index_para+=first.index_para();
			}
			index_para+=new_text.back().size();
		}
		mk=mark(_text,para,index_para);
	}
	else if (mk>=first)
	{
		// Mark located within replacement region:
		unsigned int para=first.para();
		unsigned int index_para=0;
		para+=new_text.size()-1;
		if (new_text.size()==1)
		{
			index_para+=first.index_para();
		}
		index_para+=new_text.back().size();
		mk=mark(_text,para,index_para);
	}
	return mk;
}

}; /* namespace desktop */
}; /* namespace rtk */
