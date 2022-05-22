#version 460

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_uv;

uniform mat4 u_mvpMat;
uniform vec3 u_lightPos;
uniform mat4 u_mvMat;
uniform mat4 u_normMat;
uniform mat4 u_lightMat;

out vec3 f_positionCameraSpace;
out vec3 f_normalCameraSpace;
out vec3 f_lightPosCameraSpace;
out vec2 f_texCoord;
out vec3 f_worldPos;

void main()
{
    f_worldPos = in_position;
    f_texCoord = in_uv;

    gl_Position = u_mvpMat * vec4(in_position, 1.0);

    f_positionCameraSpace = (u_mvMat * vec4(in_position, 1.0)).xyz;
    f_normalCameraSpace = (u_normMat * vec4(in_normal, 0.0)).xyz;
    f_lightPosCameraSpace = (u_lightMat * vec4(u_lightPos, 1.0)).xyz;
    
}