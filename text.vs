#version 330 core
layout (location = 0) in vec2 vertex; // <vec2 pos, vec2 tex>
layout (location = 1) in vec2 size; // <vec2 pos, vec2 tex>
layout (location = 2) in vec3 textColor_in;
layout (location = 3) in vec2 TexCoords_in; // <vec2 pos, vec2 tex>
out vec2 TexCoords;
out vec3 textColor;

uniform mat4 projection;

uniform mat4 scale_matrix;
uniform mat4 char_screen;

out VS_OUT {
    vec2 TexCoords;
    vec3 textColor;
} vs_out;

void main()
{
    vs_out.textColor=textColor_in;
    vs_out.TexCoords=TexCoords_in;
    gl_Position = projection * (vec4((char_screen * vec4(vertex, 0.0, 1.0)).xy,0.0,0.0) + (scale_matrix * vec4(size,0.0,1.0)));
}
