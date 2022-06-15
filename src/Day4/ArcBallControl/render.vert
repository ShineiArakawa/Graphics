#version 460

// Input variable ======================================================
layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_color;
layout(location = 2) in float cubeID;

// Uniform variable ====================================================
uniform mat4 u_mvpMat;

// Output Variables =====================================================
out vec3 f_color;
out float f_cubeID;

void main() {
    gl_Position = u_mvpMat * vec4(in_position, 1.0);

    f_color = in_color;
    f_cubeID = cubeID;
}