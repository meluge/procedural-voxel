#version 150

uniform mat4 u_Model;
uniform mat4 u_ViewProj;
uniform vec3 u_Center;
uniform float u_Time;

in vec4 vs_Pos;
in int vs_ID;

out vec4 fs_Col;

out vec4 fs_Pos;

void main() {
    vec4 pos = vs_Pos;

    float maxHeight = 70.0;
    float heightPercent = (pos.y - u_Center.y) / maxHeight;

    float baseSpeed = 2.0;
    float vertexOffset = float(vs_ID) * 0.5;
    float heightOffset = pos.y * 0.1;
    float angle = u_Time * 0.1 * baseSpeed + vertexOffset + heightOffset;

    vec2 toCenter = pos.xz - u_Center.xz;
    float dist = length(toCenter);

    float modifiedDist = dist * (0.4 + heightPercent * 1.0);

    pos.x = u_Center.x + modifiedDist * cos(angle);
    pos.z = u_Center.z + modifiedDist * sin(angle);

    gl_Position = u_ViewProj * u_Model * pos;
    fs_Col = vec4(1.0, 1.0, 1.0, 0.5);

    fs_Pos = pos;
}
