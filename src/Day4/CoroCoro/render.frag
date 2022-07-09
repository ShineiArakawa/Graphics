#version 330

in vec3 f_worldPos;
in vec3 f_fragColor;
in vec2 f_uv;

uniform float u_toUseTexture;
uniform sampler2D u_texture;
uniform vec3 u_position;

out vec4 out_color;

void main() {
    if (u_toUseTexture > 0.5 && u_toUseTexture < 1.5){
        out_color = texture(u_texture, f_uv);
    } else if (u_toUseTexture > -0.5 && u_toUseTexture < 0.5) {
        vec3 dfdx = dFdx(f_worldPos);
        vec3 dfdy = dFdy(f_worldPos);
        vec3 color = normalize(cross(dfdx, dfdy));
        color = (color + 1.0) / 2.0;
        out_color = vec4(color, 1.0);
    }
}