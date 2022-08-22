#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

#ifdef __APPLE__
#include <util.h>
#else
#include <pty.h>
#include <utmp.h>
#endif

#include "terminal.h"


int start_terminal(struct global_data *data){
    int slave_fd;
    int flags;
    struct winsize ws;


    //pipe(to_terminal);
    if(openpty(&data->master_fd,&slave_fd,NULL,NULL,&ws)){
        perror("failed to open openpty");
        return 1;
    }
    flags = fcntl(data->master_fd,F_GETFL);
    fcntl(data->master_fd,F_SETFL,flags|O_NONBLOCK);
    if(!fork()){
        close(data->master_fd);
        login_tty(slave_fd);
        //dup2(from_terminal[1],2);
        //close(from_terminal[1]);
        execlp("sh","sh",NULL);
        exit(1);
    }
    close(slave_fd);
    //close:
    return 0;
}


int process_terminal(struct global_data *global){
    char buffer[4096];
    int n;

    struct text_data tmp_data;

    tmp_data.fg_red=255;
    tmp_data.fg_green=255;
    tmp_data.fg_blue=255;
    tmp_data.bg_red=0;
    tmp_data.bg_green=0;
    tmp_data.bg_blue=0;

    n = read(global->master_fd,buffer,4096);

    for(int i=0;i<n;i++){
        printf("%d\n",buffer[i]);
        switch(buffer[i]){
            case '\n':
                move_cursor(0,1);
                set_cursor(0,-1);
                break;
            default:
                tmp_data.character = buffer[i];
                set_character_cursor(&tmp_data,global);
                move_cursor(1,0);
                break;
        }
    }
    return 0;
}
