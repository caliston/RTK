// This file is part of the RISC OS Toolkit (RTK).
// Copyright © 2003 Graham Shaw.
// Distribution and use are subject to the GNU Lesser General Public License,
// a copy of which may be found in the file !RTK.Copyright.

#ifndef _RTK_OS_WIMP
#define _RTK_OS_WIMP

#include "kernel.h"

#include "rtk/graphics/point.h"
#include "rtk/graphics/box.h"

namespace rtk {
namespace os {

using rtk::graphics::point;
using rtk::graphics::box;

struct sprite;
struct sprite_area;

/** A union to represent a Wimp event block. */
union wimp_block
{
	/** The event block as an array of bytes. */
	char byte[256];
	/** The event block as an array of words. */
	int word[64];
};

/** A union to represent a Wimp icon data block. */
union icon_data
{
	/** The icon data as an array of words. */
	int word[3];
	/** The icon data as an array of pointers. */
	void* pointer[3];
	/** The icon data as an array of const pointers. */
	const void* const_pointer[3];
};

/** A structure to represent the state of an icon. */
struct icon
{
	/** The bounding box. */
	box bbox;
	/** The icon flags. */
	int flags;
	/** The icon data. */
	icon_data data;
};

/** A structure for specifying a window handle and an icon handle. */
struct icon_base
{
	/** The window handle. */
	int whandle;
	/** The icon handle. */
	int ihandle;
};

/** A structure for use by Wimp_CreateIcon. */
struct icon_create
{
	/** The window handle. */
	int whandle;
	/** The icon state. */
	os::icon icon;
};

/** A structure for use by Wimp_DeleteIcon. */
struct icon_delete:
	public icon_base
{};

/** A structure for use by Wimp_GetIconState. */
struct icon_state_get:
	public icon_base
{
	/** The icon state. */
	os::icon icon;
};

/** A structure for use by Wimp_SetIconState. */
struct icon_state_set:
	public icon_base
{
	/** The exclusive-or mask. */
	int eor;
	/** The bit-inverse-clear mask. */
	int bic;
};

/** A structure for use by Wimp_GetPointerInfo. */
struct pointer_info_get
{
	/** The pointer position. */
	point p;
	/** The mouse button state. */
	int buttons;
	/** The handle of the window containing the pointer.
	 * (-1 for backdrop, -2 for icon bar.)
	 */
	int whandle;
	/** The handle of the icon containing the pointer.
	 * (-1 for work area, -2 to -13 for window furniture: see PRM.)
	 */
	int ihandle;
};

/** A structure for use by Wimp_GetCaretPosition. */
struct caret_position_get:
	public icon_base
{
	/** The caret position. */
	point p;
	/** The caret height. */
	int height;
	/** The character index. */
	int index;
};

/** A window block containing word 0 only. */
struct window_base_1
{
	/** The window handle. */
	int handle;
};

/** A window block containing words 0 to 6 inclusive. */
struct window_base_7:
	public window_base_1
{
	/** The visible area bounding box. */
	box bbox;
	/** The scroll offset. */
	point scroll;
};

/** A window block containing words 0 to 7 inclusive. */
struct window_base_8:
	public window_base_7
{
	/** The handle of the window about this one in the stack.
	 * (-1 for top, -2 for bottom.)
	 */
	int behind;
};

/** A window block containing words 0 to 8 inclusive. */
struct window_base_9:
	public window_base_8
{
	/** The window flags. */
	int wflags;
};

/** A structure for use by Wimp_CreateWindow. */
struct window_create
{
	/** The visible area bounding box. */
	box bbox;
	/** The scroll offset. */
	point scroll;
	/** The handle of the window about this one in the stack.
	 * (-1 for top, -2 for bottom.)
	 */
	int behind;
	/** The window flags. */
	int wflags;
	/** The title foreground colour. */
	char tf_colour;
	/** The title background colour. */
	char tb_colour;
	/** The work area foreground colour. */
	char wf_colour;
	/** The work area background colour. */
	char wb_colour;
	/** The outer scroll bar colour. */
	char so_colour;
	/** The inner scroll bar colour. */
	char si_colour;
	/** The title background colour when highlighted. */
	char th_colour;
	/** Reserved (must be 0). */
	char reserved;
	/** The work area extent. */
	box workarea;
	/** The title bar icon flags. */
	int tflags;
	/** The work area flags (giving the button type). */
	int waflags;
	/** The sprite area pointer. */
	int spritearea;
	/** The minimum width of the window. */
	int minxsize:16;
	/** The minimum height of the window. */
	int minysize:16;
	/** The title bar icon data. */
	icon_data title;
	/** The number of icons. */
	int numicons;
};

/** A structure for use by Wimp_DeleteWindow. */
struct window_delete:
	public window_base_1
{};

/** A structure for use by Wimp_OpenWindow. */
struct window_open:
	public window_base_8
{};

/** A structure for use by Wimp_CloseWindow. */
struct window_close:
	public window_base_1
{};

/** A structure for use by Wimp_GetWindowState. */
struct window_state_get:
	public window_base_9
{};


/** A structure for use by Wimp_GetRectangle and related functions. */
struct window_redraw:
	public window_base_7
{
	/** The current clip box. */
	box clip;
};

/** A structure to represent a RISC OS menu item. */
struct menu_item
{
	/** The menu flags. */
	int mflags;
	/** The submenu pointer or dialogue box handle. */
	int submenu;
	/** The icon flags. */
	int iflags;
	/** The icon data. */
	icon_data icon;
};

/** A structure to represent a RISC OS menu.
 * The structure defined here contains one menu item.  Use size_of
 * and casts to create menus with different lengths.
 */
struct menu
{
	/** The icon data for the title. */
	icon_data title;
	/** The title foreground colour. */
	char tf_colour;
	/** The title background colour. */
	char tb_colour;
	/** The work area foreground colour. */
	char wf_colour;
	/** The work area background colour. */
	char wb_colour;
	/** The width of the menu. */
	int width;
	/** The height of each menu item. */
	int height;
	/** The gap between menu items. */
	int gap;
	/** The first menu item. */
	menu_item item[1];
};

/** A structure for use by Wimp_DragBox. */
struct drag_box
{
	int whandle;
	int type;
	box dbox;
	box pbox;
	int r12;
	int draw_func;
	int remove_func;
	int move_func;
};

/** Register a task with the window manager.
 * @param version the most recent window manager version number known to
 *  task, multiplied by 100
 * @param name the task name (null terminated)
 * @param messages a pointer to a null-terminated list of messages that
 *  the task wishes to receive
 * @param _version a buffer for the returned window manager version
 * @param _handle a buffer for the returned task handle
 */
void Wimp_Initialise(int version,const char* name,int* messages,
	int* _version,int* _handle);

/** Create window.
 * @param block the window block
 * @param _handle a buffer for the returned window handle
 */
void Wimp_CreateWindow(const window_create& block,int* _handle);

/** Create icon.
 * @param priority the icon handle or priority
 * @param block the icon block
 * @param _handle a buffer for the returned icon handle
 */
void Wimp_CreateIcon(int priority,const icon_create& block,int* _handle);

/** Delete window.
 * @param block the window block
 */
void Wimp_DeleteWindow(const window_delete& block);

/** Delete icon.
 * @param block the icon block
 */
void Wimp_DeleteIcon(const icon_delete& block);

/** Open window.
 * @param block the window block
 */
void Wimp_OpenWindow(const window_open& block);

/** Open window (nested).
 * @param block the window block
 * @param phandle the parent window handle (-1 if none)
 * @param flags the nesting flags
 */
void Wimp_OpenWindow(const window_open& block,int phandle,int flags);

/** Close window.
 * @param block the window block
 */
void Wimp_CloseWindow(const window_close& block);

/** Poll Wimp.
 * @param mask the event mask
 * @param block the event block
 * @param pollword the pollword
 * @param _code a buffer for the returned event code
 */
void Wimp_Poll(int mask,wimp_block& block,int* pollword,int* _code);

/** Begin redraw of window (in response to Wimp_Poll).
 * @param block the redraw block
 * @param _more a buffer for the returned control flag (true if more to do,
 *  otherwise false)
 */
void Wimp_RedrawWindow(window_redraw& block,int* _more);

/** Begin redraw of window (at any time).
 * @param block the redraw block
 * @param _more a buffer for the returned control flag (true if more to do,
 *  otherwise false)
 */
void Wimp_UpdateWindow(window_redraw& block,int* _more);

/** Get next rectangle for redraw.
 * @param block the redraw block
 * @param _more a buffer for the returned control flag (true if more to do,
 *  otherwise false)
 */
void Wimp_GetRectangle(window_redraw& block,int* _more);

/** Get window state.
 * @param block the window block
 */
void Wimp_GetWindowState(window_state_get& block);

/** Set icon state.
 * @param block the icon block
 */
void Wimp_SetIconState(icon_state_set& block);

/** Get icon state.
 * @param block the icon block
 */
void Wimp_GetIconState(icon_state_get& block);

/** Get position of pointer and state of mouse buttons.
 * @param block the pointer information block
 */
void Wimp_GetPointerInfo(pointer_info_get& block);

/** Begin drag operation.
 * @param block the drag box box
 */
void Wimp_DragBox(drag_box& block);

/** Force area of screen or window to be redrawn.
 * @param handle the window handle (-1 for desktop, -2 for icon bar)
 * @param bbox the area to be redrawn
 */
void Wimp_ForceRedraw(int handle,const box& bbox);

/** Set caret position.
 * @param whandle the window handle (-1 to disown caret)
 * @param ihandle the icon handle (-1 if none)
 * @param p the required caret position
 * @param height the required caret height (-1 if using index)
 * @param index the required character index (-1 if not specified)
 */
void Wimp_SetCaretPosition(int whandle,int ihandle,const point& p,
	int height,int index);

/** Get caret position.
 * @param block the caret position block
 */
void Wimp_GetCaretPosition(caret_position_get& block);

/** Create top-level dialogue box.
 * @param handle the window handle
 * @param p the required position of the top left-hand corner of
 *  the visible area
 */
void Wimp_CreateMenu(int handle,const point& p);

/** Create top-level menu.
 * @param m the menu data
 * @param p the required position of the top left-hand corner of
 *  the visible area
 */
inline void Wimp_CreateMenu(os::menu* m,const point& p)
	{ Wimp_CreateMenu((int)m,p); }

/** Set extent of window.
 * @param handle the window handle
 * @param workarea the required dimensions of the work area
 */
void Wimp_SetExtent(int handle,const box& workarea);

/** Create or pass on keypress.
 * @param code the character code
 */
void Wimp_ProcessKey(int code);

/** Start child task.
 * @param command the command to be executed (null terminated)
 * @param _handle buffer for returned task handle (0 if none)
 */
void Wimp_StartTask(const char* command,int* _handle);

/** Report error.
 * @param block the RISC OS error block
 * @param title the application name (null terminated, for use
 *  in the window title)
 * @param flags the error dialogue flags
 * @param _result a buffer for the response (0=none, 1=OK, 2=cancel)
 */
void Wimp_ReportError(const _kernel_oserror& block,const char* title,
	int flags,int* _result);

/** Report error.
 * @param errnum the RISC OS error number
 * @param message the error message (null terminated)
 * @param title the application name (null terminated, for use in
 *  the window title)
 * @param flags the error dialogue flags
 * @param _result a buffer for the response (0=none, 1=OK, 2=cancel)
 */
void Wimp_ReportError(int errnum,const char* message,const char* title,
	int flags,int* _result);

/** Set graphics colour to a standard Wimp colour.
 * @param colour the required standard colour (bits 0-3), GCOL action
 *  (bits 4-6) and foreground (bit 7=0) or background (bit 7=1) flag
 */
void Wimp_SetColour(int colour);

/** Send message.
 * @param wimpcode the Wimp event code
 * @param block the message
 * @param thandle the destination task handle or window handle
 *  (-2=icon bar)
 * @param ihandle the icon handle (if thandle==-2)
 * @param _thandle a buffer for the returned task handle
 */
void Wimp_SendMessage(int wimpcode,wimp_block& block,int thandle,
	int ihandle,int* _thandle);

/** Create dialogue box within menu tree.
 * @param handle the window handle
 * @param p the required position of the top left-hand corner of
 *  the visible area
 */
void Wimp_CreateSubMenu(int handle,const point& p);

/** Create sub-menu.
 * @param m the menu data
 * @param p the required position of the top left-hand corner of
 *  the visible area
 */
inline void Wimp_CreateSubMenu(os::menu* m,const point& p)
	{ Wimp_CreateSubMenu((int)m,p); }

/** Read sprite information.
 * This function is equivalent to OS_SpriteOp40, but is limited to
 * sprites in the Wimp sprite pool.
 * @param name the null-terminated sprite name
 * @param _xsize a buffer for the returned width in pixels
 * @param _ysize a buffer for the returned height in pixels
 * @param _mask a buffer for the returned mask flag
 * @param _mode a buffer for the returned sprite mode
 */
void Wimp_SpriteOp40(const char* name,int* _xsize,int* _ysize,
	int* _mask,int* _mode);

/** Read pixel translation table for sprite (by name).
 * @param area the sprite area that contains the sprite (0 for system
 *  area, 1 for Wimp sprite area)
 * @param name the name of the sprite
 * @param _scale a buffer for the returned scale factors (4 words)
 * @param _table a buffer for the returned translation table
 *  (2, 4 or 16 bytes)
 */
void Wimp_ReadPixTrans(sprite_area* area,const char* name,
	unsigned int* _scale,unsigned char* _table);

/** Read pixel translation table for sprite (by pointer).
 * @param area the sprite area that contains the sprite (0 for system
 *  area)
 * @param sp the sprite pointer
 * @param _scale a buffer for the returned scale factors (4 words)
 * @param _table a buffer for the returned translation table
 *  (2, 4 or 16 bytes)
 */
void Wimp_ReadPixTrans(sprite_area* area,sprite* sp,
	unsigned int* _scale,unsigned char* _table);

/** Set text colour to a standard Wimp colour.
 * @param colour the required standard colour (bits 0-3), and foreground
 *  (bit 7=0) or background (bit 7=1) flag
 */
void Wimp_TextColour(int colour);

/** Transfer data from one task to another.
 * @param src_thandle the source task handle
 * @param src_buffer the source buffer
 * @param dst_thandle the destination task handle
 * @param dst_buffer the destination buffer
 * @param count the number of bytes to transfer
 */
void Wimp_TransferBlock(int src_thandle,const void* src_buffer,
	int dst_thandle,void* dst_buffer,unsigned int count);

/** Read system information.
 * @param index an index to select the required information
 * @param _r0 a buffer for the value returned in R0
 * @param _r1 a buffer for the value returned in R1 (if any)
 */
void Wimp_ReadSysInfo(int index,int* _r0,int* _r1);

/** Get menu state.
 * @param buffer a buffer for the returned menu tree
 */
void Wimp_GetMenuState(int* buffer);

/** Get menu state.
 * @param whandle the window handle
 * @param ihandle the icon handle
 * @param buffer a buffer for the returned menu tree
 */
void Wimp_GetMenuState(int whandle,int ihandle,int* buffer);

/** Set colours for use by Wimp_TextOp2.
 * @param fcolour the foreground palette entry
 * @param bcolour the background palette entry
 */
void Wimp_TextOp0(int fcolour,int bcolour);

/** Determine width required to plot string using desktop font.
 * @param s the string to be plotted (control character terminated)
 * @param count the number of characters to include
 * @param _width a buffer for the returned width
 */
void Wimp_TextOp1(const char* s,int count,int* _width);

/** Plot string to screen using desktop font.
 * @param s the string to plot (null terminated)
 * @param p the point at which to begin plotting
 */
void Wimp_TextOp2(const char* s,const point& p);

/** Resize or move icon.
 * @param whandle the window handle (-1 for iconbar)
 * @param ihandle the icon handle
 * @param bbox the required bounding box
 */
void Wimp_ResizeIcon(int whandle,int ihandle,const box& bbox);

}; /* namespace os */
}; /* namespace rtk */

#endif
