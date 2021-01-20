//////////////////////////////////////////////////////////////////////////////
//
//  Gouraud.cpp
//
//////////////////////////////////////////////////////////////////////////////

#include <vermilion.h>
#include "vapp.h"
#include "LoadShaders.h"

enum VAO_IDs { Triangles, NumVAOs };
enum Buffer_IDs { ArrayBuffer, NumBuffers };
enum Attrib_IDs { vPosition = 0, vColor = 1 };

GLuint  VAOs[NumVAOs];
GLuint  Buffers[NumBuffers];

const GLuint  NumVertices = 6;

BEGIN_APP_DECLARATION(GouraudExample)
	virtual void Initialize(const char * title);
	virtual void Display(bool auto_redraw);
	virtual void OnKey(int key, int scancode, int action, int mods);
	virtual void Resize(int width, int height);

	float aspect;
	int width; int height;
END_APP_DECLARATION()

DEFINE_APP(GouraudExample, "Gouraud Example")

//----------------------------------------------------------------------------
//
// init
//

void GouraudExample::Initialize(const char * title)
{
	base::Initialize(title);

	glGenVertexArrays( NumVAOs, VAOs );
	glBindVertexArray( VAOs[Triangles] );

	struct VertexData 
	{
		GLubyte color[4];
		GLfloat position[4];
	};

	VertexData vertices[NumVertices] =
	{
		{{ 255,   0,   0, 10 }, { -0.90f, -0.90f }},  // Triangle 1
		{{   0, 255,   0, 10 }, {  0.85f, -0.90f }},
		{{   0,   0, 255, 10 }, { -0.90f,  0.85f }},
		{{  10,  10,  10, 10 }, {  0.90f, -0.85f }},  // Triangle 2
		{{ 100, 100, 100, 10 }, {  0.90f,  0.90f }},
		{{ 255, 255, 255, 10 }, { -0.85f,  0.90f }}
	};

	glGenBuffers( NumBuffers, Buffers );
	glBindBuffer( GL_ARRAY_BUFFER, Buffers[ArrayBuffer] );
	glBufferData( GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW );

	ShaderInfo  shaders[] = 
	{
		{ GL_VERTEX_SHADER, "media/shaders/gouraud/gouraud.vert" },
		{ GL_FRAGMENT_SHADER, "media/shaders/gouraud/gouraud.frag" },
		{ GL_NONE, NULL }
	};

	GLuint program = LoadShaders( shaders );
	glUseProgram( program );

	glVertexAttribPointer( vColor, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(VertexData), BUFFER_OFFSET(0) );
	glVertexAttribPointer( vPosition, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), BUFFER_OFFSET(sizeof(vertices[0].color)) );
	glEnableVertexAttribArray( vColor );
	glEnableVertexAttribArray( vPosition );
}

void GouraudExample::OnKey(int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		switch (key)
		{
			case GLFW_KEY_M:
			{
				static GLenum mode = GL_FILL;

				mode = (mode == GL_FILL ? GL_LINE : GL_FILL);
				glPolygonMode(GL_FRONT_AND_BACK, mode);
				return;
			}
		}
	}

	base::OnKey(key, scancode, action, mods);
}

//----------------------------------------------------------------------------
//
// display
//

void GouraudExample::Display(bool auto_redraw)
{
	// feifei:��������
	// ���в���
	glEnable(GL_SCISSOR_TEST); glScissor(0, 0, width*0.8, height);
	// ģ�����
	//glEnable(GL_STENCIL_TEST);	glClearStencil(0);
	//glStencilFunc(GL_EQUAL, 1, 1); glStencilOp(GL_KEEP, GL_INCR, GL_DECR);
	//glStencilFunc(GL_NOTEQUAL, 1, 1); glStencilOp(GL_KEEP, GL_INCR, GL_DECR);
	// ��Ȳ���
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear( GL_COLOR_BUFFER_BIT );

	glBindVertexArray( VAOs[Triangles] );
	glDrawArrays( GL_TRIANGLES, 0, NumVertices );
	
	// Done
	base::Display();
}

void GouraudExample::Resize(int width, int height)
{
	glViewport(0, 0, width, height);

	aspect = float(height) / float(width);
	this->width = width;
	this->height = height;
}
