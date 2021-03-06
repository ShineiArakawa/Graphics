#version 460

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;

uniform mat4 u_mvpMat;
uniform vec3 u_lightPos;
uniform mat4 u_mvMat;
uniform mat4 u_normMat;
uniform mat4 u_lightMat;
uniform vec3 u_diffColor;
uniform vec3 u_specColor;
uniform vec3 u_ambiColor;
uniform float u_shininess;
uniform int u_algorithmID;

out vec3 f_worldPos;
out vec3 f_normal;
out vec3 f_positionCameraSpace;
out vec3 f_normalCameraSpace;
out vec3 f_lightPosCameraSpace;
out vec4 rgb;

vec4 shade(vec3 N, vec3 L, vec3 H) {
    float ndotl = max(0.0, dot(N, L));
    float ndoth = max(0.0, dot(N, H));

    vec3 diffuse = u_diffColor * ndotl;
    vec3 specular = u_specColor * pow(ndoth, u_shininess);
    vec3 ambient = u_ambiColor;
    vec4 rgb = vec4(diffuse + specular + ambient, 1.0);

    return rgb;
}

void main() {
    f_worldPos = in_position;

    gl_Position = u_mvpMat * vec4(in_position, 1.0);

    f_normal = in_normal;

    f_positionCameraSpace = (u_mvMat * vec4(in_position, 1.0)).xyz;
    f_normalCameraSpace = (u_normMat * vec4(in_normal, 0.0)).xyz;
    f_lightPosCameraSpace = (u_lightMat * vec4(u_lightPos, 1.0)).xyz;

    if (u_algorithmID == 2) {
        vec3 V = normalize(-f_positionCameraSpace);
        vec3 L = normalize(f_lightPosCameraSpace - f_positionCameraSpace);
        vec3 H = normalize(V + L);
        vec3 N = normalize(f_normalCameraSpace);
        rgb = shade(N, L, H);
    }
}