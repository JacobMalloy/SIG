#version 330 core
//layout (location = 1) in vec3 textColor; // <vec2 pos, vec2 tex>
in vec2 TexCoords;
in vec3 textColor;

out vec4 color;

uniform sampler2D text;
//uniform vec3 textColor;

void main()
{
    //vec4 sampled = vec4(1.0, 1.0, 1.0, 1.0);
    vec4 bg_color = vec4(0,1,1,1);
    color = vec4(mix(bg_color.xyz,textColor.xyz,texture(text, TexCoords).r),1.0);

}
