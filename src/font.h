#ifndef __FONT_H__
#define __FONT_H__

#include <ft2build.h>
#include FT_FREETYPE_H
#include "definitions.h"



int freetype_init();
int freetype_load_font(char *name,struct global_data *data,int (*setup)(int w, int h),int (*per_glyph)(FT_GlyphSlot g,int x, int y));

#endif
