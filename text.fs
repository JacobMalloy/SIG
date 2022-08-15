#version 330 core
//layout (location = 1) in vec3 textColor; // <vec2 pos, vec2 tex>
in vec2 TexCoords;
in vec3 textColor;
in vec3 bg_textColor;
out vec4 color;

uniform sampler2D text;
uniform float bg_alpha;


vec4 fromLinear(vec4 linearRGB)
{
    bvec4 cutoff = lessThan(linearRGB, vec4(0.0031308));
    vec4 higher = vec4(1.055)*pow(linearRGB, vec4(1.0/2.4)) - vec4(0.055);
    vec4 lower = linearRGB * vec4(12.92);

    return mix(higher, lower, cutoff);
}

vec4 toLinear(vec4 sRGB)
{
    bvec4 cutoff = lessThan(sRGB, vec4(0.04045));
    vec4 higher = pow((sRGB + vec4(0.055))/vec4(1.055), vec4(2.4));
    vec4 lower = sRGB/vec4(12.92);

    return mix(higher, lower, cutoff);
}

void main()
{
    //vec4 sampled = vec4(1.0, 1.0, 1.0, 1.0);
    float alpha = texelFetch(text, ivec2(int(TexCoords.x),int(TexCoords.y)),0).r;
//    float alpha = texture(text, TexCoords).r;
    // color = vec4(mix(bg_color.rgb,textColor,alpha), 1.0);
    color = fromLinear(alpha*vec4(textColor,1) + (1-alpha) * vec4(bg_textColor,bg_alpha));
    //color = vec4(textColor,1)*vec4(1,1,1,alpha);
/*    if (alpha == 0.0) {
        color = bg_color;
    } else {
        color = vec4(textColor, 1.0);
    }*/

}
