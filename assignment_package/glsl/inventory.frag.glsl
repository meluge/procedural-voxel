#version 330 core

in vec2 fs_UV;
flat in int fs_FaceID;

uniform sampler2D u_Texture;
uniform int u_IsSelected;
uniform int u_IsEmpty;
uniform int u_IsCrafting;
uniform float u_Time;

out vec4 out_Col;

void main() {
    float frontShade = 1.0;
    float topShade = 1.2;
    float rightShade = 0.8;

    float shadeFactor = rightShade;
    if(fs_FaceID == 1) shadeFactor = topShade;
    if(fs_FaceID == 2) shadeFactor = frontShade;

    if (u_IsEmpty == 1) {
        float outerBorder = 0.05;
        float midBorder = 0.04;
        float innerBorder = 0.03;

        vec4 bgColor = vec4(0.3, 0.3, 0.3, 0.7);
        vec4 darkestEdge = vec4(0.1, 0.1, 0.1, 0.8);
        vec4 darkEdge = vec4(0.15, 0.15, 0.15, 0.8);
        vec4 lightEdge = vec4(0.5, 0.5, 0.5, 0.8);
        vec4 lightestEdge = vec4(0.6, 0.6, 0.6, 0.8);

        float distFromLeft = fs_UV.x;
        float distFromRight = 1.0 - fs_UV.x;
        float distFromTop = 1.0 - fs_UV.y;
        float distFromBottom = fs_UV.y;

        if (distFromLeft < outerBorder || distFromTop < outerBorder) {
            out_Col = darkestEdge;
        }
        else if (distFromLeft < midBorder || distFromTop < midBorder) {
            out_Col = darkEdge;
        }
        else if (distFromLeft < innerBorder || distFromTop < innerBorder) {
            out_Col = mix(bgColor, darkEdge, 0.5);
        }
        else if (distFromRight < outerBorder || distFromBottom < outerBorder) {
            out_Col = lightestEdge;
        }
        else if (distFromRight < midBorder || distFromBottom < midBorder) {
            out_Col = lightEdge;
        }
        else if (distFromRight < innerBorder || distFromBottom < innerBorder) {
            out_Col = mix(bgColor, lightEdge, 0.5);
        }
        else {
            out_Col = bgColor;
        }

        if (out_Col == bgColor) {
            float gradientX = fs_UV.x * (1.0 - fs_UV.x) * 4.0;
            float gradientY = fs_UV.y * (1.0 - fs_UV.y) * 4.0;
            float gradient = gradientX * gradientY * 0.1;
            out_Col.rgb *= (1.0 + gradient);
        }
    }
    else {
        out_Col = texture(u_Texture, fs_UV);
        out_Col *= shadeFactor;

        if (u_IsSelected == 1) {
             out_Col.rgb *= 1.8;

             float borderWidth = 0.08;
             if (fs_UV.x < borderWidth || fs_UV.x > (1.0 - borderWidth) ||
                 fs_UV.y < borderWidth || fs_UV.y > (1.0 - borderWidth)) {

                 float highlightIntensity = 0.0;
                 if (fs_UV.x < borderWidth || fs_UV.y > (1.0 - borderWidth)) {
                     highlightIntensity = 0.4;
                 } else {
                     highlightIntensity = 0.2;
                 }
                 out_Col.rgb = mix(out_Col.rgb, vec3(1.0), highlightIntensity);
             }
         }
     }
 }
