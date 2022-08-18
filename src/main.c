#include <glad/glad.h>
#include "glfw.h"
#include <ft2build.h>
#include FT_FREETYPE_H
#include "definitions.h"
#include "virt_screen.h"
#include "terminal.h"
#include <stdint.h>


#include "shader.h"

#include "font.h"





struct text_vertex_array{
    struct text_vertex *data;
    int length;
    int size;
};




void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void character_callback(GLFWwindow* window, unsigned int codepoint);
void processInput(GLFWwindow *window);
void RenderText(unsigned int shader, char * text, float color_r,float color_g,float color_b,struct text_vertex * array);
void initialize_text_vertex_array(struct text_vertex_array *array);
void insert_text_vertex_array(struct text_vertex_array *array,struct text_vertex * f);
void set_size(unsigned int width,unsigned int height, unsigned int shader);
int font_per_texture_callback(FT_GlyphSlot g,int x, int y);
int font_setup_texture_callback(int w, int h);


struct global_data data;

GLuint font_tex;

unsigned int font_size=50;
unsigned int orig_font_size=50;
float font_scale;
unsigned int shader;


unsigned int VAO, VBO;
static struct text_vertex_array my_text_vertex_array;
//extern GLFWwindow* window;


int main()
{
    data.window_width=800;
    data.window_height=600;

    data.font_info.orig_font_size=orig_font_size;
    font_scale = 1.0*font_size/orig_font_size;
    initialize_text_vertex_array(&my_text_vertex_array);
    // glfw: initialize and configure
    // ------------------------------
    if(glfw_init(&data, &framebuffer_size_callback,&character_callback)){
        return -1;
    }

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        fprintf(stderr,"Failed to initialize GLAD\n");
        return -1;
    }

    // OpenGL state
    // ------------
/*     glDisable(GL_DEPTH_TEST); */
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_FRAMEBUFFER_SRGB);

    // compile and setup the shader
    // ----------------------------
    shader = opengl_compile_shaders("text.vs", "text.fs", "text.gs");
    glUseProgram(shader);
    glfwGetFramebufferSize(window, &data.window_width, &data.window_height);
    set_size(data.window_width,data.window_height,shader);


    data.font_info.characters = malloc(sizeof(struct character)*128);
    // All functions return a value different than 0 whenever an error occurred
    if(freetype_init()){
        return -1;
    }
    char * font_name = "victormono.ttf";
    if (freetype_load_font(font_name,&data,&font_setup_texture_callback,&font_per_texture_callback)){
        return -1;
    }


    float value[16]={data.font_info.advance_x*font_scale,0,0,0,0,-1.0*data.font_info.atlas_height*font_scale,0,0,0,0,1.0,0,-1.0*data.font_info.advance_x*font_scale,1.0*data.window_height,0,1};
    glUniformMatrix4fv(glGetUniformLocation(shader, "char_screen"), 1, GL_FALSE, value);
    value[0]=font_scale;
    value[1]=0.0;
    value[2]=0.0;
    value[3]=0.0;
    value[4]=0.0;
    value[5]=font_scale;
    value[6]=0.0;
    value[7]=0.0;
    value[8]=0.0;
    value[9]=0.0;
    value[10]=1.0;
    value[11]=0.0;
    value[12]=0.0;
    value[13]=0.0f;
    value[14]=0.0;
    value[15]=1.0;
    glUniformMatrix4fv(glGetUniformLocation(shader, "scale_matrix"), 1, GL_FALSE, value);
    glUniform1f(glGetUniformLocation(shader, "advance_x"), (float)data.font_info.advance_x);
    glUniform1f(glGetUniformLocation(shader, "atlas_height"), (float)data.font_info.atlas_height);
    glUniform1f(glGetUniformLocation(shader, "bg_alpha"), 0.6);


    // configure VAO/VBO for texture quads
    // -----------------------------------
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(struct text_vertex)*2500, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(struct text_vertex), 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(struct text_vertex), (void *)(0*sizeof(float)));
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(struct text_vertex), (void *)(3*sizeof(float)));
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(struct text_vertex), (void *)(6*sizeof(float)));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    set_size(data.window_width,data.window_height,shader);
    struct text_vertex *array;
    array = malloc(sizeof(struct text_vertex)*data.width_chars*data.height_chars);
    // render loop
    // -----------

    init_virtual_screen(&data);
    if(start_terminal()){
        return -1;
    }
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        glClear(GL_COLOR_BUFFER_BIT);


        my_text_vertex_array.length=0;
        // activate corresponding render state
        glUseProgram(shader);
        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(VAO);

        process_terminal(&data);
        fill_vertex_array(array,&data);

        //RenderText(shader, output, 1.0f, 6.0f, 247.0/255.0, 127.0/255.0, 0.0f);
        glBindTexture(GL_TEXTURE_2D, font_tex);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER,0, sizeof(struct text_vertex)*data.width_chars*data.height_chars, array);
        //glBufferSubData(GL_ARRAY_BUFFER, 0, my_float_array.length*sizeof(float), my_float_array.data); // be sure to use glBufferSubData and not glBufferData
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_POINTS, 0, data.width_chars*data.height_chars);
        glBindVertexArray(0);
        //glBindTexture(GL_TEXTURE_2D, 0);
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    free(array);
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, 1);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    set_size(width,height,shader);
}


// render line of text
// -------------------





void initialize_text_vertex_array(struct text_vertex_array *array){
    array->data=malloc(sizeof(struct text_vertex)*400);
    array->length=0;
    array->size=400;
}

void insert_text_vertex_array(struct text_vertex_array *array,struct text_vertex *in_data){
    if(array->size<=array->length){
        array->size*=2;
        array->data=realloc(array->data,sizeof(struct text_vertex)*array->size);
    }
    memcpy(&array->data[array->length++],in_data,sizeof(struct text_vertex));
}


void set_size(unsigned int width,unsigned int height, unsigned int shader){
    glViewport(0, 0, width, height);
    data.window_height=height;
    data.window_width=width;
    GLfloat value[16];
    value[0]=2.0/(float)width;
    value[1]=0.0;
    value[2]=0.0;
    value[3]=0.0;
    value[4]=0.0;
    value[5]=2.0/(float)height;
    value[6]=0.0;
    value[7]=0.0;
    value[8]=0.0;
    value[9]=0.0;
    value[10]=-1.0;
    value[11]=0.0;
    value[12]=-1.0;
    value[13]=-1.0;
    value[14]=0.0;
    value[15]=1.0;
    glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, value);
    data.width_chars=safe_div(width,data.font_info.advance_x);
    data.height_chars=safe_div(height,data.font_info.atlas_height);
    glUniform1i(glGetUniformLocation(shader, "width_chars"), data.width_chars);
}

void character_callback(GLFWwindow* window, unsigned int codepoint){
    switch(codepoint){
        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(window, 1);
            break;
        default:

        break;
    }
}


int font_per_texture_callback(FT_GlyphSlot g,int x, int y){
    glTexSubImage2D(GL_TEXTURE_2D, 0, x,y, g->bitmap.width, g->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, g->bitmap.buffer);
    return 0;
}

int font_setup_texture_callback(int w, int h){
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &font_tex);
    glBindTexture(GL_TEXTURE_2D, font_tex);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    char * tmp = malloc(w*h);
    memset(tmp,0,w*h);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, w, h, 0, GL_RED, GL_UNSIGNED_BYTE,tmp);
    free(tmp);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    return 0;
}
