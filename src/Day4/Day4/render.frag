#version 330

// Varying�ϐ�: ���_�V�F�[�_����󂯎�����l
// Varying variables: received from a vertex shader.
in vec3 f_fragColor;

// Varying�ϐ�: �f�B�X�v���C�ւ̏o�͕ϐ�
// Varying variables: here for exporting colors to the display 
out vec4 out_color;

void main() {
    // �`��F���� / Store pixel colors
    out_color = vec4(f_fragColor, 1.0);
}