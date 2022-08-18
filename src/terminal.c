#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <util.h>
#include "terminal.h"
int master_fd;
int slave_fd;


int start_terminal(){
    int flags;
    struct winsize ws;


    //pipe(to_terminal);
    if(openpty(&master_fd,&slave_fd,NULL,NULL,&ws)){
        perror("failed to open openpty");
        return 1;
    }
    flags = fcntl(master_fd,F_GETFL);
    fcntl(master_fd,F_SETFL,flags|O_NONBLOCK);
    if(!fork()){
        close(master_fd);
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

    n = read(master_fd,buffer,4096);
    if(n<=0){
        //perror("fuck");
    }
    //write(1,buffer,n);
    //write(1,"\n",1);
    for(int i=0;i<n;i++){
        switch(buffer[i]){
            case '\n':
                move_cursor(0,1);
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
