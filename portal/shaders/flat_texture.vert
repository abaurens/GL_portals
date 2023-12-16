#version 330 core

uniform mat4 MVP;

layout (location = 0) in vec3 vPos;
layout (location = 1) in vec2 vTex;

out vec2 uv;

void main()
{
    gl_Position = MVP * vec4(vPos, 1.0);
    uv = vTex;
}
