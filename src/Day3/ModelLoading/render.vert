#version 330

// Attribute�ϐ�: ���_�̎��������(�ʒu��F�Ȃ�)��\��
// Attribute variables: specifies vertex attributes, e.g., positions and colors.
layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;

// Varying�ϐ�: �V�F�[�_�Ԃ̕ϐ��̎󂯓n���ɗp����
// Varying variables: are used for sharing variables between shader stages.
out vec3 f_fragColor;
out vec3 f_worldPos;

// Uniform�ϐ�: �S�ẴV�F�[�_�œ����l�����p�����[�^����
// Uniform variables: are parameters shared among all the shaders
uniform mat4 u_mvpMat;

void main() {
    // gl_Position�͕K���w�肷��
    // You MUST specify gl_Position
    gl_Position = u_mvpMat * vec4(in_position, 1.0);

    // ����͕`��F��@����񂩂猈��
    // Here, pixel colors are determined by normal vectors
    f_fragColor = in_normal * 0.5 + 0.5;

    f_worldPos = in_position;
}