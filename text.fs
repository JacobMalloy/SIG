#version 330 core
//layout (location = 1) in vec3 textColor; // <vec2 pos, vec2 tex>
in vec2 TexCoords;
in vec3 textColor;

out vec4 color;

uniform sampler2D text;
//uniform vec3 textColor;

void main()
{
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
    color = vec4(textColor, 1.0) * sampled;
}
