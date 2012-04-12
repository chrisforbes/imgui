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

static void draw_button_label( SDL_Rect * r, char const * label ) {
	int w = font_measure( font, label );
	font_draw( font, r->x + r->w/2 - w/2, r->y + r->h/2 + 3, label, 0 );
}

/* draw a button */
int ui_button_ex(ui_id id, char const * label, enum layout_mode mode) {
	SDL_Rect r;
	ui_do_layout( uis.l, mode, BUTTON_WIDTH, BUTTON_HEIGHT, &r );
	if (ui_inside(&r))
		uis.hot = id;

	if (uis.active == id && !uis.buttons) {
		uis.active = 0;
		return uis.hot == id;
	}

	if (uis.lasthot == id) {
		if (uis.buttons) {
			SDL_FillRect( surf, &r, c_pushed );
			r.x += 1; r.y += 1;
			draw_button_label( &r, label );
			uis.active = id;
			return 0;
		}
		else {
			SDL_FillRect( surf, &r, uis.hot == id ? c_hilite : c_normal );
			draw_button_label( &r, label );
			return 0;
		}
	}

	SDL_FillRect( surf, &r, c_normal );
	draw_button_label( &r, label );

	return 0;
}

int ui_button(ui_id id, char const * label) {
	return ui_button_ex( id, label, uis.l->mode );
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

