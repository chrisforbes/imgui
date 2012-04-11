#include "imgui.h"
#include <stdlib.h>

/* carve out some space from a layout */
void ui_do_layout(struct layout * l, int dx, int dy, SDL_Rect * r) {
	r->x = l->x + l->ux;
	r->y = l->y + l->uy;
	r->w = (l->flags & LAYOUT_VBOX) ? l->w : dx;
	r->h = (l->flags & LAYOUT_HBOX) ? l->h : dy;

	if (l->flags & LAYOUT_VBOX) l->uy += dy + l->ipad;
	if (l->flags & LAYOUT_HBOX) l->ux += dx + l->ipad;

	/* todo: what happens if the element doesn't actually fit?
	 * in some cases, it's sensible to spill into another row/column */
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
	uis.l->ipad = 0;
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
	uis.l->ipad = 0;
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
	uis.l->ipad = 0;
}

/* stacks elements horizontally */
void ui_hbox(int h) { ui_box_helper( 0, h, LAYOUT_HBOX ); }
/* stacks elements vertically */
void ui_vbox(int w) { ui_box_helper( w, 0, LAYOUT_VBOX ); }

void ui_pad(int xpad, int ipad) {
	uis.l->x += xpad;
	uis.l->y += xpad;
	uis.l->w -= 2 * xpad;
	uis.l->h -= 2 * xpad;
	uis.l->ipad = ipad;
}
