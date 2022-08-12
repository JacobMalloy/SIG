#version 330 core
layout (points) in;
layout (triangle_strip, max_vertices = 6) out;


uniform float advance_x;
uniform float atlas_height;
uniform mat4 projection;
uniform mat4 scale_matrix;

in VS_OUT {
    float TexCoords;
    vec3 textColor;
} gs_in[];

out vec2 TexCoords;
out vec3 textColor;

void main() {
    vec4 char_loc;

    char_loc= vec4(0,0,0,1);
    gl_Position = projection*(gl_in[0].gl_Position + scale_matrix * char_loc);
    TexCoords = vec2(gs_in[0].TexCoords,atlas_height);
    textColor = gs_in[0].textColor;
    EmitVertex();
    char_loc= vec4(advance_x,0,0,1);
    gl_Position = projection*(gl_in[0].gl_Position + scale_matrix * char_loc);
    TexCoords = vec2(gs_in[0].TexCoords + advance_x,atlas_height);
    textColor = gs_in[0].textColor;
    EmitVertex();
    char_loc= vec4(0,atlas_height,0,1);
    gl_Position = projection*(gl_in[0].gl_Position + scale_matrix * char_loc);
    TexCoords = ivec2(gs_in[0].TexCoords,0);
    textColor = gs_in[0].textColor;
    EmitVertex();

    char_loc= vec4(advance_x,atlas_height,0,1);
    gl_Position = projection*(gl_in[0].gl_Position + scale_matrix * char_loc);
    TexCoords = ivec2(gs_in[0].TexCoords + advance_x,0);
    textColor = gs_in[0].textColor;

    //textColor=vec3(0,0,1);
    EmitVertex();

    EndPrimitive();

}
