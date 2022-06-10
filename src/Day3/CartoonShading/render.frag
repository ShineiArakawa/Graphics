#version 460

in vec3 f_positionCameraSpace;
in vec3 f_normalCameraSpace;
in vec3 f_lightPosCameraSpace;

out vec4 out_color;

uniform sampler2D u_texture;
uniform sampler2D u_textureDots;
uniform sampler2D u_textureLines;
uniform float u_contourThreshold;
uniform float u_diffuseThreshold;
uniform float u_specularThreshold;
uniform vec2 u_textureSize;

void main() {
    
    vec3 V = normalize(-f_positionCameraSpace);
    vec3 N = normalize(f_normalCameraSpace);
    vec3 L = normalize(f_lightPosCameraSpace - f_positionCameraSpace);
    vec3 H = normalize(V + L);

    vec2 uvScreen;
    uvScreen.x = (gl_FragCoord.x) / u_textureSize.x;
    uvScreen.y = -(gl_FragCoord.y) / u_textureSize.y;

    float diffuse = clamp(dot(N, L), 0.0, 1.0);
    float specular  = clamp(dot(N, H), 0.0, 1.0);
    
    vec3 rgb = texture(u_texture, vec2(dot(N, L), 0.5)).rgb;
    
    // Contour Lines
    if (abs(N[2]) < u_contourThreshold){
        rgb[0] = 0.0f;
        rgb[1] = 0.0f;
        rgb[2] = 0.0f;
    }
    // Lines
    if (diffuse < u_diffuseThreshold) {
        vec3 rgbLine = texture(u_textureLines, uvScreen).rgb;
        if (rgbLine.x < 0.5 && rgbLine.y < 0.5 && rgbLine.z < 0.5) {
            rgb = rgbLine;
        }
    }

    // Dots
    if (specular > u_specularThreshold) {
        rgb += texture(u_textureDots, uvScreen).rgb;
    }


    out_color = vec4(rgb, 1.0);
}