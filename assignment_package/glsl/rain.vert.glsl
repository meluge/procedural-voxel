#version 150

uniform mat4 u_Model;
uniform mat4 u_ViewProj;

in vec4 vs_Pos;

in int vs_ID;
uniform float u_Time;

out vec4 fs_Col;

void main()
{
    fs_Col = vec4(0.6, 0.6, 0.8, 0.4);
        vec4 pos = vs_Pos;
        float speed = 50.0;
        float height = 130.0;
        float minHeight = 40.0;
        float offset = float(vs_ID) * 0.1;


        pos.y = pos.y - mod((u_Time * 0.008 + offset) * speed, height);

        gl_Position = u_ViewProj * u_Model * pos;

}
