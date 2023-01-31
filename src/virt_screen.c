#include "virt_screen.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

struct line_array{
    struct text_data *data;
    int size;
    int index;
};


struct circle_array{
    struct line_array *data;
    int size;
    int index;
};

void initialize_line_array(struct line_array *la,int size){
    la->data=malloc(sizeof(struct line_array)*size);
    la->size = size;
    la->index=0;
}

void destroy_line_array(struct line_array *la){
    la->size=0;
    la->index=0;
    if(la->data){
        free(la->data);
    }
    la->data=NULL;
}

void push_back_line_array(struct line_array *la,struct text_data *data){
    if(la->data){
        la->size=60;
        la->data=malloc(sizeof(struct text_data)*la->size);
        la->index=-1;
    }
    la->index+=1;
    if(la->index >= la->size){
        la->size = la->size*3/2;
        la->data=realloc(la->data,sizeof(struct text_data)*la->size);
    }
    memcpy(&la->data[la->index],data,sizeof(struct text_data));
}

void initialize_circle_array(struct circle_array *array,int size){
    array->size=size;
    array->index=0;
    array->data=malloc(sizeof(struct line_array)*size);
    memset(array->data,0,sizeof(struct line_array)*size);
}
void destroy_circle_array(struct circle_array *array){
    for(int i=0;i<array->size;i++){
        destroy_line_array(array->data[i]);
    }
    free(array->data);
    array->data=0;
}

int push_back_circle_array(struct circle_array *array){
    array->index+=1;
    array->index%=array->size;
    return array->index;
}




int scroll_back;
int cursor_x;
int cursor_y;


static int create_empty_text_data_array(struct line_struct *in_line){
    #if 0

    struct text_data return_value;
    return_value.character=32;
    return_value.fg_red=255;
    return_value.fg_green=255;
    return_value.fg_blue=255;
    return_value.fg_flags=0;
    return_value.bg_red=0;
    return_value.bg_green=0;
    return_value.bg_blue=0;
    return_value.bg_flags=0;
    #endif
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
