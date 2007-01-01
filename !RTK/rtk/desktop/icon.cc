// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003-2004 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#include "rtk/swi/os.h"
#include "rtk/swi/wimp.h"
#include "rtk/os/os.h"
#include "rtk/os/wimp.h"
#include "rtk/os/exception.h"
#include "rtk/desktop/icon.h"
#include "rtk/desktop/basic_window.h"
#include "rtk/desktop/application.h"
#include "rtk/events/wimp.h"
#include "rtk/events/mouse_click.h"
#include "rtk/events/key_pressed.h"
#include "rtk/events/message.h"
#include "rtk/events/help_request.h"

namespace rtk {
namespace desktop {

namespace {

/** Parse spaces in validation string.
 * Skip any number of spaces in the validation string.
 * @param i reference to validation string iterator
 */
void parse_spaces(char*& i);

/** Parse semicolon in validation string.
 * Search for a semicolon in the validation string then skip past it.
 * Any preceeding characters are ignored.
 * @param i reference to validation string iterator
 */
void parse_semicolon(char*& i);

/** Parse unsigned integer in validation string.
 * Parse unsigned integer.  Stop at first non-digit.
 * If no digits are found then the result will be zero.
 * @param i reference to validation string iterator
 * @param value buffer for returned integer
 */
void parse_int(char*& i,unsigned int* value);

/** Parse string in validation string.
 * Parse string terminated by comma or semicolon.  Stop at but
 * do not skip past terminator.  The result string is
 * null-terminated.
 * @param i reference to validation string iterator
 * @param buffer buffer for returned string
 * @param size size of result buffer, excluding terminator
 */
void parse_string(char*& i,char* buffer,unsigned int size);

/** Parse validation string.
 * The border type is extracted from the R command, if present.
 * The sprite names are extracted from the S command, if present.
 * If either or both of these commands are not present then the
 * corresponding buffers are left unchanged.  The buffers should
 * therefore be initialised with default values before this
 * function is called.  The length of each sprite name is limited
 * to 12 characters plus a null terminator, therefore the capacity
 * of each buffer should be at least 13 bytes.
 * If there is more than one instance of a
 * given command then the last one takes precedence.
 * @param i reference to validation string iterator
 * @param border_type buffer for returned border type
 * @param sprite0 buffer for first returned sprite name
 * @param sprite1 buffer for second returned sprite name
 */
void parse_validation(char*& i,unsigned int* border_type,
	char* sprite0,char* sprite1);

/** Get size of sprite, given name.
 * @param area the sprite area
 * @param name the sprite name
 * @param _xsize a buffer for the returned width
 * @param _ysize a buffer for the returned height
 */
void sprite_size(os::sprite_area* area,const char* name,
	int* _xsize,int* _ysize);

/** Get size of sprite, given pointer.
 * @param area the sprite area
 * @param name the sprite
 * @param _xsize a buffer for the returned width
 * @param _ysize a buffer for the returned height
 */
void sprite_size(os::sprite_area* area,os::sprite* sprite,
	int* _xsize,int* _ysize);

/** Get border box for given icon border type.
 * @param border true if the icon has a border, otherwise false
 * @param border_type the icon border type (from its validation string)
 * @return the border box
 */
box make_border_box(bool border,unsigned int border_type);

void parse_spaces(char*& i)
{
	while (*i==' ') ++i;
}

void parse_semicolon(char*& i)
{
	while ((*i!=0)&&(*i!=';'))
	{
		if (*i=='\\')
		{
			++i;
			if (*i!=0) ++i;
		}
		else ++i;
	}
	if (*i==';') ++i;
}

void parse_int(char*& i,unsigned int* value)
{
	int result=0;
	while ((*i>='0')&&(*i<='9'))
	{
		result=(result*10)+(*i-'0');
		++i;
	}
	if (value) *value=result;
}

void parse_string(char*& i,char* buffer,unsigned int size)
{
	while ((*i!=0)&&(*i!=',')&&(*i!=';')&&size)
	{
		if (*i=='\\') ++i;
		char ch=*i++;
		if (buffer) *buffer++=ch;
		--size;
	}
	while ((*i!=0)&&(*i!=',')&&(*i!=';')) ++i;
	if (buffer) *buffer=0;
}

void parse_validation(char*& i,unsigned int* border_type,char* sprite0,
	char* sprite1)
{
	while (*i!=0)
	{
		parse_spaces(i);
		switch (*i)
		{
		case 'r':
		case 'R':
			++i;
			parse_spaces(i);
			parse_int(i,border_type);
			parse_spaces(i);
			parse_int(i,0);
			parse_semicolon(i);
			break;
		case 's':
		case 'S':
			++i;
			parse_string(i,sprite0,12);
			if (*i==',')
			{
				++i;
				parse_string(i,sprite1,12);
			}
			else
			{
				if (sprite1) sprite1[0]=0;
			}
			parse_semicolon(i);
		default:
			parse_semicolon(i);
			break;
		}
	}
}

void sprite_size(os::sprite_area* area,const char* name,
	int* _xsize,int* _ysize)
{
	int xsize=0;
	int ysize=0;
	if (name)
	{
		try {
			if (area) os::OS_SpriteOp40(area,name,&xsize,&ysize,0,0);
			else os::Wimp_SpriteOp40(name,&xsize,&ysize,0,0);
			unsigned scale[4]={1,1,1,1};
			if (area) os::Wimp_ReadPixTrans(area,name,scale,0);
			else os::Wimp_ReadPixTrans((os::sprite_area*)1,name,scale,0);
			xsize*=scale[0];
			ysize*=scale[1];
			xsize/=scale[2];
			ysize/=scale[3];
		}
		catch (os::exception) {
			// Supress exceptions due to the sprite not existing
			xsize=0;
			ysize=0;
		}
	}
	if (_xsize) *_xsize=xsize;
	if (_ysize) *_ysize=ysize;
}

void sprite_size(os::sprite_area* area,os::sprite* sprite,
	int* _xsize,int* _ysize)
{
	int xsize=0;
	int ysize=0;
	if (area&&sprite)
	{
		os::OS_SpriteOp40(area,sprite,&xsize,&ysize,0,0);
		unsigned scale[4]={1,1,1,1};
		os::Wimp_ReadPixTrans(area,sprite,scale,0);
		xsize*=scale[0];
		ysize*=scale[1];
		xsize/=scale[2];
		ysize/=scale[3];
	}
	if (_xsize) *_xsize=xsize;
	if (_ysize) *_ysize=ysize;
}

box make_border_box(bool border,unsigned int border_type)
{
	box bdrbox;
	if (border)
	{
		// Note: the following constants are based on
		// the amount by which the Wimp moves the content
		// of an icon when a given border is added.  If
		// different behaviour is required (as it is to
		// fully comply with the style guide) then it
		// should be implemented in a subclass.
		switch (border_type)
		{
		case 1:
		case 2:
		case 5:
			bdrbox=box(-4,-4,4,4);
			break;
		case 3:
		case 4:
			bdrbox=box(-8,-8,8,8);
			break;
		case 6:
			bdrbox=box(-12,-12,12,12);
			break;
		case 7:
			bdrbox=box(-10,-10,10,10);
			break;
		default:
			// Default is 2 pixels.
			int xeigfactor=0;
			int yeigfactor=0;
			os::OS_ReadModeVariable(swi::XEigFactor,&xeigfactor);
			os::OS_ReadModeVariable(swi::YEigFactor,&yeigfactor);
			unsigned int xpix=1<<xeigfactor;
			unsigned int ypix=1<<yeigfactor;
			bdrbox=box(-xpix,-ypix,xpix,ypix);
			break;
		}
	}
	return bdrbox;
}

} /* anonymous namespace */

icon::icon():
	_handle(0),
	_itype(empty_icon),
	_created(false),
	_text_and_sprite(false),
	_border(false),
	_hcentre(false),
	_vcentre(false),
	_fill(false),
	_has_font(false),
	_rjustify(false),
	_adjust_select(false),
	_half_size(false),
	_button(0),
	_esg(0),
	_selected(false),
	_enabled(true),
	_fcolour(7),
	_bcolour(1),
	_font(0)
{}

icon::~icon()
{
	remove();
}

box icon::auto_bbox() const
{
	// Auto bounding box = content + border.
	box abbox=content_box()+border_box();

	// Translate to external origin and return.
	abbox-=external_origin(abbox,xbaseline_text,ybaseline_text);
	return abbox;
}

box icon::bbox() const
{
	return _bbox;
}

void icon::reformat(const point& origin,const box& pbbox)
{
	// Fit bounding box to parent.
	box bbox=fit(pbbox);

	// Update origin and bounding box of this component, force
	// redraw if necessary.
	bool moved=(origin!=this->origin())||(bbox!=this->bbox());
	if (moved&&_created) force_redraw(true);
	_bbox=bbox;
	inherited::reformat(origin,bbox);
	if (moved||!_created) force_redraw(_created);

	// Calculate offset from origin of work area to origin of this
	// component.
	point offset;
	basic_window* w=parent_work_area(offset);
	int whandle=(w)?w->handle():-1;

	if (!_created)
	{
		// Icon has not yet been created.
		static os::icon_create block;
		block.icon.bbox=_bbox+offset;
		block.whandle=whandle;
		block.icon.flags=icon_flags();
		block.icon.data=icon_data();

		os::Wimp_CreateIcon(0,block,&_handle);
		if (w)
		{
			w->register_icon(*this);
		}
		else if (application* app=parent_application())
		{
			app->register_icon(*this);
		}
		_created=true;
	}
	else
	{
		// Icon has been created.
		// Get current position (with respect to the work area),
		// then compare with required position.
		static os::icon_state_get block;
		block.whandle=whandle;
		block.ihandle=_handle;
		os::Wimp_GetIconState(block);

		box bbox=_bbox+offset;
		if (bbox!=block.icon.bbox)
		{
			// Icon has moved (with respect to the work area).
			os::Wimp_ResizeIcon(whandle,_handle,bbox);
		}
	}
}

void icon::unformat()
{
	if (_created)
	{
		basic_window* w=parent_work_area();
		if (w)
		{
			w->deregister_icon(*this);
		}
		else if (application* app=parent_application())
		{
			app->deregister_icon(*this);
		}
		os::icon_delete block;
		block.whandle=(w)?w->handle():-1;
		block.ihandle=_handle;
		os::Wimp_DeleteIcon(block);
		_handle=0;
		_created=false;
	}
}

icon& icon::text(const string& text,size_type capacity)
{
	size_type length=text.length();
	if (capacity<length) capacity=length;
	change_type(text_icon);
	if (capacity>_textsize)
	{
		unformat();
		invalidate();
		delete[] _text;
		_text=0;
		_textsize=0;
		_text=new char[capacity+1];
		_textsize=capacity;
	}
	if (_text)
	{
		size_type i=text.copy(_text,_textsize);
		_text[i]=0;
	}
	force_redraw();
	return *this;
}

icon& icon::validation(const string& validation,size_type capacity)
{
	size_type length=validation.length();
	if (capacity<length) capacity=length;
	change_type(text_icon);
	if (capacity>_valsize)
	{
		unformat();
		delete[] _val;
		_val=0;
		_valsize=0;
		_val=new char[capacity+1];
		_valsize=capacity;
	}
	if (_val)
	{
		size_type i=validation.copy(_val,_valsize);
		_val[i]=0;
	}
	force_redraw();
	return *this;
}

icon& icon::sprite_name(const string& sprite_name,size_type capacity)
{
	size_type length=sprite_name.length();
	if (capacity<length) capacity=length;
	change_type(named_sprite_icon);
	if (capacity>_namesize)
	{
		unformat();
		delete[] _name;
		_name=0;
		_namesize=0;
		_name=new char[capacity+1];
		_namesize=capacity;
	}
	if (_name)
	{
		size_type i=sprite_name.copy(_name,_namesize);
		_name[i]=0;
	}
	force_redraw();
	return *this;
}

icon& icon::text_and_sprite(bool value)
{
	_text_and_sprite=value;
	set_state();
	force_redraw();
	invalidate();
	return *this;
}

icon& icon::border(bool value)
{
	_border=value;
	set_state();
	force_redraw();
	invalidate();
	return *this;
}

icon& icon::hcentre(bool value)
{
	_hcentre=value;
	set_state();
	force_redraw();
	invalidate();
	return *this;
}

icon& icon::vcentre(bool value)
{
	_vcentre=value;
	set_state();
	force_redraw();
	invalidate();
	return *this;
}

icon& icon::fill(bool value)
{
	_fill=value;
	set_state();
	force_redraw();
	return *this;
}

icon& icon::rjustify(bool value)
{
	_rjustify=value;
	set_state();
	force_redraw();
	invalidate();
	return *this;
}

icon& icon::adjust_select(bool value)
{
	_adjust_select=value;
	set_state();
	return *this;
}

icon& icon::half_size(bool value)
{
	_half_size=value;
	set_state();
	force_redraw();
	invalidate();
	return *this;
}

icon& icon::button(int value)
{
	_button=value;
	set_state();
	return *this;
}

icon& icon::esg(int value)
{
	_esg=value;
	set_state();
	return *this;
}

icon& icon::selected(bool value)
{
	_selected=value;
	set_state();
	force_redraw();
	return *this;
}

icon& icon::enabled(bool value)
{
	_enabled=value;
	set_state();
	force_redraw();
	invalidate();
	return *this;
}

icon& icon::fcolour(int colour)
{
	_fcolour=colour;
	set_state();
	force_redraw();
	return *this;
}

icon& icon::bcolour(int colour)
{
	_bcolour=colour;
	set_state();
	force_redraw();
	return *this;
}

string icon::text() const
{
	return ((_itype==text_icon)&&_text)?string(_text):string();
}

string icon::validation() const
{
	return ((_itype==text_icon)&&_val)?string(_val):string();
}

string icon::sprite_name() const
{
	return ((_itype==named_sprite_icon)&&_name)?string(_name):string();
}

bool icon::selected() const
{
	bool selected=_selected;
	if (_created)
	{
		basic_window* w=parent_work_area();
		int whandle=(w)?w->handle():-1;
		os::icon_state_get block;
		block.whandle=whandle;
		block.ihandle=_handle;
		os::Wimp_GetIconState(block);
		selected=(block.icon.flags>>21)&1;
	}
	return selected;
}

void icon::deliver_wimp_block(int wimpcode,os::wimp_block& wimpblock)
{
	switch (wimpcode)
	{
	case 6:
		{
			events::mouse_click ev(*this,wimpblock);
			ev.post();
		}
		break;
	case 8:
		{
			events::key_pressed ev(*this,wimpblock);
			ev.post();
		}
		break;
	case 17:
	case 18:
		deliver_message(wimpcode,wimpblock);
		break;
	default:
		{ 
			events::wimp ev(*this,wimpcode,wimpblock);
			ev.post();
		}
		break;
	}
}

void icon::deliver_message(int wimpcode,os::wimp_block& wimpblock)
{
	switch (wimpblock.word[4])
	{
		case swi::Message_DataSave:
		{
			events::datasave ev(*this,wimpcode,wimpblock);
			ev.post();
		}
		break;
	case swi::Message_DataLoad:
		{
			events::dataload ev(*this,wimpcode,wimpblock);
			ev.post();
		}
		break;
	case swi::Message_HelpRequest:
		{
			events::help_request ev(*this,wimpblock);
			ev.post();
		}
		break;
	default:
		{
			events::message ev(*this,wimpcode,wimpblock);
			ev.post();
		}
		break;
	}
}

int icon::icon_flags() const
{
	int flags=0;
	switch (_itype)
	{
	case empty_icon:
		flags=0;
		break;
	case text_icon:
		flags=(1<<0)+(_text_and_sprite<<1)+(1<<8);
		break;
	case named_sprite_icon:
	case sprite_icon:
		flags=(1<<1)+(1<<8);
		break;
	}
	flags|=(_border<<2)|
		(_hcentre<<3)|
		(_vcentre<<4)|
		(_fill<<5)|
		(_has_font<<6)|
		(_rjustify<<9)|
		(_adjust_select<<10)|
		(_half_size<<11)|
		(_button<<12)|
		(_esg<<16)|
		(_selected<<21)|
		((1-_enabled)<<22)|
		(_has_font?(_font<<24):((_fcolour<<24)+(_bcolour<<28)));
	return flags;
}

os::icon_data icon::icon_data() const
{
	os::icon_data data;
	switch (_itype)
	{
	case empty_icon:
		data.word[0]=0;
		data.word[1]=0;
		data.word[2]=0;
	case text_icon:
		if (_text) data.pointer[0]=_text;
		else data.const_pointer[0]="";
		if (_val) data.pointer[1]=_val;
		else data.const_pointer[1]="";
		data.word[2]=_textsize+1;
		break;
	case named_sprite_icon:
		if (_name) data.pointer[0]=_name;
		else data.const_pointer[0]="";
		if (_area) data.pointer[1]=_area;
		else data.word[1]=1;
		data.word[2]=_namesize+1;
		break;
	case sprite_icon:
		data.pointer[0]=_sprite;
		if (_area) data.pointer[1]=_area;
		else data.word[1]=1;
		data.word[2]=0;
		break;
	}
	return data;
}

void icon::change_type(icon_type itype)
{
	// (Using operator== instead of operator!= to avoid potential
	// problem when operator!=<T>(T&,T&) has been declared.)
	if (!(itype==_itype))
	{
		if (_created) unformat();
		switch (_itype)
		{
		case empty_icon:
		case sprite_icon:
			break;
		case text_icon:
			delete[] _text;
			delete[] _val;
			break;
		case named_sprite_icon:
			delete[] _name;
			break;
		}
		_itype=itype;
		switch (_itype)
		{
		case empty_icon:
			break;
		case text_icon:
			_text=0;
			_textsize=0;
			_val=0;
			_valsize=0;
			break;
		case named_sprite_icon:
			_name=0;
			_namesize=0;
			_area=0;
			break;
		case sprite_icon:
			_sprite=0;
			_area=0;
			break;
		}
	}
}

void icon::set_state()
{
	if (_created)
	{
		basic_window* w=parent_work_area();
		int _whandle=(w)?w->handle():-1;
		os::icon_state_set block;
		block.whandle=_whandle;
		block.ihandle=_handle;
		block.eor=icon_flags();
		block.bic=-1;
		os::Wimp_SetIconState(block);
	}
}

box icon::content_box() const
{
	int xeigfactor=0;
	int yeigfactor=0;
	os::OS_ReadModeVariable(swi::XEigFactor,&xeigfactor);
	os::OS_ReadModeVariable(swi::YEigFactor,&yeigfactor);

	// Initialise dimensions and border type.
	int prefxsize=0;
	int prefysize=0;
	int descent=0;
	unsigned int border_type=0;

	switch (_itype)
	{
	case empty_icon:
		prefxsize=0;
		prefysize=0;
		break;
	case text_icon:
		char sprite0[16];
		char sprite1[16];
		*sprite0=0;
		*sprite1=0;
		if (_val)
		{
			char* p=_val;
			parse_validation(p,&border_type,sprite0,sprite1);
		}
		if (_text) os::Wimp_TextOp1(_text,0,&prefxsize);
		prefxsize+=16;
		prefysize=44;
		descent=4;
		if (_text_and_sprite==1)
		{
			int spxsize=0;
			int spysize=0;
			if (*sprite0!=0) sprite_size(0,sprite0,&spxsize,&spysize);
			else if (_text) sprite_size(0,_text,&spxsize,&spysize);
			spxsize<<=xeigfactor;
			spysize<<=yeigfactor;
			if (_half_size)
			{
				spxsize=(spxsize+1)>>1;
				spysize=(spysize+1)>>1;
			}
			switch ((_hcentre<<2)+(_vcentre<<1)+_rjustify)
			{
			case 0:
			case 1:
			case 4:
			case 5:
			case 6:
				if (spxsize>prefxsize) prefxsize=spxsize;
				break;
			case 2:
				prefxsize+=12+spxsize;
				break;
			case 3:
			case 7:
				prefxsize+=spxsize;
				break;
			}
			if (_vcentre) { if (spysize>prefysize) prefysize=spysize; }
			else prefysize+=spysize;
		}
		break;
	case named_sprite_icon:
		{
			int spxsize=0;
			int spysize=0;
			sprite_size(_area,_name,&spxsize,&spysize);
			spxsize<<=xeigfactor;
			spysize<<=yeigfactor;
			if (_half_size)
			{
				spxsize=(spxsize+1)>>1;
				spysize=(spysize+1)>>1;
			}
			prefxsize=spxsize;
			prefysize=spysize;
		}
		break;
	case sprite_icon:
		{
			int spxsize=0;
			int spysize=0;
			sprite_size(_area,_sprite,&spxsize,&spysize);
			spxsize<<=xeigfactor;
			spysize<<=yeigfactor;
			if (_half_size)
			{
				spxsize=(spxsize+1)>>1;
				spysize=(spysize+1)>>1;
			}
			prefxsize=spxsize;
			prefysize=spysize;
		}
		break;
	}

	// Round up to nearest pixel.
	prefxsize=((prefxsize+(1<<xeigfactor)-1)>>xeigfactor)<<xeigfactor;
	prefysize=((prefysize+(1<<yeigfactor)-1)>>yeigfactor)<<yeigfactor;

	// Return bounding box with respect to alpha baselines.
	return box(0,-descent,prefxsize,prefysize-descent);
}

box icon::border_box() const
{
	unsigned int border_type=0;
	if ((_itype==text_icon)&&_val)
	{
		char* p=_val;
		parse_validation(p,&border_type,0,0);
	}
	return make_border_box(_border,border_type);
}

} /* namespace desktop */
} /* namespace rtk */
