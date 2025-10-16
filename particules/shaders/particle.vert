#version 430
layout (location=0) in vec3 position;
layout (location=1) in vec4 color;

uniform mat4 mvp;

// Si utilisation du temps
uniform float time = 0;

out vec3 initialVertPos;
out vec4 Color;

void main(void)
{
    // Position 3D gardée sur le GPU et envoyée au geometry shader
    initialVertPos = position + vec3(0, 0, -0.05) * time;
    
    // Position dans le viewport
    gl_Position = mvp * vec4(initialVertPos, 1.0);

    Color = color;
}