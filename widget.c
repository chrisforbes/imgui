#include "imgui.h"

/* ui colors: todo: factor this out */
static int c_shadow = 0xff000000;
static int c_normal = 0xffcccccc;
static int c_hilite = 0xffeeeeee;
static int c_pushed = 0xff888888;

/* check if a point is in a rect */
int ui_inside(SDL_Rect * r) {
	return uis.x >= r->x && uis.x < r->x + r->w &&
		uis.y >= r->y && uis.y < r->y + r->h;
}

/* draw a button */
int ui_button(ui_id id, char const * label) {
	SDL_Rect r;
	ui_do_layout( uis.l, BUTTON_WIDTH, BUTTON_HEIGHT, &r );
	if (ui_inside(&r))
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

/* begin building the ui */
void ui_begin(void) {
	uis.hot = 0;
}

/* end building the ui */
void ui_end(void) {
	uis.lasthot = uis.hot;

	/* it is an error to leave layouts pushed */
	if (uis.l)
		die(1, "ui_end called with unfinished layouts");
}

