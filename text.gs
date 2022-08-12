#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;


in VS_OUT {
    vec2 TexCoords;
    vec3 textColor;
} gs_in[];

out vec2 TexCoords;
out vec3 textColor;

void main() {
    gl_Position = gl_in[0].gl_Position;
    TexCoords = gs_in[0].TexCoords;
    textColor = gs_in[0].textColor;
    EmitVertex();
    gl_Position = gl_in[1].gl_Position;
    TexCoords = gs_in[1].TexCoords;
    textColor = gs_in[1].textColor;
    EmitVertex();
    gl_Position = gl_in[2].gl_Position;
    TexCoords = gs_in[2].TexCoords;
    textColor = gs_in[2].textColor;
    EmitVertex();
    EndPrimitive();

}
