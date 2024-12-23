#version 150
uniform mat4 u_Model;
uniform mat4 u_ViewProj;
uniform float u_Time;

in vec4 vs_Pos;
in int vs_ID;

out vec4 fs_Col;

mat4 rotationX(float angle) {
    float s = sin(angle);
    float c = cos(angle);
    return mat4(
        1.0, 0.0, 0.0, 0.0,
        0.0, c, -s, 0.0,
        0.0, s, c, 0.0,
        0.0, 0.0, 0.0, 1.0
    );
}

mat4 rotationY(float angle) {
    float s = sin(angle);
    float c = cos(angle);
    return mat4(
        c, 0.0, s, 0.0,
        0.0, 1.0, 0.0, 0.0,
        -s, 0.0, c, 0.0,
        0.0, 0.0, 0.0, 1.0
    );
}

mat4 rotationZ(float angle) {
    float s = sin(angle);
    float c = cos(angle);
    return mat4(
        c, -s, 0.0, 0.0,
        s, c, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        0.0, 0.0, 0.0, 1.0
    );
}

void main()
{
    fs_Col = vec4(0.9, 0.9, 1.0, 0.8);

    vec4 pos = vs_Pos;
    float speed = 20.0;
    float height = 130.0;
    float offset = float(vs_ID) * 0.1;

    pos.y = pos.y - mod((u_Time * 0.004 + offset) * speed, height);

    vec4 centerPos = vec4(pos.x, pos.y, pos.z, 1.0);
    vec4 localPos = pos - centerPos;

    float rotSpeed = 1.0 + sin(float(vs_ID) * 0.1) * 0.5;

    mat4 rotX = rotationX(u_Time * 0.01 * rotSpeed);
    mat4 rotY = rotationY(u_Time * 0.015 * rotSpeed);
    mat4 rotZ = rotationZ(u_Time * 0.002 * rotSpeed);

    vec4 rotatedPos = rotX * rotY * rotZ * localPos;
    pos = centerPos + rotatedPos;

    float swayAmount = 0.5 + sin(float(vs_ID) * 0.2) * 0.3;
    pos.x += sin(u_Time * 0.001 + offset) * swayAmount;
    pos.z += cos(u_Time * 0.001 + offset) * swayAmount;

    gl_Position = u_ViewProj * u_Model * pos;
}
