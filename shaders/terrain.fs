#version 330 core
precision highp float;
in vec3 ex_Color;
in vec4 shadowCoord;
out vec4 out_Color;

uniform sampler2D shadowMap;

void main(void)
{
    float objectNear = texture(shadowMap, shadowCoord.xy).r;
    float lightFactor = 1.0;
    float bias = 0.005;
    if (shadowCoord.z - bias > objectNear)
        lightFactor = 0.6;
    out_Color = vec4(ex_Color * lightFactor, 1.0);
}
