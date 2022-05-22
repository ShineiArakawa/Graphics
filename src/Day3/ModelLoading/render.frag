#version 330

in vec3 f_worldPos;
in vec3 f_fragColor;

out vec4 out_color;

void main() {
    vec3 dfdx = dFdx(f_worldPos);
    vec3 dfdy = dFdy(f_worldPos);
    vec3 color = cross(dfdx, dfdy);
    color = normalize(color);
    color = (color + 1.0) / 2.0;
    out_color = vec4(color, 1.0);
}