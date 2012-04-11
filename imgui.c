#include <stdio.h>
#include <stdarg.h>
#include <SDL/SDL.h>

/* imgui
 *	- very simple widgets
 *	- no 'widget tree' state retained in ui lib, that's the app's problem
 *	- automated layout and metrics as much as possible
 *		(although multipass layout requires app cooperation)
 */

typedef int ui_id;

#define LAYOUT_VBOX	1
#define LAYOUT_HBOX	2

struct layout {
	struct layout * prev;	/* parent layout */
	int x, y, w, h;		/* bounds of this layout */
	int ux, uy;		/* offset into the layout */
	int flags;		/* layout style flags */
};

struct state {
	int x, y, buttons;	/* current input device state */
	ui_id hot, lasthot, active;	/* widget interaction state */
	struct layout * l;	/* current layout object */
	struct layout * sl;	/* spare layouts */
} uis = { 0 };

#define __noreturn __attribute__(( noreturn ))

void __noreturn die(int code, char const * p, ...) {
	va_list vl;
	va_start(vl, p);
	vfprintf(stderr, p, vl);
	fputc('\n', stderr);
	va_end(vl);

	fflush(stderr);
	exit(code);
}

static int windowWidth = 640;
static int windowHeight = 480;
static char const * const windowTitle = "IMGUI Test";
SDL_Surface * surf;
static int c_background = 0xff0000ff;
static int c_shadow = 0xff000000;
static int c_normal = 0xffcccccc;
static int c_hilite = 0xffeeeeee;
static int c_pushed = 0xff888888;

int ui_in(SDL_Rect * r, int x, int y) {
	return x >= r->x && x < r->x + r->w &&
		y >= r->y && y < r->y + r->h;
}

/* carve out some space from a layout */
void ui_do_layout(struct layout * l, int dx, int dy, SDL_Rect * r) {
	r->x = l->x + l->ux;
	r->y = l->y + l->uy;
	r->w = (l->flags & LAYOUT_VBOX) ? l->w : dx;
	r->h = (l->flags & LAYOUT_HBOX) ? l->h : dy;

	if (l->flags & LAYOUT_VBOX) l->uy += dy;
	if (l->flags & LAYOUT_HBOX) l->ux += dx;

	/* todo: what happens if the element doesn't actually fit?
	 * in some cases, it's sensible to spill into another row/column */
}

static int button_width = 64;
static int button_height = 48;

/* draw a button */
int ui_button(ui_id id, char const * label) {
	SDL_Rect r;
	ui_do_layout( uis.l, button_width, button_height, &r );
	if (ui_in(&r, uis.x, uis.y))
		uis.hot = id;

	if (uis.active == id && !uis.buttons) {
		uis.active = 0;
		return 1;
	}

	if (uis.lasthot == id) {
		if (uis.buttons) {
			SDL_FillRect( surf, &r, c_pushed );
			uis.active = id;
			return 0;
		}
		else {
			SDL_FillRect( surf, &r,
				uis.hot == id ? c_hilite : c_normal );
			return 0;
		}
	}

	SDL_FillRect( surf, &r, c_normal );

	return 0;
}

/* fill the background of the current layout */
int ui_fill(int c) {
	SDL_Rect r = { uis.l->x, uis.l->y, uis.l->w, uis.l->h };
	SDL_FillRect( surf, &r, c );
}

void ui_pushlayout(void) {
	if (!uis.sl) /* there is no spare layout, alloc one */
		uis.sl = calloc(1, sizeof(*uis.sl));

	/* shuffle a layout from the spare list to the active list,
	 * fixing up the prev links */
	struct layout * temp = uis.sl->prev;
	uis.sl->prev = uis.l;
	uis.l = uis.sl;
	uis.sl = temp;
}

void ui_poplayout(void) {
	if (!uis.l) /* it is an error to call this with no current layout */
		die(1, "ui_poplayout called with no layout");

	/* shuffle a layout back onto the spare list */
	struct layout * temp = uis.l->prev;
	uis.l->prev = uis.sl;
	uis.sl = uis.l;
	uis.l = temp;
}

/* toplevel layout */
void ui_toplevel(int w, int h) {
	ui_pushlayout();
	uis.l->ux = 0;
	uis.l->uy = 0;
	uis.l->x = 0;
	uis.l->y = 0;
	uis.l->w = w;
	uis.l->h = h;
}

/* a centered floating panel */
void ui_float(int w, int h) {
	ui_pushlayout();
	uis.l->ux = 0;
	uis.l->uy = 0;
	uis.l->w = w;
	uis.l->h = h;
	uis.l->x = uis.l->prev->x + (uis.l->prev->w - w)/2;
	uis.l->y = uis.l->prev->y + (uis.l->prev->h - h)/2;
}

void ui_box_helper(int w, int h, int flags) {
	SDL_Rect r;
	ui_do_layout( uis.l, w, h, &r );
	ui_pushlayout();
	uis.l->ux = 0;
	uis.l->uy = 0;
	uis.l->x = r.x;
	uis.l->y = r.y;
	uis.l->w = r.w;
	uis.l->h = r.h;
	uis.l->flags = flags;
}

/* stacks elements horizontally */
void ui_hbox(int h) { ui_box_helper( 0, h, LAYOUT_HBOX ); }
/* stacks elements vertically */
void ui_vbox(int w) { ui_box_helper( w, 0, LAYOUT_VBOX ); }

void ui_begin(void) {
	uis.hot = 0;
	/* push a toplevel layout so the layout engine
	 * knows how big the display surface is */
	ui_toplevel(windowWidth, windowHeight);
}

void ui_end(void) {
	uis.lasthot = uis.hot;

	/* pop the (hopefully) toplevel layout */
	ui_poplayout();
	
	/* it is an error to leave other layouts pushed */
	if (uis.l)
		die(1, "ui_end called with unfinished layouts");
}

/* local ui state */

void draw(void) {
	SDL_FillRect(surf, 0, c_background);
	ui_begin();

	ui_float(300,300);
	ui_fill(0xff444444);	/* window background */

	ui_hbox(button_height);
	
	if (ui_button(1, "Toggle Red Channel"))
		c_background ^= 0x00ff0000;

	if (ui_button(2, "Toggle Green Channel"))
		c_background ^= 0x0000ff00;

	ui_poplayout();
	
	ui_poplayout();

	ui_end();
}

int main(void) {
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE))
		die(1, "Unable to initialize SDL: %s", SDL_GetError());
	surf = SDL_SetVideoMode( windowWidth, windowHeight, 0, SDL_DOUBLEBUF | SDL_SWSURFACE );
	SDL_WM_SetCaption(windowTitle,windowTitle);
	atexit(SDL_Quit);

	for(;;) {
		SDL_Event e;
		while (SDL_PollEvent( &e ))
			switch( e.type ) {
			case SDL_QUIT:
				return 0;
			case SDL_MOUSEMOTION:
				uis.x = e.motion.x;
				uis.y = e.motion.y;
				break;
			case SDL_MOUSEBUTTONDOWN:
				if (e.button.button == SDL_BUTTON_LEFT)
					uis.buttons |= 1;
				break;
			case SDL_MOUSEBUTTONUP:
				if (e.button.button == SDL_BUTTON_LEFT)
					uis.buttons &= ~1;
				break;
			}

		draw();
		SDL_Flip(surf);
	}
	return 0;
}

