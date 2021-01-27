/* $URL$
   $Rev$
   $Author$
   $Date$
   $Id$
 */

#define USE_GL3W
#include <vermilion.h>

#include "vapp.h"
#include "vutils.h"
#include "vbm.h"

#include "vmath.h"
#include "LoadShaders.h"

#include <stdio.h>

using namespace vmath;

#define BUFFER_OFFSET(a) ((void*)(a))


#define GL_PI  3.141592654f
#define GL_2PI  6.283185307f
#define GL_1DIVPI  0.318309886f
#define GL_1DIV2PI  0.159154943f
#define GL_PIDIV2  1.570796327f
#define GL_PIDIV4  0.785398163f

GLuint VAO;     // 顶点对象
GLuint TexObj;  // 纹理对象

GLuint IndexBuffer;     // 索引显存 (EBO???)
GLuint ElemBufferObj;   // 顶点显存（位置+颜色）(vbo???)
//GLuint SampBufferObj;   // 纹理索引显存

GLuint vbByteSize;      // 顶点显存大小
GLuint ibByteSize;      // 索引显存大小
GLuint clByteSize;      // 颜色显存大小
GLuint smpByteSize;     // 纹理索引显存大小

mat4 ObjMoveProj;       // MoveMatrix 常量缓冲区
mat4 WorldViewProj;     // ProjMatrix 常量缓冲区
GLint MoveMatrixId;     // shader 中 MoveMatrix 变量的id
GLint ProjMatrixId;     // shader 中 ProjMatrix 变量的id

int win_height = 600;
int win_width = 800;
float cursor_x = 0.f;
float cursor_y = 0.f;
float last_cursor_x = 0.f;
float last_cursor_y = 0.f;
bool left_down = false;
bool right_down = false;
float mRadius = 1.0f;           // 摄像机和原点的距离
float mTheta = GL_PIDIV4;       // 摄像机在xz平面和x轴的夹角
float mPhi = GL_PIDIV4;         // 摄像机和y轴的夹角
float mZoom = 1.0f;             // 缩放
float mMoveX = 0.0f;            // X轴平移
float mMoveY = 0.0f;            // Y轴平移
float mMoveZ = 0.0f;            // Z轴平移
float mRotX = 0.0f;             // X轴旋转
float mRotY = 0.0f;             // Y轴旋转
float mRotZ = 0.0f;             // Z轴旋转

GLenum PolyMode = GL_LINE;      // GL_FILL / GL_LINE;
GLuint program;

void InitGeometry()
{
    GLfloat  vertices[] =
    {
        -1.0f, -1.0f, -1.0f, 1.0f,
        -1.0f, +1.0f, -1.0f, 1.0f,
        +1.0f, +1.0f, -1.0f, 1.0f,
        +1.0f, -1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, +1.0f, 1.0f,
        -1.0f, +1.0f, +1.0f, 1.0f,
        +1.0f, +1.0f, +1.0f, 1.0f,
        +1.0f, -1.0f, +1.0f, 1.0f
    };
    GLfloat colors[] =
    {
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 1.0f, 1.0f,
        0.0f, 1.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f
    };
    GLuint indices[] =
    {
        // front face
        0, 1, 2,
        0, 2, 3,

        // back face
        4, 6, 5,
        4, 7, 6,

        // left face
        4, 5, 1,
        4, 1, 0,

        // right face
        3, 2, 6,
        3, 6, 7,

        // top face
        1, 5, 6,
        1, 6, 2,

        // bottom face
        4, 0, 3,
        4, 3, 7
    };
    GLfloat sampler[] =
    {
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f
    };

    static const GLfloat quad_data[] =
    {
         1.0f, -1.0f,
        -1.0f, -1.0f,
        -1.0f, 1.0f,
         1.0f, 1.0f,

         0.0f, 0.0f,
         1.0f, 0.0f,
         1.0f, 1.0f,
         0.0f, 1.0f
    };

    vbByteSize = sizeof(vertices);
    ibByteSize = sizeof(indices);
    clByteSize = sizeof(colors);
    smpByteSize = sizeof(sampler);

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

     glGenBuffers(1, &IndexBuffer);
     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBuffer);
     glBufferData(GL_ELEMENT_ARRAY_BUFFER, ibByteSize, indices, GL_STATIC_DRAW);

     glGenBuffers(1, &ElemBufferObj);
     glBindBuffer(GL_ARRAY_BUFFER, ElemBufferObj);
     glBufferData(GL_ARRAY_BUFFER, vbByteSize + clByteSize + smpByteSize, NULL, GL_STATIC_DRAW);
     
     glBufferSubData(GL_ARRAY_BUFFER, 0,                         vbByteSize, vertices);
     glBufferSubData(GL_ARRAY_BUFFER, vbByteSize,                clByteSize, colors);
     glBufferSubData(GL_ARRAY_BUFFER, vbByteSize + clByteSize,   smpByteSize, sampler);



   // glGenBuffers(1, &ElemBufferObj);
   // glBindBuffer(GL_ARRAY_BUFFER, ElemBufferObj);
   // glBufferData(GL_ARRAY_BUFFER, sizeof(quad_data), quad_data, GL_STATIC_DRAW);

    ObjMoveProj = mat4(
        vec4(1.0f, 0, 0, 0),
        vec4(0, 1.0f, 0, 0),
        vec4(0, 0, 1.0f, 0),
        vec4(0, 0, 0, 1.0f));
    WorldViewProj = mat4(
        vec4(1.0f, 0, 0, 0),
        vec4(0, 1.0f, 0, 0),
        vec4(0, 0, 1.0f, 0),
        vec4(0, 0, 0, 1.0f));
}
void InitShader()
{
    ShaderInfo  shaders[] =
    {
        { GL_VERTEX_SHADER, "./Shader/triangles.vert" },
        { GL_FRAGMENT_SHADER, "./Shader/triangles.frag" },
        //{ GL_TESS_CONTROL_SHADER,    "./Shader/simple.cont" },
        //{ GL_TESS_EVALUATION_SHADER, "./Shader/simple.eval" },
        //{ GL_GEOMETRY_SHADER, "./Shader/simple.geom"},
        { GL_NONE, NULL }
    };

    program = LoadShaders(shaders);
    glUseProgram(program);

     MoveMatrixId = glGetUniformLocation(program, "MoveMatrix");
     ProjMatrixId = glGetUniformLocation(program, "ProjMatrix");

     glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
     glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(vbByteSize));
     glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(vbByteSize + clByteSize));
     glEnableVertexAttribArray(0);
     glEnableVertexAttribArray(1);
     glEnableVertexAttribArray(2);



  //  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
  //  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(8 * sizeof(float)));
  //
  //  glEnableVertexAttribArray(0);
  //  glEnableVertexAttribArray(1);
}
void InitTexture()
{
    vglImageData image;
    TexObj = vglLoadTexture("./test.dds", 0, &image);

    glTexParameteri(image.target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
}
void Init(void)
{
    InitGeometry();
    InitShader();
    InitTexture();

    glPolygonMode(GL_FRONT_AND_BACK, PolyMode);
    glViewport(0, 0, win_width, win_height);
    glLineWidth(2.0f);
}

void window_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    win_width = width;
    win_height = win_height;
}
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    float move_step = 0.02f;
    float rot_step = 0.5f;

    if (action == GLFW_PRESS)
    {
        switch (key)
        {
        case GLFW_KEY_M:
            PolyMode = (PolyMode == GL_FILL ? GL_LINE : GL_FILL);
            glPolygonMode(GL_FRONT_AND_BACK, PolyMode);
            break;
        case GLFW_KEY_P:
            mRadius = 0.12f;
            mTheta = 0.77667f;
            mPhi = 1.3875f;
            mZoom = 1.0f;
            mMoveX = 0;
            mMoveY = 0;
            mMoveZ = 0;
            mRotX = 0;
            mRotY = 22.5f;
            mRotZ = -1.0f;
            break;
        default:break;
        }
    }

    if ((action == GLFW_PRESS) || (action == GLFW_REPEAT))
    {
        switch (key)
        {
        case GLFW_KEY_Q:    mMoveX = mMoveX - move_step;    break;
        case GLFW_KEY_W:    mMoveX = mMoveX + move_step;    break;
        case GLFW_KEY_A:    mMoveY = mMoveY + move_step;    break;
        case GLFW_KEY_S:    mMoveY = mMoveY - move_step;    break;
        case GLFW_KEY_Z:    mMoveZ = mMoveZ + move_step;    break;
        case GLFW_KEY_X:    mMoveZ = mMoveZ - move_step;    break;

        case GLFW_KEY_E:    mRotX = mRotX + rot_step;       break;
        case GLFW_KEY_R:    mRotX = mRotX - rot_step;       break;
        case GLFW_KEY_D:    mRotY = mRotY + rot_step;       break;
        case GLFW_KEY_F:    mRotY = mRotY - rot_step;       break;
        case GLFW_KEY_C:    mRotZ = mRotZ + rot_step;       break;
        case GLFW_KEY_V:    mRotZ = mRotZ - rot_step;       break;

        case GLFW_KEY_T:    mRadius = mRadius + move_step;  break;  // 摄像机远离原点
        case GLFW_KEY_Y:    mRadius = mRadius - move_step;  break;  // 摄像机拉近原点
        case GLFW_KEY_G:    mTheta = mTheta + rot_step * GL_PI / 180;     break;  // 
        case GLFW_KEY_H:    mTheta = mTheta - rot_step * GL_PI / 180;     break;
        case GLFW_KEY_B:    mPhi = mPhi + rot_step * GL_PI / 180;         break;
        case GLFW_KEY_N:    mPhi = mPhi - rot_step * GL_PI / 180;         break;

        default:break;
        }
    }
}
void scroll_callback(GLFWwindow* window, double x, double y)
{
    mZoom += (0.1f * (float)y);
    if (mZoom <= 0) mZoom = 0.001f;
    return;
}
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        if (action == GLFW_PRESS)
        {
            last_cursor_x = cursor_x;
            last_cursor_y = cursor_y;
            left_down = true;
        }
        else if (action == GLFW_RELEASE)
        {
            left_down = false;
        }
    }
    else if (button == GLFW_MOUSE_BUTTON_RIGHT)
    {
        if (action == GLFW_PRESS)
        {
            last_cursor_x = cursor_x;
            last_cursor_y = cursor_y;
            right_down = true;
        }
        else if (action == GLFW_RELEASE)
        {
            right_down = false;
        }
    }
}
void cursor_position_callback(GLFWwindow* window, double x, double y)
{
    cursor_x = (float)x;
    cursor_y = (float)y;

    if (left_down == true)
    {
        // 转为弧度
        float dx = (0.005f * (float)(x - last_cursor_x)) * (GL_PI / 180.0f);
        float dy = (0.005f * (float)(y - last_cursor_y)) * (GL_PI / 180.0f);

        // Update angles based on input to orbit camera around box.
        mTheta += dx;
        mPhi += dy;

        // Restrict the angle mPhi.
        if (mPhi >= GL_PI - 0.1f)   mPhi = GL_PI - 0.1f;
        if (mPhi <= 0.1f)    mPhi = 0.1f;
    }
    if (right_down == true)
    {
    }
}

void Update()
{
    float x = mRadius * sinf(mPhi) * cosf(mTheta);
    float y = mRadius * cosf(mPhi);
    float z = mRadius * sinf(mPhi) * sinf(mTheta);

    vec3 eye = vec3(x, y, z);
    vec3 center = vec3(0, 0, 0);
    vec3 up = vec3(0.0f, 1.0f, 0.0f);
    mat4 view = lookat(eye, center, up);

    mat4 scl = scale<float>(mZoom);
    mat4 rot = rotate<float>(mRotX, mRotY, mRotZ);
    mat4 mov = translate<float>(mMoveX, mMoveY, mMoveZ);
    mat4 world = scl * mov * rot;

    //mat4 proj = frustum(-10.0f, 10.0f, -10.0f, 10.0f, 10.0f, 10.0f);
    mat4 proj = ortho(-2.0f, 2.0f, -2.0f, 2.0f, -2.0f, 2.0f);
    WorldViewProj = view * world * proj;
}
void Draw(void)
{
    static const float black[] = { 0, 0, 0, 0 };

    glClearBufferfv(GL_COLOR, 0, black);

    //glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
    glClearDepth(1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_CULL_FACE);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBuffer);

    glUniformMatrix4fv(MoveMatrixId, 1, GL_FALSE, ObjMoveProj);
    glUniformMatrix4fv(ProjMatrixId, 1, GL_FALSE, WorldViewProj);

    //glDrawArrays(GL_TRIANGLE_FAN, 0, 8); // GL_TRIANGLES / GL_TRIANGLE_FAN
    glDrawElements(GL_TRIANGLE_STRIP, 36, GL_UNSIGNED_INT, 0); // GL_LINES / GL_LINE_STRIP / GL_LINE_LOOP 


    //glUseProgram(program);
    //glBindVertexArray(VAO);
    //glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}
int CALLBACK WinMain(_In_ HINSTANCE hInstance, _In_ HINSTANCE hPrevInstance, _In_ LPSTR     lpCmdLine, _In_ int       nCmdShow)
{
    // init glfw
    glfwInit();
    GLFWwindow* window = glfwCreateWindow(win_width, win_height, "Minimal", NULL, NULL);
    glfwMakeContextCurrent(window);

    // glad
    //gladLoadGL(glfwGetProcAddress);

    gl3wInit();

    // set massage functions
    glfwSetWindowSizeCallback(window, window_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetScrollCallback(window, scroll_callback);

    Init();

    while (!glfwWindowShouldClose(window))
    {
        Update();
        Draw();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
}
