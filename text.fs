#version 330 core
//layout (location = 1) in vec3 textColor; // <vec2 pos, vec2 tex>
in vec2 TexCoords;
in vec3 textColor;

out vec4 color;

uniform sampler2D text;
uniform float bg_alpha;

void main()
{
    //vec4 sampled = vec4(1.0, 1.0, 1.0, 1.0);
    vec4 bg_color = vec4(0,0,0,bg_alpha);
    float alpha = texelFetch(text, ivec2(int(TexCoords.x),int(TexCoords.y)),0).r;
//    float alpha = texture(text, TexCoords).r;
    // color = vec4(mix(bg_color.rgb,textColor,alpha), 1.0);
    if (alpha == 0.0) {
        color = bg_color;
    } else {
        color = vec4(textColor, 1.0);
    }
    //color = vec4(mix(bg_color.rgb,textColor,alpha), clamp(bg_alpha + alpha, 0.0, 1.0));

}
