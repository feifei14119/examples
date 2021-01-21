//////////////////////////////////////////////////////////////////////////////
//
//  Triangles.cpp
//
//////////////////////////////////////////////////////////////////////////////

#include "vgl.h"
#include "LoadShaders.h"
#include <stdio.h>

enum VAO_IDs { Triangles, NumVAOs };
enum Buffer_IDs { ArrayBuffer, NumBuffers };
enum Attrib_IDs { vPosition = 0 };

GLuint  VAOs[NumVAOs];
GLuint  Buffers[NumBuffers];

const GLuint  NumVertices = 3;

//----------------------------------------------------------------------------
//
// init
//

void
init( void )
{
    glGenVertexArrays( NumVAOs, VAOs );		// create 1 Vtx objs
    glBindVertexArray( VAOs[Triangles] );

    // vertext host
    GLfloat  vertices[NumVertices][2] = 
	{
		//   X,      Y 
        { -0.90f, -0.90f }, 
		{  0.85f, -0.90f }, 
		{ -0.90f,  0.85f },  // Triangle 1
    };

    glCreateBuffers( NumBuffers, Buffers ); // create 1 buffer objs
    glBindBuffer( GL_ARRAY_BUFFER, Buffers[ArrayBuffer] );
    glBufferStorage( GL_ARRAY_BUFFER, sizeof(vertices), vertices, 0); // Memcpy H2D: from vertices(host) to active(bind) buffer obj 
	//glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	
	// feifei test: read buffer back
	//GLfloat readback[12];
	//glGetNamedBufferSubData(Buffers[0], 0, sizeof(vertices), readback);

	// feifei test: map buffer to cpu
	// only for glBufferData()
	//GLfloat * mapped_mem = (GLfloat*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);

    ShaderInfo  shaders[] =
    {
        { GL_VERTEX_SHADER, "media/shaders/triangles/triangles.vert" },
        { GL_FRAGMENT_SHADER, "media/shaders/triangles/triangles.frag" },
        { GL_NONE, NULL }
    };

    GLuint program = LoadShaders( shaders );
    glUseProgram( program );

    glVertexAttribPointer( vPosition, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
    glEnableVertexAttribArray( vPosition );
}

//----------------------------------------------------------------------------
//
// display
//

void
display( void )
{
    static const float black[] = { 0.0f, 0.0f, 0.0f, 0.0f };

    glClearBufferfv(GL_COLOR, 0, black);

    glBindVertexArray( VAOs[Triangles] );
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);
	//glPointSize(15.0f); glDrawArrays(GL_POINTS, 0, NumVertices);
	//glLineWidth(5.0f); glDrawArrays(GL_LINES, 0, NumVertices);

   // GLfloat readback[3*2];
    //glGetNamedBufferSubData(Buffers[0], 0, sizeof(GLfloat) * 2 * NumVertices, readback);
}

//----------------------------------------------------------------------------
//
// main
//

#ifdef _WIN32
#define _main() \
CALLBACK WinMain( \
  _In_ HINSTANCE hInstance, \
  _In_ HINSTANCE hPrevInstance, \
  _In_ LPSTR     lpCmdLine, \
  _In_ int       nCmdShow \
)
#else
#define _main() \
main( int argc, char** argv )
#endif

int _main()
{
    glfwInit();

    GLFWwindow* window = glfwCreateWindow(800, 600, "Triangles", NULL, NULL);

    glfwMakeContextCurrent(window);
    gl3wInit();

    init();

    while (!glfwWindowShouldClose(window))
    {
        display();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();
}
