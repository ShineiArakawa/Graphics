#version 460


in vec3 f_worldPos;
in vec2 f_texCoord;
in vec3 f_positionCameraSpace;
in vec3 f_normalCameraSpace;
in vec3 f_lightPosCameraSpace;

uniform sampler2D u_texture;
uniform sampler2D u_normalMap;
uniform vec3 u_diffColor;
uniform vec3 u_specColor;
uniform vec3 u_ambiColor;
uniform float u_shininess;
uniform int u_algorithmID;

out vec4 out_color;

void shade(vec3 N, vec3 L, vec3 H, vec3 rgb) {
    float diffuse   = clamp(dot(N, L), 0.0, 1.0);
    float specular  = pow(clamp(dot(N, H), 0.0, 1.0), u_shininess);
    float ambient = 0.8;
    rgb = clamp(rgb * (diffuse + specular + ambient), 0.0, 1.0);
    out_color = vec4(rgb, 1.0);
}

vec3 calcLocalNormal() {
    vec3 dPdx = dFdx(f_worldPos);
    vec3 dPdy = dFdy(f_worldPos);

    vec2 dUdx = dFdx(f_texCoord); /* (du/dx, dv/dx) */
    vec2 dUdy = dFdy(f_texCoord); /* (du/dy, dv/dy) */
    
    float det = abs(dUdx[0]*dUdy[1]-dUdx[1]*dUdy[0]);

    float invMat00 = dUdy[1] / det;
    float invMat01 = - dUdx[1] / det;
    float invMat10 = - dUdy[0] / det;
    float invMat11 = dUdx[0] / det;

    vec3 localX = normalize(invMat00 * dPdx + invMat01 * dPdy);
    vec3 localY = normalize(invMat10 * dPdx + invMat11 * dPdy);
    vec3 localZ = cross(localX, localY);

    vec4 normalFromMap = texture(u_normalMap, f_texCoord) * 2.0 - 1.0;
    vec3 N = normalize(normalFromMap[0] * localX + normalFromMap[1] * localY + normalFromMap[2] * localZ);

    return N;
}

void main()
{
    vec3 rgb = texture(u_texture, f_texCoord).rgb;

    vec3 V = normalize(-f_positionCameraSpace);
    vec3 L = normalize(f_lightPosCameraSpace - f_positionCameraSpace);
    vec3 H = normalize(V + L);
    vec3 N;

    if (u_algorithmID == 0) {
        N = normalize(f_normalCameraSpace);
        shade(N, L, H, rgb);
    } else if (u_algorithmID == 1){
        N = calcLocalNormal();
        shade(N, L, H, rgb);
    }

    /*
    float ndotl = max(0.0, dot(N, L));
    float ndoth = max(0.0, dot(N, H));

    vec3 diffuse = u_diffColor * ndotl;
    vec3 specular = u_specColor * pow(ndoth, u_shininess);
    vec3 ambient = u_ambiColor;
    out_color = vec4(diffuse + specular + ambient, 1.0);
    */

    /* Success
    float ndotl = max(0.0, dot(N, L));
    float ndoth = max(0.0, dot(N, H));
    vec3 diffuse = rgb * ndotl;
    vec3 specular = rgb * pow(ndoth, u_shininess);
    vec3 ambient = rgb * 0.5;
    out_color = vec4(diffuse + specular + ambient, 1.0);
    */

    /*
    vec3 diff = max(0, dot(N, L)) * u_diffColor;
    vec3 spec = pow(max(0, dot(N, H)), u_shininess) * u_diffColor;
    rgb = rgb * diff + spec;
    out_color = vec4(rgb, 1.0);
    */

    /*
    rgb = (N + 1.0) * 0.5; // For Debug
    out_color = vec4(rgb, 1.0);
    */

    /*
    vec4 tex = texture(u_normalMap, f_texCoord);
    // vec4 tex = texture(u_texture, f_texCoord);
    out_color = tex;
    */
    
    /*
    // Success
    float diffuse   = clamp(dot(N, L), 0.0, 1.0);
    float specular  = pow(clamp(dot(N, H), 0.0, 1.0), u_shininess);
    float ambient = 0.8;
    rgb = rgb * (diffuse + specular + ambient);
    out_color = vec4(rgb, 1.0);
    */
}
