#version 150
// ^ Change this to version 130 if you have compatibility issues

//This is a vertex shader. While it is called a "shader" due to outdated conventions, this file
//is used to apply matrix transformations to the arrays of vertex data passed to it.
//Since this code is run on your GPU, each vertex is transformed simultaneously.
//If it were run on your CPU, each vertex would have to be processed in a FOR loop, one at a time.
//This simultaneous transformation allows your program to run much faster, especially when rendering
//geometry with millions of vertices.

uniform mat4 u_Model;       // The matrix that defines the transformation of the
                            // object we're rendering. In this assignment,
                            // this will be the result of traversing your scene graph.

uniform mat4 u_ModelInvTr;  // The inverse transpose of the model matrix.
                            // This allows us to transform the object's normals properly
                            // if the object has been non-uniformly scaled.

uniform mat4 u_ViewProj;    // The matrix that defines the camera's transformation.
                            // We've written a static matrix for you to use for HW2,
                            // but in HW3 you'll have to generate one yourself

uniform vec3 u_PlayerPos;

uniform int u_IsSpherical;

uniform vec4 lightDir;

uniform vec4 u_Color;       // When drawing the cube instance, we'll set our uniform color to represent different block types.

in vec4 vs_Pos;             // The array of vertex positions passed to the shader

in vec4 vs_Nor;             // The array of vertex normals passed to the shader

in vec4 vs_UV;             // The array of vertex uvs passed to the shader.

out vec4 fs_Pos;
out vec4 fs_Nor;            // The array of normals that has been transformed by u_ModelInvTr. This is implicitly passed to the fragment shader.
out vec4 fs_LightVec;       // The direction in which our virtual light lies, relative to each vertex. This is implicitly passed to the fragment shader.
out vec4 fs_UV;            // The color of each vertex. This is implicitly passed to the fragment shader.

 // The direction of our virtual light, which is used to compute the shading of
                                        // the geometry in the fragment shader.
mat4 rodriguesRotation(float rad, vec3 axis) {
    float c = cos(rad);
    float s = sin(rad);
    float wx = axis.x;
    float wy = axis.y;
    float wz = axis.z;

    return mat4(
        c + wx*wx * (1 - c), wz * s + wx*wy * (1 - c), -wy * s + wx*wz * (1 - c), 0,
        -wz * s + wx*wy * (1 - c), c + wy*wy * (1 - c), wx * s + wy*wz * (1 - c), 0,
        wy * s + wx*wz * (1 - c), -wx * s + wy*wz * (1 - c), c + wz*wz * (1 - c), 0,
        0, 0, 0, 1
    );
}

void main()
{
    vec4 p = u_Model * vs_Pos;
    vec4 final_pos;

    if (u_IsSpherical == 1) {
        vec3 planet_center = vec3(u_PlayerPos.x, 64.0, u_PlayerPos.z);

        vec4 p_planet = p - vec4(planet_center, 0.0);

        vec2 xz_dist = p_planet.xz - vec2(0.0);
        float d = length(xz_dist);

        vec3 axis = normalize(vec3(-xz_dist.y, 0.0, xz_dist.x));

        float angle = -d / 512.0;

        mat4 rot_mat = rodriguesRotation(angle, axis);
        p_planet = rot_mat * p_planet;

        final_pos = p_planet + vec4(planet_center, 0.0);
    } else {
        final_pos = p;
    }

    fs_Pos = final_pos;
    fs_UV = vs_UV;
    mat3 invTranspose = mat3(u_ModelInvTr);
    fs_Nor = vec4(invTranspose * vec3(vs_Nor), 0);
    fs_LightVec = lightDir;

    gl_Position = u_ViewProj * final_pos;
}
