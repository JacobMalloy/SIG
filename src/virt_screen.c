#include "virt_screen.h"

bucket_array_t virtual_screen;
int scroll_back;
int cursor_x;
int cursor_y;


static int create_empty_text_data_array(struct line_struct *in_line){
    struct text_data *return_value = in_line->data_array;
    if(return_value == NULL || in_line->length==0){
        return 1;
    }
    return_value->character=32;
    return_value->fg_red=255;
    return_value->fg_green=255;
    return_value->fg_blue=255;
    return_value->fg_flags=0;
    return_value->bg_red=0;
    return_value->bg_green=0;
    return_value->bg_blue=0;
    return_value->bg_flags=0;
    for(int i =1;i<in_line->length;i++){
        memcpy(return_value+i,return_value,sizeof(struct text_data));
    }
    return 0;
}

static int vertex_from_data(struct text_vertex *vertex,struct text_data *t_data, struct global_data *data){
    vertex->r         = (float)t_data->fg_red/255;
    vertex->g         = (float)t_data->fg_green/255;
    vertex->b         = (float)t_data->fg_blue/255;
    vertex->bg_r      = (float)t_data->bg_red/255;
    vertex->bg_g      = (float)t_data->bg_green/255;
    vertex->bg_b      = (float)t_data->bg_blue/255;
    vertex->tx_offset = data->font_info.characters[t_data->character].tx;
    return 0;
}

static int fill_vertex_from_data(struct line_struct *in_line,struct global_data *g_data){
    scroll_back=0;
    for(int i=0;i<in_line->length;i++){
        vertex_from_data(in_line->vertex_array+i,in_line->data_array+i,g_data);

    }
    return 0;
}

int init_virtual_screen(struct global_data *data){
    cursor_x=0;
    cursor_y=0;
    struct line_struct tmp_line;
    tmp_line.size = data->width_chars;
    tmp_line.length = data->width_chars;
    virtual_screen = bucket_array_make( 64,struct line_struct );
    for(int i =0;i<data->height_chars;i++){
        tmp_line.vertex_array=malloc(sizeof(struct text_vertex)*tmp_line.size);
        tmp_line.data_array=malloc(sizeof(struct text_data)*tmp_line.size);
        create_empty_text_data_array(&tmp_line);
        fill_vertex_from_data(&tmp_line,data);
        bucket_array_push(virtual_screen,tmp_line);
    }

    return 0;
}

int fill_vertex_array(struct text_vertex *target,struct global_data *data){
    struct line_struct* tmp_line;
    for(int i = 0;i<data->height_chars;i++){
        tmp_line = bucket_array_item(virtual_screen,i);
        memcpy((target +(i*data->width_chars)) ,tmp_line->vertex_array,sizeof(struct text_vertex)*data->width_chars);
    }
    return 0;
}


int set_character(int x,int y,struct text_data *td, struct global_data *data){
    struct line_struct *tmp_line;
    tmp_line = bucket_array_item(virtual_screen,y+scroll_back);
    memcpy(tmp_line->data_array+x,td,sizeof(struct text_data));
    vertex_from_data(tmp_line->vertex_array+x,tmp_line->data_array+x,data);
    return 0;
}

int set_character_cursor(struct text_data *td, struct global_data *data){
    set_character(cursor_x,cursor_y,td,data);
    return 0;
}


int set_cursor(int x, int y){
    if(x<0){
        x=cursor_x;
    }
    if(y<0){
        y=cursor_y;
    }
    cursor_x=x;
    cursor_y=y;
    return 0;
}

int move_cursor(int x, int y){
    cursor_x+=x;
    cursor_y+=y;
    return 0;
}
