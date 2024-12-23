#version 150
in vec4 fs_Col;

out vec4 out_Col;

void main()
{
    out_Col = vec4(1-fs_Col.x, 1-fs_Col.y, 1-fs_Col.z, 1);
}
