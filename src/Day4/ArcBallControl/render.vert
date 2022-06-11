#version 460

// Attribute�ϐ�
layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_color;

// Varying�ϐ�
out vec3 f_fragColor;

// Uniform�ϐ�
uniform mat4 u_mvpMat;

void main() {
    // gl_Position�͒��_�V�F�[�_�̑g�ݍ��ݕϐ�
    // �w���Y���ƃG���[�ɂȂ�̂Œ���
    gl_Position = u_mvpMat * vec4(in_position, 1.0);

    // Varying�ϐ��ւ̑��
    f_fragColor = in_color;
}