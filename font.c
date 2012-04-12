#include "imgui.h"

#include <stdlib.h>
#include <ft2build.h>
#include FT_FREETYPE_H

static FT_Library library = 0;

struct glyph {
	SDL_Rect r;
	int set;
	int ox, oy;
	float advance;
};

struct font {
	FT_Face face;
	SDL_Surface * surf;
	int x, y, h;
	struct glyph *gs;
};

void font_precache( struct font * f, int ch )
{
	int index = FT_Get_Char_Index(f->face, ch);
	if (FT_Load_Glyph(f->face, index, FT_LOAD_RENDER))
		die(1, "FT_Load_Glyph failed for %d[-> %d]", ch, index);

	FT_GlyphSlot slot = f->face->glyph;
	struct glyph * g = f->gs + ch;

	g->set = 1;
	g->r.w = slot->metrics.width >> 6;
	g->r.h = slot->metrics.height >> 6;

	if (g->r.w + f->x > f->surf->pitch) {	/* overflow this line */
		f->x = 0;
		f->y += f->h;
		f->h = 0;
	}

	g->r.x = f->x;
	g->r.y = f->y;
	g->advance = slot->metrics.horiAdvance / 64.0f;
	g->ox = slot->bitmap_left;
	g->oy = slot->bitmap_top;
	
	if (f->h < g->r.h) f->h = g->r.h;
	f->x += g->r.w;

	for( int y = 0; y < g->r.h; y++ )
		memcpy( (char *)f->surf->pixels + (y + g->r.y) * f->surf->pitch + g->r.x,
			(char *)slot->bitmap.buffer + y * slot->bitmap.pitch,
			slot->bitmap.pitch );
}

static unsigned blend_channel(unsigned a, unsigned b, unsigned t) {
	a &= 0xff; b &= 0xff;
	return (((255 - t) * a) + (t * b)) >> 8;
}

static unsigned blend(unsigned t, unsigned s) {
	unsigned a = s >> 24;
	return (blend_channel(t,s,a)) |
	       (blend_channel(t>>8,s>>8,a)<<8) |
	       (blend_channel(t>>16,s>>16,a)<<16);
}

typedef void glyph_f( struct font * f, int x, int y, struct glyph * g, int c );

static void font_draw_glyph( struct font * f, int x, int y, struct glyph * g, int c ) {
	for( int j = 0; j < g->r.h; j++ ) {
		int * t = (int *)surf->pixels + (j + y) * (surf->pitch>>2) + x;
		char * s = (char *)f->surf->pixels + 
			(j + g->r.y) * f->surf->pitch + g->r.x;
		for( int i = 0; i < g->r.w; i++ )
			t[i] = blend(t[i],(c&0x00ffffff) | (s[i]<<24));
	}
}

int font_draw_ex( struct font * f, int x, int y, char const * p, int c, glyph_f * dg ) {
	float u = x;
	int _c = c;
	int in_escape = 0;

	while(*p) {
		if (!in_escape && *p == '\\') { in_escape = 1; p++; continue; }
		if (in_escape && *p != '\\') {
			if (*p == '!') { c = _c; p++; in_escape = 0; continue; }
			char * q = 0;
			c = strtol(p, &q, 16);
			if (q != p) { if (*q == ';') q++; p = q; in_escape = 0; }
			else { in_escape = 0; p++; };
			continue;
		}
		
		struct glyph * g = f->gs + *p;
		if (!g->set) font_precache( f, *p );

		if (dg) dg( f, (int)u + g->ox, y - g->oy, g, c );

		u += g->advance;
		in_escape = 0;
		p++;
	}

	return (int)(u - x);
}

int font_draw( struct font * f, int x, int y, char const * p, int c ) {
	return font_draw_ex( f, x, y, p, c, font_draw_glyph );
}

int font_measure( struct font * f, char const * p ) {
	return font_draw_ex( f, 0, 0, p, 0, 0 );
}

struct font * font_load( char const * name, int size ) {
	if (!library)
		FT_Init_FreeType( &library );

	struct font * f = calloc(1, sizeof(*f));

	if (FT_New_Face(library, name, 0, &f->face))
		die( 1, "Unable to load font %s:%d", name, size );

	FT_Set_Pixel_Sizes( f->face, 0, size );

	f->surf = SDL_CreateRGBSurface(SDL_SWSURFACE, 1024, 1024, 8, 0, 0, 0, 0);
	if (!f->surf)
		die( 1, "Unable to create paletted surface for font" );
	
	f->gs = calloc(256, sizeof(*f->gs));

	for (int i = 0x20; i < 0x80; i++)
		font_precache(f, i);

	return f;
}
