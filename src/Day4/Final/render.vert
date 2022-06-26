#version 460

// Input variable ======================================================
layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;

// Uniform variable ====================================================
uniform mat4 u_mvpMat;

// Output Variables =====================================================
out vec3 f_normal;

void main() {
    gl_Position = u_mvpMat * vec4(in_position, 1.0);

    f_normal = in_normal;
}