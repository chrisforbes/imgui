#pragma once

#include <SDL/SDL.h>

#define __noreturn __attribute__(( noreturn ))

/* from util.c */
extern void __noreturn die(int code, char const * p, ...);

/* core ui structures */
typedef int ui_id;

enum layout_mode { LM_LEFT, LM_TOP, LM_RIGHT, LM_BOTTOM, LM_FILL };

/* todo: redefine layout flags: left,top,right,bottom,fill align */

struct layout {
	struct layout * prev;	/* parent layout */
	int x, y, w, h;		/* bounds of this layout */
	int ipad;		/* internal padding between elements */
	enum layout_mode mode;	/* default child layout */
};

struct state {
	int x, y, buttons;	/* current input device state */
	ui_id hot, lasthot, active;	/* widget interaction state */
	struct layout * l;	/* current layout object */
	struct layout * sl;	/* spare layouts */
};

/* things provided by the client */
extern struct state uis;
extern SDL_Surface * surf;

/* from layout.c */
void ui_pushlayout(void);
void ui_poplayout(void);
void ui_toplevel(int x, int y);
void ui_float(int x, int y, enum layout_mode mode);
void ui_box_ex(int w, int h, enum layout_mode mode, enum layout_mode placement);
void ui_box(int w, int h, enum layout_mode mode);
void ui_do_layout(struct layout * l, enum layout_mode mode, int dx, int dy, SDL_Rect * r);
void ui_pad(int xpad, int ipad);

/* from widget.c */
int ui_inside(SDL_Rect * r);
int ui_button_ex(ui_id id, char const * label, enum layout_mode mode);
int ui_button(ui_id id, char const * label);
int ui_fill(int c);
void ui_begin(void);
void ui_end(void);

#define BUTTON_WIDTH 64
#define BUTTON_HEIGHT 48

/* from font.c */

struct font;
struct font * font_load( char const * name, int size );
int font_draw( struct font * f, int x, int y, char const * p, int c );
int font_measure( struct font * f, char const *p );

extern struct font * font;
