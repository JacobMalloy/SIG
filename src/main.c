#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "shader.h"

#include <time.h>

struct text_vertex{
    float x;
    float y;
    float r;
    float g;
    float b;
    float bg_r;
    float bg_g;
    float bg_b;
    float tx_offset;
};

struct text_vertex_array{
    struct text_vertex *data;
    int length;
    int size;
};

/// Holds all state information relevant to a character as loaded using FreeType
struct character {

  int tx; // x offset of glyph in texture coordinates
};


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void character_callback(GLFWwindow* window, unsigned int codepoint);
void processInput(GLFWwindow *window);
void RenderText(unsigned int shader, char * text, float x, float y, float color_r,float color_g,float color_b);
void initialize_text_vertex_array(struct text_vertex_array *array);
void insert_text_vertex_array(struct text_vertex_array *array,struct text_vertex * f);
void set_size(unsigned int width,unsigned int height, unsigned int shader);
int SCR_WIDTH = 800;
int SCR_HEIGHT = 600;

unsigned int atlas_width;
unsigned int atlas_height;
unsigned int advance_x;
GLuint font_tex;

unsigned int font_size=50;
unsigned int orig_font_size=50;
float font_scale;
unsigned int shader;

char output[4096];
char *current_location;

struct character* characters;
unsigned int VAO, VBO;
static struct text_vertex_array my_text_vertex_array;

int main()
{
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
    // FreeType
    // --------
    FT_Library ft;
    // All functions return a value different than 0 whenever an error occurred
    if (FT_Init_FreeType(&ft))
    {
        fprintf(stderr,"ERROR::FREETYPE: Could not init FreeType Library\n");
        return -1;
    }

    // find path to font
    char* font_name = "victormono.ttf";
    if (font_name[0]=='0')
    {
        fprintf(stderr,"ERROR::FREETYPE: Failed to load font_name\n");
        return -1;
    }

    // load font as face
    FT_Face face;
    if (FT_New_Face(ft, font_name, 0, &face)) {
        fprintf(stderr,"ERROR::FREETYPE: Failed to load font\n");
        return -1;
    }
    else {
        // set size to load glyphs as
        FT_Set_Pixel_Sizes(face, 0, orig_font_size);

        // disable byte-alignment restriction
        //glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        // load first 128 characters of ASCII set
        FT_GlyphSlot g = face->glyph;
        int w = 0;
        int h = 0;

        int max_above, max_below;

        max_above=0;
        max_below=0;
        characters=(struct character *)malloc((128)*sizeof(struct character));
        for(int i = 32; i < 128; i++) {
            if(FT_Load_Char(face, i, FT_LOAD_RENDER)) {
                fprintf(stderr, "Loading character %c failed!\n", i);
                continue;
            }
            w += g->advance.x>>6;

            h = g->bitmap.rows<h?h:g->bitmap.rows;

            max_above = g->bitmap_top<h?h:g->bitmap_top;
            max_below = -(g->bitmap_top-g->bitmap.rows)<h?h:-(g->bitmap_top-g->bitmap.rows);

        }
        h = max_below + max_above;

        atlas_width = w;
        atlas_height=h;

        glActiveTexture(GL_TEXTURE0);
        glGenTextures(1, &font_tex);
        glBindTexture(GL_TEXTURE_2D, font_tex);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        unsigned char *tmp_data = malloc(w*h);
        memset(tmp_data,0,w*h);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, w, h, 0, GL_RED, GL_UNSIGNED_BYTE, tmp_data);
        free(tmp_data);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        int x = 0;

        for(int i = 32; i < 128; i++) {
            struct character * ch = characters+i;
            if(FT_Load_Char(face, i, FT_LOAD_RENDER)){
                fprintf(stderr,"failed:%c\n",i);
                continue;
            }
            glTexSubImage2D(GL_TEXTURE_2D, 0, x+g->bitmap_left,h-g->bitmap_top-max_below, g->bitmap.width, g->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, g->bitmap.buffer);


            ch->tx = x;
            x += g->advance.x>>6;
            advance_x=g->advance.x>>6;
            //printf("width:%f,height:%f\n",characters[i].bw,characters[i].bh);
        }

        //glBindTexture(GL_TEXTURE_2D, 0);
    }
    // destroy FreeType once we're finished
    FT_Done_Face(face);
    FT_Done_FreeType(ft);


    // configure VAO/VBO for texture quads
    // -----------------------------------
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(struct text_vertex), 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(struct text_vertex), (void *)(2*sizeof(float)));
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(struct text_vertex), (void *)(5*sizeof(float)));
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(struct text_vertex), (void *)(8*sizeof(float)));
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

        RenderText(shader, "Testing green", 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
        RenderText(shader, "testing red", 0.0f, 1.0f, 1.0f, 0.0f, 0.0f);
        RenderText(shader, "Hello world", 4.0f, 5.0f, 0.0f, 0.0f, 0.8f);
        RenderText(shader, output, 0.0f, 6.0f, 247.0/255.0, 127.0/255.0, 0.0f);

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
void RenderText(unsigned int shader, char* text, float x, float y, float color_r,float color_g,float color_b)
{
    my_text_vertex_array.length=0;
    // activate corresponding render state
    glUseProgram(shader);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    GLfloat value[16];
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
    glUniform1f(glGetUniformLocation(shader, "advance_x"), (float)advance_x);
    glUniform1f(glGetUniformLocation(shader, "atlas_height"), (float)atlas_height);
    glUniform1f(glGetUniformLocation(shader, "bg_alpha"), 0.6);
    // iterate through all characters
    float value2[16]={advance_x*font_scale,0,0,0,0,-1.0*atlas_height*font_scale,0,0,0,0,1.0,0,0,1.0*SCR_HEIGHT-(1.0*atlas_height*font_scale),0,1};
    glUniformMatrix4fv(glGetUniformLocation(shader, "char_screen"), 1, GL_FALSE, value2);
    char * c;
    for (c = text; *c != '\0'; c++)
    {
        struct character ch = characters[*c];
        struct text_vertex vert;

        vert.x=x;
        vert.y=y;
        vert.r=color_r;
        vert.g=color_g;
        vert.b=color_b;
        vert.bg_r=0;
        vert.bg_g=0;
        vert.bg_b=0;
        vert.tx_offset=ch.tx;

        insert_text_vertex_array(&my_text_vertex_array,&vert);

        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x ++; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
    }
    // render glyph texture over quad
    glBindTexture(GL_TEXTURE_2D, font_tex);
    // update content of VBO memory
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(struct text_vertex) * my_text_vertex_array.length, my_text_vertex_array.data, GL_DYNAMIC_DRAW);
    //glBufferSubData(GL_ARRAY_BUFFER, 0, my_float_array.length*sizeof(float), my_float_array.data); // be sure to use glBufferSubData and not glBufferData

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // render quad
    glDrawArrays(GL_POINTS, 0, my_text_vertex_array.length);
    glBindVertexArray(0);
    //glBindTexture(GL_TEXTURE_2D, 0);
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
