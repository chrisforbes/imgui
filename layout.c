#include "imgui.h"
#include <stdlib.h>

/* carve out some space from a layout */
void ui_do_layout(struct layout * l, enum layout_mode mode, int dx, int dy, SDL_Rect * r) {
	switch(mode) {
		case LM_LEFT:
			r->x = l->x;	r->y = l->y;
			r->w = dx;	r->h = l->h;
			l->x += dx + l->ipad;
			l->w -= dx + l->ipad;
			return;
		case LM_RIGHT:
			r->x = l->x + l->w - dx;	r->y = l->y;
			r->w = dx;	r->h = l->h;
			l->w -= dx + l->ipad;
			return;
		case LM_TOP:
			r->x = l->x;	r->y = l->y;
			r->w = l->w;	r->h = dy;
			l->y += dy + l->ipad;
			l->h -= dy + l->ipad;
			return;
		case LM_BOTTOM:
			r->x = l->x;	r->y = l->y + l->h - dy;
			r->w = l->w;	r->h = dy;
			l->h -= dx + l->ipad;
			return;
		case LM_FILL:
			r->x = l->x;	r->y = l->y;
			r->w = l->w;	r->h = l->h;
			/* ? no change to the layout itself? */
			/* fill should be the last thing added always */
			return;
		default:
			die( 1, "invalid layout mode %d", mode );
	}
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
	uis.l->x = 0;
	uis.l->y = 0;
	uis.l->w = w;
	uis.l->h = h;
	uis.l->ipad = 0;
	uis.l->mode = 0;
}

/* a centered floating panel */
void ui_float(int w, int h, enum layout_mode mode) {
	ui_pushlayout();
	uis.l->w = w;
	uis.l->h = h;
	uis.l->x = uis.l->prev->x + (uis.l->prev->w - w)/2;
	uis.l->y = uis.l->prev->y + (uis.l->prev->h - h)/2;
	uis.l->ipad = 0;
	uis.l->mode = mode;
}

void ui_box_ex(int w, int h, enum layout_mode mode, enum layout_mode placement) {
	SDL_Rect r;
	ui_do_layout( uis.l, placement, w, h, &r );
	ui_pushlayout();
	uis.l->x = r.x;
	uis.l->y = r.y;
	uis.l->w = r.w;
	uis.l->h = r.h;
	uis.l->mode = mode;
	uis.l->ipad = 0;
}

void ui_box(int w, int h, enum layout_mode mode) {
	ui_box_ex( w, h, mode, uis.l->mode );
}

void ui_pad(int xpad, int ipad) {
	uis.l->x += xpad;
	uis.l->y += xpad;
	uis.l->w -= 2 * xpad;
	uis.l->h -= 2 * xpad;
	uis.l->ipad = ipad;
}
