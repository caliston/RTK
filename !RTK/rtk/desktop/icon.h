// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_DESKTOP_ICON
#define _RTK_DESKTOP_ICON

#include <string>

#include "rtk/desktop/sizeable_component.h"

namespace rtk {
namespace os {

struct sprite;
struct sprite_area;
union wimp_block;
union icon_data;

}; /* namespace os */

namespace desktop {

/** A class to represent a RISC OS icon.
 * @todo Automatic sizing of text plus sprite icons is not yet implemented.
 */
class icon:
	public sizeable_component
{
private:
	/** The class from which this one is derived. */
	typedef sizeable_component inherited;
public:
	/** A type for representing the size of a buffer. */
	typedef unsigned int size_type;
private:
	/** An enumerated type to indicate one of four possible icon types.
	 * Every icon has three data words.  This type is used to specify
	 * what the first of those words is used for.
	 */
	enum icon_type
	{
		/** A constant which indicates that an icon contains neither
		 * text nor a sprite.
		 */
		empty_icon,
		/** A constant which indicates that an icon contains text (and
		 * possibly also a named sprite).
		 */
		text_icon,
		/** A constant which indicates that an icon contains a sprite
		 * (referred to by name).
		 */
		named_sprite_icon,
		/** A constant which indicates that an icon contains a sprite
		 * (referred to by pointer).
		 */
		sprite_icon
	};

	/** The RISC OS icon handle.
	 * If _created is false then _handle should be 0.
	 */
	int _handle;

	/** The current bounding box, with respect to the origin of
	 * of this component.
	 */
	box _bbox;

	/** A union used to hold the first word of the icon data block. */
	union
	{
		/** The current text buffer.
		 * This variable is meaningful when _itype==text_icon.  It may
		 * be null, in which case no buffer has been allocated.
		 */
		char* _text;
		/** The current sprite name buffer.
		 * This variable is meaningful when _itype==named_sprite_icon.
		 * It may be null, in which case no buffer has been allocated.
		 */
		char* _name;
		/** The current sprite pointer.
		 * This variable is meaningful when _itype==sprite_icon.
		 * It may be null, in which case no sprite has been specified.
		 */
		os::sprite* _sprite;
	};

	/** A union used to hold the second word of the icon data block. */
	union
	{
		/** The current validation string buffer.
		 * This variable is meaningful when _itype==text_icon.  It may
		 * be null, in which case no buffer has been allocated.
		 */
		char* _val;
		/** The current sprite area pointer.
		 * This variable is meaningful when _itype==named_sprite_icon,
		 * or itype==sprite_icon.
		 */
		os::sprite_area *_area;
	};

	/** A union used to hold the third word of the icon data block. */
	union
	{
		/** The current text buffer size.
		 * This variable is meaningful when _itype==text_icon.  It is
		 * the maximum number of characters that the buffer can hold,
		 * not counting the terminator.
		 */
		size_type _textsize;
		/** The current sprite name buffer size.
		 * This variable is meaningful when _itype==named_sprite_icon.
		 * It is the maximum number of characters that the buffer can
		 * hold, not counting the terminator.
		 */
		size_type _namesize;
	};

	/** The current validation string buffer size.
	 * This variable is meaningful when _itype==text_icon.  It is
	 * the maximum number of characters that the buffer can hold,
	 * not counting the terminator.
	 */
	size_type _valsize;

	/** The current icon type.
	 * See icon_type for a description of the allowed values.
	 */
	icon_type _itype:2;

	/** The icon created flag.
	 * True if a RISC OS icon exists for this component, otherwise false.
	 */
	bool _created:1;

	/** The text-and-sprite flag.
	 * If true, and if _itype==text_icon, then the icon will contain
	 * both text and a named sprite.  (The sprite name defaults to
	 * the content of the text buffer, but may be changed using the
	 * validation string.)
	 */
	bool _text_and_sprite:1;

	/** The border flag.
	 * True if the icon has a border, otherwise false.
	 */
	bool _border:1;

	/** The horizontal centre flag.
	 * True if the content of the icon is horizontally centred,
	 * otherwise false.
	 */
	bool _hcentre:1;

	/** The vertical centre flag.
	 * True if the content of the icon is vertically centred,
	 * otherwise false.
	 */
	bool _vcentre:1;

	/** The fill flag.
	 * True if the icon is filled, otherwise false.
	 */
	bool _fill:1;

	/** The has-font flag.
	 * True if the font handle in _font should be used, otherwise false.
	 */
	bool _has_font:1;

	/** The right justify flag.
	 * True if the content of the icon is right justified,
	 * otherwise false.
	 */
	bool _rjustify:1;

	/** The half size flag.
	 * True if the icon is to be shown at half size, otherwise false.
	 */
	bool _half_size:1;

	/** The button type.
	 */
	unsigned int _button:4;

	/** The exclusive selection group. */
	unsigned int _esg:5;

	/** The selected flag.
	 * True if the icon is selected, otherwise false.
	 */
	bool _selected:1;

	/** The enabled flag.
	 * True if the icon is enabled, otherwise false.
	 */
	bool _enabled:1;

	/** The foreground colour.
	 * This is one of the 16 standard Wimp colours.
	 */
	unsigned int _fcolour:4;

	/** The background colour.
	 * This is one of the 16 standard Wimp colours.
	 */
	unsigned int _bcolour:4;

	/** The anti-aliased font handle.
	 * This is used only if _has_font is true.
	 */
	unsigned int _font:8;
public:
	/** Construct icon.
	 * By default an icon:
	 * - contains neither text nor a sprite;
	 * - has no border;
	 * - is neither horizontally nor vertically centred;
	 * - is not filled;
	 * - is not right-justified;
	 * - is displayed at full size;
	 * - has a button type of 0;
	 * - is in exclusive selection group 0;
	 * - is not selected;
	 * - is enabled;
	 * - has a foreground colour of 7;
	 * - has a background colour of 1; and
	 * - uses the default desktop font.
	 */
	icon();

	/** Destroy icon. */
	virtual ~icon();

	virtual box auto_bbox() const;
	virtual box bbox() const;
	virtual void reformat(const point& origin,const box& pbbox);
	virtual void unformat();

	/** Get icon text.
	 * @return the icon text
	 */
	string text() const;

	/** Get validation string.
	 * @return the validation string
	 */
	string validation() const;

	/** Get sprite name.
	 * @return the sprite name
	 */
	string sprite_name() const;

	/** Get text-and-sprite flag.
	 * @return true if icon contains both text and a sprite, otherwise
	 *  false
	 */
	bool text_and_sprite() const
		{ return _text_and_sprite; }

	/** Get border flag.
	 * @return true if icon has a border, otherwise false
	 */
	bool border() const
		{ return _border; }

	/** Get horizontal centre flag.
	 * @return true if sprite is horizontally centred, otherwise false
	 */
	bool hcentre() const
		{ return _hcentre; }

	/** Get vertical centre flag.
	 * @return true if sprite is vertically centred, otherwise false
	 */
	bool vcentre() const
		{ return _vcentre; }

	/** Get fill flag.
	 * @return true if sprite background is filled, otherwise false
	 */
	bool fill() const
		{ return _fill; }

	/** Get right justification flag.
	 * @return true if sprite is right justified, otherwise false
	 */
	bool rjustify() const
		{ return _rjustify; }

	/** Get half size flag.
	 * @return true if icon is displayed at half size, otherwise false
	 */
	bool half_size() const
		{ return _half_size; }

	/** Get button type.
	 * @return the icon button type
	 */
	int button() const
		{ return _button; }

	/** Get selected flag.
	 * If true then the appearance of the icon will change to indicate
	 * that it is selected.
	 * @return true if icon is selected, otherwise false
	 */
	bool selected() const;

	/** Get enabled flag.
	 * If false then the icon will not be visible.
	 * @return true if icon is enabled, otherwise false
	 */
	bool enabled() const
		{ return _enabled; }

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

	/** Set icon text.
	 * This function automatically changes the icon type to text_icon.
	 * If the text is likely to change then its capacity should be set
	 * to the maximum number of characters likely to be needed.
	 * @param text the required icon text
	 * @param capacity the required capacity (defaults to length of
	 *  specified icon text)
	 * @return a reference to this
	 */
	icon& text(const string& text,size_type capacity=0);

	/** Set validation string.
	 * This function automatically changes the icon type to text_icon.
	 * If the validation string is likely to change then its capacity
	 * should be set to the maximum number of characters likely to be needed.
	 * @param validation the required validation string
	 * @param capacity the required capacity (defaults to length of
	 *  specified validation string)
	 * @return a reference to this
	 */
	icon& validation(const string& validation,size_type capacity=0);

	/** Set sprite name.
	 * This function automatically changes the icon type to
	 * named_sprite_icon.
	 * If the sprite name is likely to change then its capacity should
	 * be set to the maximum number of characters likely to be needed.
	 * @param sprite_name the required sprite name
	 * @param capacity the required capacity (defaults to length of
	 *  specified sprite name)
	 * @return a reference to this
	 */
	icon& sprite_name(const string& sprite_name,size_type capacity=0);

	/** Set text-and-sprite flag.
	 * @param value true if the icon is to contain both text and a sprite,
	 *  otherwise false.
	 * @return a reference to this
	 */
	icon& text_and_sprite(bool value);

	/** Set border flag.
	 * @param value true if the icon is to have a border, otherwise false
	 * @return a reference to this
	 */
	icon& border(bool value);

	/** Set horizontal centre flag.
	 * @param value true if the icon is to be horizontally centred, otherwise
	 *  false
	 * @return a reference to this
	 */
	icon& hcentre(bool value);

	/** Set vertical centre flag.
	 * @param value true if the icon is to be vertically centred, otherwise
	 *  false
	 * @return a reference to this
	 */
	icon& vcentre(bool value);

	/** Set fill flag.
	 * @param value true if the icon background is to be filled, otherwise
	 *  false
	 * @return a reference to this
	 */
	icon& fill(bool value);

	/** Set right-justification flag.
	 * @param value true if the icon is to be right-justified, otherwise false
	 * @return a reference to this
	 */
	icon& rjustify(bool value);

	/** Set halfsize flag.
	 * @param value true if the icon is to be displayed at half size,
	 *  otherwise false
	 * @return a reference to this
	 */
	icon& half_size(bool value);

	/** Set button type.
	 * @param value the required button type
	 * @return a reference to this
	 */
	icon& button(int value);

	/** Set selected flag.
	 * If true then the appearance of the icon will change to indicate
	 * that it is selected.
	 * @param value true if the icon is to be selected, otherwise false
	 * @return a reference to this
	 */
	icon& selected(bool value);

	/** Set enabled flag.
	 * If false then the icon will not be visible.
	 * @param value true if the icon is to be enabled, otherwise false
	 * @return a reference to this
	 */
	icon& enabled(bool value);

	/** Set foreground colour.
	 * This is one of the 16 standard Wimp colours.
	 * @param fcolour the required foreground colour
	 * @return a reference to this
	 */
	icon& fcolour(int fcolour);

	/** Set background colour.
	 * This is one of the 16 standard Wimp colours.
	 * @param bcolour the required background colour
	 * @return a reference to this
	 */
	icon& bcolour(int bcolour);

	/** Deliver Wimp event block.
	 * @internal
	 * The event block is converted into a suitable event object and
	 * posted to the appropriate target.  This function should be
	 * used when it has been established that this icon is the target.
	 * @param wimpcode the Wimp event code
	 * @param wimpblock the Wimp event block
	 */
	void deliver_wimp_block(int wimpcode,os::wimp_block& wimpblock);

	/** Deliver Wimp message.
	 * @internal
	 * The event block (which should contain a message) is converted
	 * into a suitable event object and posted to the appropriate target.
	 * This function should be used when it has been established that
	 * this icon is the target.
	 * @param wimpcode the Wimp event code
	 * @param wimpblock the Wimp event block
	 */
	void deliver_message(int wimpcode,os::wimp_block& wimpblock);

	/** Get icon handle.
	 * Be aware that this class assumes that it has full control over
	 * any RISC OS icons that it creates.  It may override, or fail to
	 * take account of, changes that are made directly using the icon
	 * handle.
	 * @return the icon handle
	 */
	int handle() const
		{ return _handle; }

	/** Get icon flags.
	 * @return icon flags for use by the Wimp
	 */
	int icon_flags() const;

	/** Get icon data.
	 * @return icon data for use by the Wimp
	 */
	os::icon_data icon_data() const;
private:
	/** Change icon type.
	 * See icon_type for a description of the allowed values.
	 * If necessary the existing RISC OS icon is deleted, to be
	 * re-created by reformat().
	 * @param itype the required icon type.
	 */
	void change_type(icon_type itype);

	/** Set icon state.
	 * The RISC OS icon state is updated to match the result of
	 * icon_flags().
	 */
	void set_state();

	/** Parse spaces in validation string.
	 * Skip any number of spaces in the validation string.
	 * @param i reference to validation string iterator
	 */
	static void parse_spaces(char*& i);

	/** Parse semicolon in validation string.
	 * Search for a semicolon in the validation string then skip past it.
	 * Any preceeding characters are ignored.
	 * @param i reference to validation string iterator
	 */
	static void parse_semicolon(char*& i);

	/** Parse unsigned integer in validation string.
	 * Parse unsigned integer.  Stop at first non-digit.
	 * If no digits are found then the result will be zero.
	 * @param i reference to validation string iterator
	 * @param value buffer for returned integer
	 */
	static void parse_int(char*& i,unsigned int* value);

	/** Parse string in validation string.
	 * Parse string terminated by comma or semicolon.  Stop at but
	 * do not skip past terminator.  The result string is
	 * null-terminated.
	 * @param i reference to validation string iterator
	 * @param buffer buffer for returned string
	 * @param size size of result buffer, excluding terminator
	 */
	static void parse_string(char*& i,char* buffer,size_type size);

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
	static void parse_validation(char*& i,unsigned int* border_type,
		char* sprite0,char* sprite1);

	/** Get size of sprite, given name.
	 * @param area the sprite area
	 * @param name the sprite name
	 * @param _xsize a buffer for the returned width
	 * @param _ysize a buffer for the returned height
	 */
	static void sprite_size(os::sprite_area* area,const char* name,
		int* _xsize,int* _ysize);

	/** Get size of sprite, given pointer.
	 * @param area the sprite area
	 * @param name the sprite
	 * @param _xsize a buffer for the returned width
	 * @param _ysize a buffer for the returned height
	 */
	static void sprite_size(os::sprite_area* area,os::sprite* sprite,
		int* _xsize,int* _ysize);
};

}; /* namespace desktop */
}; /* namespace rtk */

#endif
