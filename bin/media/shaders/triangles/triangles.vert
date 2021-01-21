
#version 400 core

layout( location = 0 ) in vec4 vPosition; // ??????????why can not read back vPos even ue inout 

void
main()
{
    gl_Position = vPosition;
    //vPosition.x = 14119.0f;
}
