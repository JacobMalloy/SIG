#version 330 core
layout (location = 0) in vec2 vertex; // <vec2 pos, vec2 tex>
layout (location = 1) in vec3 textColor_in;
layout (location = 2) in vec3 bg_textColor_in;
layout (location = 3) in float TexCoords_in; // <vec2 pos, vec2 tex>
out vec2 TexCoords;
out vec3 textColor;


uniform mat4 char_screen;

out VS_OUT {
    float TexCoords;
    vec3 bg_textColor;
    vec3 textColor;
} vs_out;

void main()
{
    vs_out.textColor=textColor_in;
    vs_out.TexCoords=TexCoords_in;
    vs_out.bg_textColor=bg_textColor_in;
    gl_Position = (vec4((char_screen * vec4(gl_VertexID%100,gl_VertexID/100+1, 0.0, 1.0)).xy,0.0,0.0) );
}
