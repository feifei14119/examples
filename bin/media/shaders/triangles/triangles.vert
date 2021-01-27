
#version 400 core

uniform mat4  TransMatrix;
uniform mat4  ProjMatrix;

layout( location = 0 ) in vec4 vPosition; // ??????????why can not read back vPos even ue inout 

void
main()
{
    gl_Position = vPosition;
    gl_Position.x = gl_Position.x + 1.0f;
    //vPosition.x = 14119.0f;
}
