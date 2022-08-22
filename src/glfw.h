#ifndef __GLFW_H__
#define __GLFW_H__
#include <GLFW/glfw3.h>
#include "definitions.h"
#include <stdio.h>

static GLFWwindow* window;

int glfw_init(struct global_data *data,void(*framebuffer_size_callback)(GLFWwindow *,int,int),void(*character_callback)(GLFWwindow *,unsigned int),void (*key_callback)(GLFWwindow *,int,int,int,int)){
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER,1);
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwSwapInterval(1);


#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    window = glfwCreateWindow(data->window_width, data->window_height, "Term", NULL, NULL);
    if (window == NULL)
    {
        fprintf(stderr,"Failed to create GLFW window\n");
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCharCallback(window, character_callback);
    glfwSetKeyCallback(window, key_callback);
    return 0;
}




#endif
