#version 460

// Input Variables ======================================================
in vec2 f_texCoord;
in vec3 f_positionCameraSpace;
in vec3 f_normalCameraSpace;
in vec3 f_lightPosCameraSpace;
in float f_faceGroupID;


// Uniform Variables ====================================================
uniform sampler2D u_texture;
uniform float u_diffusionParam;
uniform float u_specularParam;
uniform float u_ambientParam;
uniform int u_selectMode;
uniform int u_selectedFaceID;

// Output Variables =====================================================
out vec4 out_color;

vec4 shade(vec3 V, vec3 N, vec3 L, vec3 H, vec3 texRGB) {
    // float diffuse  = u_diffusionParam * max(dot(N, L), 0.0);
    // float specular = pow(max(dot(N, H), 0.0), u_specularParam);
    // float ambient  = u_ambientParam;

    // vec3 color = texRGB * (diffuse + specular + ambient);
    vec3 color = texRGB;
    
    return vec4(color, 1.0);
}

void main() {
    if (u_selectMode > 0.5) {
        // 選択のIDが0より大きい => 選択モード, IDで描画する
        float c = float(f_faceGroupID + 1.0) / 255.0;
        out_color = vec4(c, c, c, 1.0);
    } else {
        // IDが0より小さい => 描画モード, 通常の色を使う
        vec3 texRGB = texture(u_texture, f_texCoord).rgb;
    
        vec3 V = normalize(-f_positionCameraSpace);
        vec3 L = normalize(f_lightPosCameraSpace - f_positionCameraSpace);
        vec3 H = normalize(V + L);
        vec3 N = normalize(f_normalCameraSpace);
        
        out_color = shade(V, N, L, H, texRGB);
        if ((u_selectedFaceID != -1) && (u_selectedFaceID != f_faceGroupID)) {
            out_color *= 0.3;
        }
    }
}