#version 330 core
//layout (location = 1) in vec3 textColor; // <vec2 pos, vec2 tex>
in vec2 TexCoords;
in vec3 textColor;

out vec4 color;

uniform sampler2D text;

void main()
{
    //vec4 sampled = vec4(1.0, 1.0, 1.0, 1.0);
    vec3 bg_color = vec3(0,0,0);
    float alpha = texelFetch(text, ivec2(int(TexCoords.x),int(TexCoords.y)),0).r;
//    float alpha = texture(text, TexCoords).r;
    color = vec4(mix(bg_color,textColor,alpha),1.0);

}
