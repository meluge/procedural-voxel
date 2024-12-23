#version 330 core

layout(location = 0) in vec4 vs_Pos;
layout(location = 1) in vec2 vs_UV;

uniform mat4 u_Model;
uniform mat4 u_ViewProj;


out vec2 fs_UV;
out vec3 fs_Normal;
flat out int fs_FaceID;

void main() {
    gl_Position = u_ViewProj * u_Model * vs_Pos;
    fs_UV = vs_UV;
    fs_FaceID = gl_VertexID / 4;

}
