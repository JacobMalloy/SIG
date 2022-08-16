#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <stdint.h>


#include "shader.h"

#include "bucket_array.h"
#include "font.h"



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

struct text_vertex_array{
    struct text_vertex *data;
    int length;
    int size;
};

struct line_struct{
    struct text_vertex *vertex_array;
    struct text_data *data_array;
    int length;
    int size;
    struct{
        uint32_t dirty:1;
    };
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


int SCR_WIDTH = 800;
int SCR_HEIGHT = 600;
unsigned int width_chars;
unsigned int height_chars;

GLuint font_tex;

unsigned int font_size=50;
unsigned int orig_font_size=50;
float font_scale;
unsigned int shader;

char output[4096];
char *current_location;

struct font_info my_font_info;
unsigned int VAO, VBO;
static struct text_vertex_array my_text_vertex_array;

int main()
{
    my_font_info.orig_font_size=orig_font_size;
    //bucket_array_t virtual_screen = bucket_array_make( 64,struct line_struct );
    current_location=output;
    font_scale = 1.0*font_size/orig_font_size;
    initialize_text_vertex_array(&my_text_vertex_array);
    // glfw: initialize and configure
    // ------------------------------
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
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Term", NULL, NULL);
    if (window == NULL)
    {
        fprintf(stderr,"Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCharCallback(window, character_callback);

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
    glfwGetFramebufferSize(window, &SCR_WIDTH, &SCR_HEIGHT);
    set_size(SCR_WIDTH,SCR_HEIGHT,shader);


    my_font_info.characters = malloc(sizeof(struct character)*128);
    // All functions return a value different than 0 whenever an error occurred
    if(freetype_init()){
        return -1;
    }
    char * font_name = "victormono.ttf";
    if (freetype_load_font(font_name,&my_font_info,&font_setup_texture_callback,&font_per_texture_callback)){
        return -1;
    }


    float value[16]={my_font_info.advance_x*font_scale,0,0,0,0,-1.0*my_font_info.atlas_height*font_scale,0,0,0,0,1.0,0,-1.0*my_font_info.advance_x*font_scale,1.0*SCR_HEIGHT,0,1};
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
    glUniform1f(glGetUniformLocation(shader, "advance_x"), (float)my_font_info.advance_x);
    glUniform1f(glGetUniformLocation(shader, "atlas_height"), (float)my_font_info.atlas_height);
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

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.0f, 0.9f, 0.9f, 1.0f);

        glClear(GL_COLOR_BUFFER_BIT);


        my_text_vertex_array.length=0;
        // activate corresponding render state
        glUseProgram(shader);
        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(VAO);

        struct text_vertex *array;
        array= malloc(sizeof(struct text_vertex)*300);
        for(int i = 0 ; i<300;i++){
            array[i].r=1;
            array[i].g=1;
            array[i].b=1;
            array[i].bg_r=0;
            array[i].bg_g=0;
            array[i].bg_b=0;
            array[i].tx_offset=0.0;
        }

        RenderText(shader, "Testing green", 0.0f, 1.0f, 0.0f,array);
        RenderText(shader, "testing red", 1.0f, 0.0f, 0.0f,array+100);
        RenderText(shader, "Hello world", 0.0f, 0.0f, 0.8f,array+200);
        //RenderText(shader, output, 1.0f, 6.0f, 247.0/255.0, 127.0/255.0, 0.0f);
        glBindTexture(GL_TEXTURE_2D, font_tex);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER,0, sizeof(struct text_vertex) * 300, array);
        free(array);
        //glBufferSubData(GL_ARRAY_BUFFER, 0, my_float_array.length*sizeof(float), my_float_array.data); // be sure to use glBufferSubData and not glBufferData

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_POINTS, 0, 300);
        glBindVertexArray(0);
        //glBindTexture(GL_TEXTURE_2D, 0);
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

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
void RenderText(unsigned int shader, char* text, float color_r,float color_g,float color_b, struct text_vertex* array)
{

    int x = 1;
    // iterate through all characters
    char * c;
    for (c = text; *c != '\0'; c++)
    {
        struct character* ch = &my_font_info.characters[(int)*c];
        //struct text_vertex vert;

        array[x].r=color_r;
        array[x].g=color_g;
        array[x].b=color_b;
        array[x].bg_r=0;
        array[x].bg_g=0;
        array[x].bg_b=0;
        array[x].tx_offset=ch->tx;
        //insert_text_vertex_array(&my_text_vertex_array,&vert);

        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x ++; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
    }
    // render glyph texture over quad


}




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
    SCR_HEIGHT=height;
    SCR_WIDTH=width;
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
    width_chars=SCR_WIDTH/width;
    height_chars=SCR_HEIGHT/height;
}

void character_callback(GLFWwindow* window, unsigned int codepoint){
    switch(codepoint){
        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(window, 1);
            break;
        default:
            *(current_location++)=(char)codepoint;

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
