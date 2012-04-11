#include <stdio.h>

/* imgui
 *	- very simple widgets
 *	- no 'widget tree' state retained in ui lib, that's the app's problem
 *	- automated layout and metrics as much as possible
 *		(although multipass layout requires app cooperation)
 */

#include "imgui.h"

struct state uis = { 0 };

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

void ui_begin() {
	uis.hot = 0;
}

void ui_end(void) {
	uis.lasthot = uis.hot;

	/* it is an error to leave layouts pushed */
	if (uis.l)
		die(1, "ui_end called with unfinished layouts");
}

/* local ui state */

void draw(void) {
	SDL_FillRect(surf, 0, c_background);
	ui_begin(windowWidth, windowHeight);

	ui_toplevel(windowWidth, windowHeight);
	ui_float(300,300);
	ui_fill(0xff444444);	/* window background */

	ui_hbox(button_height);
	
	if (ui_button(1, "Toggle Red Channel"))
		c_background ^= 0x00ff0000;

	if (ui_button(2, "Toggle Green Channel"))
		c_background ^= 0x0000ff00;

	ui_poplayout();
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

