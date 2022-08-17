#ifndef __DEFINITIONS_H__
#define __DEFINITIONS_H__
#include <stdint.h>

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

struct global_data{
    struct font_info font_info;
    int window_width;
    int window_height;
    int width_chars;
    int height_chars;
};

struct text_vertex{
    float r;
    float g;
    float b;
    float bg_r;
    float bg_g;
    float bg_b;
    float tx_offset;
};

struct text_data{
    uint32_t character;
    struct{
            struct{
                uint32_t fg_red:8;
                uint32_t fg_green:8;
                uint32_t fg_blue:8;
                uint32_t fg_flags:8;
            };
            struct{
                uint32_t bg_red:8;
                uint32_t bg_green:8;
                uint32_t bg_blue:8;
                uint32_t bg_flags:8;
            };
    };
};

struct line_struct{
    struct text_vertex *vertex_array;
    struct text_data *data_array;
    int length;
    int size;
    struct{
        uint32_t dirty:1;
        uint32_t wrapped:1;
    };
};

#endif
