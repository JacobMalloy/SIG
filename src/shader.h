#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>

#include <stdio.h>
#include <stdlib.h>
int opengl_compile_shaders();

int opengl_compile_shaders(char * vertex, char *frag){
    unsigned int fragmentShader;
    unsigned int vertexShader;
    unsigned int shaderProgram;

    int  success;
    char infoLog[512];
    char file_buffer[4096];
    char * tmp = file_buffer;
    FILE *fd;
    int tmp_int;


    fd = fopen(vertex,"r");
    tmp_int =fread(file_buffer,1,4096,fd);
    file_buffer[tmp_int]=0;
    fclose(fd);
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, (const char**)&tmp, NULL);
    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if(!success){
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        fprintf(stderr,"Failed to compile vertex shader\n%s\n",infoLog);
        goto fail_post_vertex;
    }
    fd = fopen(frag,"r");
    tmp_int =fread(file_buffer,1,4096,fd);
    file_buffer[tmp_int]=0;
    fclose(fd);
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, (const char**)&tmp, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if(!success){
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        fprintf(stderr,"Failed to compile fragment shader\n%s\n",infoLog);
        goto fail_post_fragment;
    }
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        fprintf(stderr,"Failed to link shaders\n%s\n",infoLog);
        goto fail_post_fragment;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return shaderProgram;

    fail_post_fragment:;
    glDeleteShader(fragmentShader);
    fail_post_vertex:;
    glDeleteShader(vertexShader);
    return -1;
}


#endif
