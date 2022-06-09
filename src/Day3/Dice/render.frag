#version 460

// Input Variables ======================================================
in vec2 f_texCoord;
in vec3 f_positionCameraSpace;
in vec3 f_normalCameraSpace;
in vec3 f_lightPosCameraSpace;

// Uniform Variables ====================================================
uniform sampler2D u_texture;
uniform float u_diffusionParam;
uniform float u_specularParam;
uniform float u_ambientParam;

// Output Variables =====================================================
out vec4 out_color;

vec4 shade(vec3 V, vec3 N, vec3 L, vec3 H, vec3 texRGB) {
    float diffuse  = u_diffusionParam * max(dot(N, L), 0.0);
    float specular = pow(max(dot(N, H), 0.0), u_specularParam);
    float ambient  = u_ambientParam;

    vec3 color = texRGB * (diffuse + specular + ambient);
    
    return vec4(color, 1.0);
}

void main() {
    vec3 texRGB = texture(u_texture, f_texCoord).rgb;

    vec3 V = normalize(-f_positionCameraSpace);
    vec3 L = normalize(f_lightPosCameraSpace - f_positionCameraSpace);
    vec3 H = normalize(V + L);
    vec3 N = normalize(f_normalCameraSpace);

    out_color = shade(V, N, L, H, texRGB);
}