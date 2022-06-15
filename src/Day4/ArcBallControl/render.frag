#version 460

// Input Variables ======================================================
in vec3 f_color;
in float f_cubeID;

// Uniform Variables ====================================================
uniform int u_selectMode;

// Output Variables =====================================================
out vec4 out_color;

void main() {
    if (u_selectMode > 0.5) {
        float c = float(f_cubeID + 1.0) / 255.0;
        out_color = vec4(c, c, c, 1.0);
    } else {
        out_color = vec4(f_color, 1.0);
    }
    // out_color = vec4(float(u_selectMode),0.0, 0.0, 1.0);
}