#version 330

// Input variable ======================================================
layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_uv;
layout(location = 3) in float faceGroupID;

// Uniform variable ====================================================
uniform mat4 u_mvpMat;
uniform vec3 u_lightPos;
uniform mat4 u_mvMat;
uniform mat4 u_normMat;
uniform mat4 u_lightMat;

// Output Variables =====================================================
out vec2 f_texCoord;
out vec3 f_positionCameraSpace;
out vec3 f_normalCameraSpace;
out vec3 f_lightPosCameraSpace;
flat out float f_faceGroupID;

void main() {
    f_texCoord = in_uv;
    f_texCoord.y = - f_texCoord.y + 1.0;

    gl_Position = u_mvpMat * vec4(in_position, 1.0);

    f_positionCameraSpace = (u_mvMat * vec4(in_position, 1.0)).xyz;
    f_normalCameraSpace = (u_normMat * vec4(in_normal, 0.0)).xyz;
    f_lightPosCameraSpace = (u_lightMat * vec4(u_lightPos, 1.0)).xyz;

    f_faceGroupID = faceGroupID;
}