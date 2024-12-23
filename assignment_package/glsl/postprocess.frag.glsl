#version 330 core
in vec2 fs_UV;
uniform int u_MotionBlurEnabled;
uniform sampler2D u_RenderedTexture;
uniform sampler2D u_History1;
uniform sampler2D u_History2;
uniform sampler2D u_History3;
uniform sampler2D u_History4;
uniform int u_InWater;
uniform int u_InLava;
uniform int u_Raining;

out vec4 out_Col;

float noise(vec2 st) {
    vec2 i = floor(st);
    vec2 f = fract(st);
    f = f * f * (3.0 - 2.0 * f);

    float a = fract(sin(dot(i, vec2(12.9898, 78.233))) * 43758.5453);
    float b = fract(sin(dot(i + vec2(1.0, 0.0), vec2(12.9898, 78.233))) * 43758.5453);
    float c = fract(sin(dot(i + vec2(0.0, 1.0), vec2(12.9898, 78.233))) * 43758.5453);
    float d = fract(sin(dot(i + vec2(1.0, 1.0), vec2(12.9898, 78.233))) * 43758.5453);

    return mix(mix(a, b, f.x),
              mix(c, d, f.x), f.y);
}

vec4 applyMotionBlur() {
    vec4 current = texture(u_RenderedTexture, fs_UV);
    if (u_MotionBlurEnabled == 0) {
        return current;
    }

    vec4 prev1 = texture(u_History1, fs_UV);
    vec4 prev2 = texture(u_History2, fs_UV);
    vec4 prev3 = texture(u_History3, fs_UV);
    vec4 prev4 = texture(u_History4, fs_UV);

    float weight_current = 0.45;
    float weight_prev1 = 0.25;
    float weight_prev2 = 0.15;
    float weight_prev3 = 0.1;
    float weight_prev4 = 0.05;

    return current * weight_current +
           prev1 * weight_prev1 +
           prev2 * weight_prev2 +
           prev3 * weight_prev3 +
           prev4 * weight_prev4;
}


vec4 applyMotionBlur(vec2 uv) {
    vec4 current = texture(u_RenderedTexture, uv);
    vec4 prev1 = texture(u_History1, uv);
    vec4 prev2 = texture(u_History2, uv);
    vec4 prev3 = texture(u_History3, uv);
    vec4 prev4 = texture(u_History4, uv);

    float weight_current = 0.45;
    float weight_prev1 = 0.25;
    float weight_prev2 = 0.15;
    float weight_prev3 = 0.1;
    float weight_prev4 = 0.05;

    return current * weight_current +
           prev1 * weight_prev1 +
           prev2 * weight_prev2 +
           prev3 * weight_prev3 +
           prev4 * weight_prev4;
}

void main() {
    vec2 uv = fs_UV;
    vec4 finalColor;

    if (u_InWater == 1) {
        float scale = 8.0;
        float noiseVal = noise(uv * scale);
        float noiseVal2 = noise((uv + 0.4) * scale * 1.5);
        vec2 distortion = vec2(noiseVal + noiseVal2) * 0.02;
        vec4 blurredColor = applyMotionBlur(uv + distortion);
        finalColor = mix(blurredColor, vec4(0.0, 0.2, 1.0, 1.0), 0.2);
    }
    else if (u_InLava == 1) {
        float scale = 6.0;
        float noiseVal = noise(uv * scale);
        float noiseVal2 = noise((uv + 0.6) * scale * 1.2);
        vec2 distortion = vec2(noiseVal + noiseVal2) * 0.03;
        vec4 blurredColor = applyMotionBlur();
        finalColor = mix(blurredColor, vec4(1.0, 0.3, 0.0, 1.0), 0.3);
    }
    else {
        finalColor = applyMotionBlur();
    }

    out_Col = finalColor;
}
