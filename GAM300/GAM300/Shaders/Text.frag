#version 330 core
in vec2 TexCoords;
out vec4 color;

uniform sampler2D text;
uniform vec3 textColor;
uniform float AlphaScaler;

uniform float gammaCorrection;

void main()
{    
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
    color = vec4(textColor, AlphaScaler) * sampled;
    color.rgb = pow(color.rgb , vec3(gammaCorrection));
}  