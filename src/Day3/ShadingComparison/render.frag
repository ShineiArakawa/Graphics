#version 460

in vec3 f_worldPos;
in vec3 f_normal;
in vec3 f_positionCameraSpace;
in vec3 f_normalCameraSpace;
in vec3 f_lightPosCameraSpace;
in vec4 rgb;

uniform vec3 u_diffColor;
uniform vec3 u_specColor;
uniform vec3 u_ambiColor;
uniform float u_shininess;
uniform int u_algorithmID;

out vec4 out_color;

void shade(vec3 N, vec3 L, vec3 H) {
    float ndotl = max(0.0, dot(N, L));
    float ndoth = max(0.0, dot(N, H));

    vec3 diffuse = u_diffColor * ndotl;
    vec3 specular = u_specColor * pow(ndoth, u_shininess);
    vec3 ambient = u_ambiColor;
    out_color = vec4(diffuse + specular + ambient, 1.0);
}

void normalShading() {
    vec3 dfdx = dFdx(f_worldPos);
    vec3 dfdy = dFdy(f_worldPos);
    
    vec3 rgb = (normalize(cross(dfdx, dfdy)) + 1.0) / 2.0;
    out_color = vec4(rgb, 1.0);
}

void main() {
    if (u_algorithmID == 1) {
        vec3 V = normalize(-f_positionCameraSpace);
        vec3 L = normalize(f_lightPosCameraSpace - f_positionCameraSpace);
        vec3 H = normalize(V + L);
        vec3 N = normalize(f_normalCameraSpace);
        shade(N, L, H);
    } else if (u_algorithmID == 2) {
       out_color = rgb;
    } 
    else  {
        normalShading();
    }
}