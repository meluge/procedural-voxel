#version 150
in vec4 fs_Col;
in vec4 fs_Pos;
uniform vec3 u_PlayerPos;
out vec4 out_Col;

void main() {
    vec4 baseColor = fs_Col;
    vec3 fogColor = vec3(0.7, 0.8, 0.9);
    float fogStart = 50.0;
    float fogEnd = 150.0;

    float fragmentDistance = length(fs_Pos.xyz - u_PlayerPos);
    float fogFactor = clamp((fragmentDistance - fogStart) / (fogEnd - fogStart), 0.0, 1.0);

    float alphaStart = 130.0;
    float alphaEnd = 200.0;
    float alphaFactor = clamp((fragmentDistance - alphaStart) / (alphaEnd - alphaStart), 0.0, 1.0);
    float finalAlpha = baseColor.a * (1.0 - alphaFactor);

    vec3 finalColor = mix(baseColor.rgb, fogColor, fogFactor);
    out_Col = vec4(finalColor, finalAlpha);
}
