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
SDL_Surface * surf;
static int c_background = 0xff0000ff;

/* local ui state */

void draw(void) {
	SDL_FillRect(surf, 0, c_background);
	ui_begin();

	ui_toplevel(windowWidth, windowHeight);
	ui_float(300,300, LAYOUT_VBOX);
	ui_fill(0xff444444);	/* window background */

	ui_pad(6,6);
	ui_hbox(BUTTON_HEIGHT);
	ui_pad(0,3);	
	if (ui_button(1, "Toggle Red Channel"))
		c_background ^= 0x00ff0000;

	if (ui_button(2, "Toggle Green Channel"))
		c_background ^= 0x0000ff00;

	ui_poplayout();

	/* a second row */
	ui_hbox(BUTTON_HEIGHT);
	ui_pad(0,3);
	if (ui_button(3, "Toggle Blue Channel"))
		c_background ^= 0x000000ff;
	ui_poplayout();

	ui_poplayout();
	ui_poplayout();

	ui_end();
}

int main(void) {
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE))
		die(1, "Unable to initialize SDL: %s", SDL_GetError());
	surf = SDL_SetVideoMode( windowWidth, windowHeight, 0, SDL_DOUBLEBUF | SDL_SWSURFACE );
	SDL_WM_SetCaption("IMGUI Test", "IMGUI Test");
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

