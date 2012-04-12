#include <stdio.h>

/* imgui
 *	- very simple widgets
 *	- no 'widget tree' state retained in ui lib, that's the app's problem
 *	- automated layout and metrics as much as possible
 *		(although multipass layout requires app cooperation
 *
 * things that are kindof tricky
 * 	- different handling for each mouse button / complex interactions
 * 	- keyboard vs mouse focus
 * 	- tooltip routing
 * 	- unique id generation
 * 	- ...
 *
 * big todos
 * 	- keyboard handling
 * 	- text rendering
 * 	- separation of ui behavior from drawing code (not everything wants to be sdl)
 * 	- hbox/vbox model isnt really good enough. we need to be able to allocate chunks out
 * 		of any side of a layout.
 */

#include "imgui.h"

struct state uis = { 0 };
struct font * font;

static int windowWidth = 640;
static int windowHeight = 480;
SDL_Surface * surf;
static int c_background = 0xff0000ff;

/* local ui state */

void draw(void) {
	SDL_FillRect(surf, 0, c_background);
	ui_begin();

	ui_toplevel(windowWidth, windowHeight);
	ui_float(300,300, LM_TOP);
	ui_fill(0xff444444);	/* window border */
	ui_pad(3,0);		/* 3px border */
	ui_fill(0xff777777);	/* window fill */

	ui_pad(6,3);
	ui_box(0, BUTTON_HEIGHT, LM_LEFT);
	ui_pad(0,3);	
	if (ui_button(1, "Red"))
		c_background ^= 0x00ff0000;

	if (ui_button(2, "Green"))
		c_background ^= 0x0000ff00;

	if (ui_button(3, "Blue"))
		c_background ^= 0x000000ff;

	ui_poplayout();

	/* a second row */
	ui_box_ex(0, BUTTON_HEIGHT, LM_LEFT, LM_BOTTOM);
	ui_pad(0,3);
	if (ui_button_ex(4, "Exit", LM_RIGHT)) {
		SDL_Event e;
		e.type = SDL_QUIT;
		SDL_PushEvent(&e);
	}

	ui_poplayout();

	/* show what is left */
	ui_fill(0xffff0000);

	ui_poplayout();
	ui_poplayout();

	font_draw( font, 20, 20, "Hello World \\ff00;in green \\!and back and \\ff0000;in red \\!and back", -1 );

	ui_end();
}

int main(void) {
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE))
		die(1, "Unable to initialize SDL: %s", SDL_GetError());
	surf = SDL_SetVideoMode( windowWidth, windowHeight, 0, SDL_DOUBLEBUF | SDL_SWSURFACE );
	SDL_WM_SetCaption("IMGUI Test", "IMGUI Test");
	atexit(SDL_Quit);

	font = font_load( "data/FreeSans.ttf", 12 );

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

