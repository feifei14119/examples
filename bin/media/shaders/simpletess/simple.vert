#version 400 core

//uniform mat4  MV;
//uniform mat4  P;

in  vec4  vPosition;

void
main()
{
    gl_Position = vPosition;
    //gl_Position = P * MV * vPosition;
}
