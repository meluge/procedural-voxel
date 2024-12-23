#version 330 core
// ^ Change this to version 130 if you have compatibility issues

// This is a fragment shader. If you've opened this file first, please
// open and read lambert.vert.glsl before reading on.
// Unlike the vertex shader, the fragment shader actually does compute
// the shading of geometry. For every pixel in your program's output
// screen, the fragment shader is run for every bit of geometry that
// particular pixel overlaps. By implicitly interpolating the position
// data passed into the fragment shader by the vertex shader, the fragment shader
// can compute what color to apply to its pixel based on things like vertex
// position, light position, and vertex color.

uniform sampler2D u_Texture; // The color with which to render this instance of geometry.
uniform vec4 u_Color; // The color with which to render this instance of geometry.
uniform int u_Time; // The color with which to render this instance of geometry.
uniform vec3 u_PlayerPos;

// These are the interpolated values out of the rasterizer, so you can't know
// their specific values without knowing the vertices that contributed to them
in vec4 fs_Pos;
in vec4 fs_Nor;
in vec4 fs_LightVec;
in vec4 fs_UV;
in vec4 fs_Col;

out vec4 out_Col;

void main()
{
    vec2 uv_offset = vec2(0.0f,0.0f);
    if (fs_UV.z != 0) {
       uv_offset = vec2(1.0f,-1.0f) + vec2(1.0f,1.0f) * (float(u_Time%int(fs_UV.w))/(fs_UV.w));
    }
    uv_offset /= 16.0f;

    vec4 diffuseColor = texture(u_Texture, (fs_UV.xy + uv_offset));
    if (diffuseColor.a <= 0.2f) {
        discard;
    }

    float sunHeight = normalize(fs_LightVec).y;
    float dayFactor = smoothstep(-0.2, 0.2, sunHeight);

    float diffuseTerm = dot(normalize(fs_Nor), normalize(fs_LightVec));
    diffuseTerm = clamp(diffuseTerm, 0, 1);

    float nightAmbient = 0.15;
    float dayAmbient = 0.2;
    float ambientTerm = mix(nightAmbient, dayAmbient, dayFactor);

    diffuseTerm *= mix(0.3, 1.0, dayFactor);

    float lightIntensity = diffuseTerm + ambientTerm;

    vec4 shadedColor = vec4(diffuseColor.rgb * lightIntensity, diffuseColor.a);

    vec3 fogColor = vec3(0.7, 0.8, 0.9);
    float fogStart = 33.0;
    float fogEnd = 110.0;
    float fragmentDistance = length(fs_Pos.xyz - u_PlayerPos);
    float fogFactor = clamp((fragmentDistance - fogStart) / (fogEnd - fogStart), 0.0, 1.0);

    fogColor = mix(vec3(0.1, 0.1, 0.2), fogColor, dayFactor);

    vec3 finalColor = mix(shadedColor.rgb, fogColor, fogFactor);
    out_Col = vec4(finalColor, shadedColor.a);
}
