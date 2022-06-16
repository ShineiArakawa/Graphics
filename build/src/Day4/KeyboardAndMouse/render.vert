#version 330

// Attribute�ϐ�: ���_�̎��������(�ʒu��F�Ȃ�)��\��
// Attribute variables: specifies vertex attributes, e.g., positions and colors.
layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_color;

// Varying�ϐ�: �V�F�[�_�Ԃ̕ϐ��̎󂯓n���ɗp����
// Varying variables: are used for sharing variables between shader stages.
out vec3 f_fragColor;

// Uniform�ϐ�: �S�ẴV�F�[�_�œ����l�����p�����[�^����
// Uniform variables: are parameters shared among all the shaders
uniform mat4 u_mvpMat;

void main() {
    // gl_Position�͒��_�V�F�[�_�̑g�ݍ��ݕϐ��D�w���Y���ƃG���[�ɂȂ�̂Œ���
    // You must specify "gl_Position", which is a build-in variable for vertex shader
    gl_Position = u_mvpMat * vec4(in_position, 1.0);

    // Varying�ϐ��ւ̑��
    // Store color data to varying variable
    f_fragColor = in_color;
}