#pragma once

#include <SDL/SDL.h>

#define __noreturn __attribute__(( noreturn ))

/* from util.c */
extern void __noreturn die(int code, char const * p, ...);

/* core ui structures */
typedef int ui_id;

#define LAYOUT_VBOX	1
#define LAYOUT_HBOX	2

struct layout {
	struct layout * prev;	/* parent layout */
	int x, y, w, h;		/* bounds of this layout */
	int ux, uy;		/* offset into the layout */
	int flags;		/* layout style flags */
	int ipad;		/* internal padding between elements */
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
void ui_float(int x, int y);
void ui_hbox(int h);
void ui_vbox(int w);
void ui_do_layout(struct layout * l, int dx, int dy, SDL_Rect * r);
void ui_pad(int xpad, int ipad);

/* from widget.c */
int ui_inside(SDL_Rect * r);
int ui_button(ui_id id, char const * label);
int ui_fill(int c);

#define BUTTON_WIDTH 64
#define BUTTON_HEIGHT 48
