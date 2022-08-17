#include "font.h"

#define MAX(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })



static FT_Library ft;


int freetype_init(){
    if (FT_Init_FreeType(&ft))
    {
        fprintf(stderr,"ERROR::FREETYPE: Could not init FreeType Library\n");
        return 1;
    }
    return 0;
}

int freetype_load_font(char *name,struct global_data *data,int (*setup)(int w, int h),int (*per_glyph)(FT_GlyphSlot g,int x, int y)){
    FT_Face face;
    int w;
    int h;
    int x;
    int max_above;
    int max_below;
    FT_GlyphSlot g;

    x=0;
    w=0;
    if (name[0]=='0')
    {
        fprintf(stderr,"ERROR::FREETYPE: Failed to load font_name\n");
        return 1;
    }

    // load font as face
    if (FT_New_Face(ft, name, 0, &face)) {
        fprintf(stderr,"ERROR::FREETYPE: Failed to load font\n");
        return 1;
    }
    else {
        // set size to load glyphs as
        FT_Set_Pixel_Sizes(face, 0, data->font_info.orig_font_size);

        // disable byte-alignment restriction
        //glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        // load first 128 info->characters of ASCII set
        g = face->glyph;

        max_above=0;
        max_below=0;
        for(int i = 32; i < 128; i++) {
            if(FT_Load_Char(face, i, FT_LOAD_RENDER)) {
                fprintf(stderr, "Loading character %c failed!\n", i);
                continue;
            }
            w += g->advance.x>>6;



            max_above = MAX(g->bitmap_top,max_above);
            max_below = MAX(-((int)g->bitmap_top-(int)g->bitmap.rows),max_below);

        }
        h = max_below + max_above;

        data->font_info.atlas_width = w;
        data->font_info.atlas_height=h;

        (*setup)(w,h);

        for(int i = 32; i < 128; i++) {
            struct character * ch = data->font_info.characters+i;
            if(FT_Load_Char(face, i, FT_LOAD_RENDER)){
                fprintf(stderr,"failed:%c\n",i);
                continue;
            }
            (*per_glyph)(g,x+g->bitmap_left,h-g->bitmap_top-max_below);

            ch->tx = x;
            x += g->advance.x>>6;
            //printf("width:%f,height:%f\n",data->font_info.characters[i].bw,data->font_info.characters[i].bh);
        }
        data->font_info.advance_x=g->advance.x>>6;

    }
    // destroy FreeType once we're finished
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
    return 0;
}
