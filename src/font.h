#ifndef __FONT_H__
#define __FONT_H__

#include <ft2build.h>
#include FT_FREETYPE_H


struct character {

    int tx; // x offset of glyph in texture coordinates
};
struct font_info{
    struct character *characters;
    int orig_font_size;
    int atlas_height;
    int atlas_width;
    int advance_x;
};

int freetype_init();
int freetype_load_font(char *name,struct font_info *info,int (*setup)(int w, int h),int (*per_glyph)(FT_GlyphSlot g,int x, int y));

#endif
