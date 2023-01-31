#ifndef __VIRT_SCREEN_H__
#define __VIRT_SCREEN_H__
#include "definitions.h"
#include <stdlib.h>
int init_virtual_screen(struct global_data *data);
int fill_vertex_array(struct text_vertex *target,struct global_data *data);

int set_character(int x,int y,struct text_data *td, struct global_data *data);

int set_character_cursor(struct text_data *td, struct global_data *data);


int set_cursor(int x, int y);

int move_cursor(int x, int y);


#endif
