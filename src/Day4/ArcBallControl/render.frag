#version 460

// Varying�ϐ�
in vec3 f_fragColor;

// �f�B�X�v���C�ւ̏o�͕ϐ�
out vec4 out_color;

void main() {
    // �`��F����
    out_color = vec4(f_fragColor, 1.0);
}