#version 330

// Varying変数: 頂点シェーダから受け取った値
// Varying variables: received from a vertex shader.
in vec3 f_positionCameraSpace;
in vec3 f_normalCameraSpace;
in vec3 f_lightPosCameraSpace;

// Varying変数: ディスプレイへの出力変数
// Varying variables: here for exporting colors to the display
out vec4 out_color;

// マテリアルのデータ
// Material parameters
uniform vec3 u_diffColor;
uniform vec3 u_specColor;
uniform vec3 u_ambiColor;
uniform float u_shininess;

void main() {
    vec3 V = normalize(-f_positionCameraSpace);
    vec3 N = normalize(f_normalCameraSpace);
    vec3 L = normalize(f_lightPosCameraSpace - f_positionCameraSpace);
    vec3 H = normalize(V + L);

    float ndotl = max(0.0, dot(N, L));
    float ndoth = max(0.0, dot(N, H));
    vec3 diffuse = u_diffColor * ndotl;
    vec3 specular = u_specColor * pow(ndoth, u_shininess);
    vec3 ambient = u_ambiColor;

    // out_color = vec4(diffuse + specular + ambient, 1.0);
    out_color = vec4(N, 1.0);
}