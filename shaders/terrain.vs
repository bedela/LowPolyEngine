#version 330 core
precision highp float;
attribute vec4 aPos;
attribute vec3 in_Color;
attribute vec3 normal;
varying  vec3 ex_Color;
varying vec4 shadowCoord;
varying float bias;
uniform vec3 lightDir;
uniform float ambiantCoeff;
uniform vec3 cameraDir;
uniform vec3 lightColor;
uniform float specularStrength;

uniform mat4 mvp;
uniform mat4 lightMvp;

float diff = max(dot(normal, -lightDir), 0.0);
vec3 reflectDir = reflect(-lightDir, normal);
float spec = pow(max(dot(cameraDir, reflectDir), 0.0), 32);


vec3 ambiant = ambiantCoeff * lightColor;
vec3 diffuse = diff * lightColor;
vec3 specular = specularStrength * spec * lightColor;

const float shadowDistance = 1040;
const float transitionDistance = 15;

void main()
{
    ex_Color = (ambiant + diffuse + specular) * in_Color;
    gl_Position = mvp * aPos;
    shadowCoord = lightMvp * aPos;
    bias = 0.005*tan(acos(max(dot(normal, -lightDir), 0.0))); // cosTheta is dot( n,l ), clamped between 0 and 1
    bias = clamp(bias, 0,0.01);

    float distance = length(gl_Position.xyz);
    distance = distance - (shadowDistance - transitionDistance);
    distance = distance / transitionDistance;
    shadowCoord.w = clamp(1.0 - distance, 0.0, 1.0);
}