#version 430 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in float aNormRadius;

uniform sampler1D RadialMapTexture;

uniform mat4 VPM;

void main()
{
    float height = texture(RadialMapTexture, aNormRadius).r - 4.0f;

    // going from world position to clip space
    gl_Position = VPM * vec4(aPos.x, height, aPos.y, 1.0);
}
