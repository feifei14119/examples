/*
 * Copyright ?2012-2015 Graham Sellers
 *
 * This code is part of the OpenGL Programming Guide, 9th Edition.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 */

/* $URL$
   $Rev$
   $Author$
   $Date$
   $Id$
 */

#include "vapp.h"
#include "vutils.h"

#include "vmath.h"

#include "LoadShaders.h"

#include <stdio.h>

using namespace vmath;

BEGIN_APP_DECLARATION(DrawCommandExample)
	// Override functions from base class
	virtual void Initialize(const char * title);
	virtual void Display(bool auto_redraw);
	virtual void Finalize(void);
	virtual void Resize(int width, int height);

	// Member variables
	float aspect;
	GLuint render_prog;
	GLuint vao[1]; // vertex attribute = 1;
	GLuint vbo[1]; // vertex buffer  = 2; vertex_positions + vertex_colors
	GLuint ebo[1]; // element buffer = 1;

	GLint render_model_matrix_loc;		// shader中model_matrix变量的索引.  = 0
	GLint render_projection_matrix_loc;	// shader中projection_matrix的索引. = 1
END_APP_DECLARATION()

DEFINE_APP(DrawCommandExample, "Drawing Commands Example")

void DrawCommandExample::Initialize(const char * title)
{
	base::Initialize(title);

	ShaderInfo shader_info[] =
	{
		{ GL_VERTEX_SHADER, "media/shaders/primitive_restart/primitive_restart.vs.glsl" },
		{ GL_FRAGMENT_SHADER, "media/shaders/primitive_restart/primitive_restart.fs.glsl" },
		{ GL_NONE, NULL }
	};

	render_prog = LoadShaders(shader_info);

	glUseProgram(render_prog);

	// "model_matrix" is actually an array of 4 matrices
	render_model_matrix_loc = glGetUniformLocation(render_prog, "model_matrix");
	render_projection_matrix_loc = glGetUniformLocation(render_prog, "projection_matrix");

	// A single triangle
	static const GLfloat vertex_positions[] =
	{
		//  x,     y,     z,   zoom in Homogeneous coordinates
		-1.0f, -1.0f,  0.0f, 1.0f, // left  bottom
		 2.0f, -2.0f,  0.0f, 2.0f, // right bottom
		-1.0f,  1.0f,  0.0f, 1.0f, // left  top
		 1.0f,  1.0f,  0.0f, 1.0f, // right top
	};

    // Color for each vertex
	static const GLfloat vertex_colors[] =
	{
		// r,    g,    b, alph,
		1.0f, 0.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f, 1.0f,
		0.0f, 0.0f, 1.0f, 1.0f
	};

	// Indices for the triangle strips
	static const GLushort vertex_indices[] = { 0, 1, 3 };

	// Set up the vertex attributes objects
	glGenVertexArrays(1, vao); // 
	glBindVertexArray(vao[0]); // 

	// Set up the element array buffer
	// index buffer
	glGenBuffers(1, ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vertex_indices), vertex_indices, GL_STATIC_DRAW);
	
	// buffer obj for vertext and color
	size_t v_pos_sz = sizeof(vertex_positions);
	size_t v_color_sz = sizeof(vertex_colors);
	size_t v_pos_bias = 0;
	size_t v_color_bias = v_pos_bias + v_pos_sz;
	glGenBuffers(1, vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, v_pos_sz + v_color_sz, NULL, GL_STATIC_DRAW);
    
	glBufferSubData(GL_ARRAY_BUFFER, v_pos_bias, v_pos_sz, vertex_positions);
	glBufferSubData(GL_ARRAY_BUFFER, v_color_bias, v_color_sz, vertex_colors);
	
	// program input parameters 
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(v_pos_bias));
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(v_color_bias));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
}

void DrawCommandExample::Display(bool auto_redraw)
{
	float t = float(app_time() & 0x1FFF) / float(0x1FFF);
	static float q = 0.0f;
	static const vmath::vec3 X(1.0f, 0.0f, 0.0f);
	static const vmath::vec3 Y(0.0f, 1.0f, 0.0f);
	static const vmath::vec3 Z(0.0f, 0.0f, 1.0f);
	static const vmath::vec4 black = vmath::vec4(0.0f, 0.0f, 0.0f, 0.0f);

	mat4 model_matrix; // translate matrix, move obj

	// Setup
	glEnable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);

	glClearBufferfv(GL_COLOR, 0, black);

	// Set up the model and projection matrix
	vmath::mat4 projection_matrix(vmath::frustum(-1.0f, 1.0f, -aspect, aspect, 1.0f, 500.0f));
	glUniformMatrix4fv(render_projection_matrix_loc, 1, GL_FALSE, projection_matrix);
	glBindVertexArray(vao[0]);						// for dram without index
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[0]);	// for draw with index

	//1. Set up for a glDrawElements call. dram without index
	model_matrix = vmath::translate(-3.0f, 1.0f, -5.0f);
	glUniformMatrix4fv(render_model_matrix_loc, 1, GL_FALSE, model_matrix);
	glDrawArrays(GL_TRIANGLES, 0, 3);

	//2. DrawElements. draw with index
	model_matrix = vmath::translate(-1.0f, -1.0f, -5.0f);
	glUniformMatrix4fv(render_model_matrix_loc, 1, GL_FALSE, model_matrix);
	glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, NULL);

	// DrawElementsBaseVertex
	model_matrix = vmath::translate(1.0f, 0.0f, -5.0f);
	glUniformMatrix4fv(render_model_matrix_loc, 1, GL_FALSE, model_matrix);
	glLineWidth(5.0f);
	glDrawElementsBaseVertex(GL_LINE_LOOP, 3, GL_UNSIGNED_SHORT, NULL, 1);

	// DrawArraysInstanced
	model_matrix = vmath::translate(3.0f, 0.0f, -5.0f);
	glUniformMatrix4fv(render_model_matrix_loc, 1, GL_FALSE, model_matrix);
	glPointSize(15.0f);	glDrawArraysInstanced(GL_POINTS, 1, 3, 1);

	base::Display();
}

void DrawCommandExample::Finalize(void)
{
	glUseProgram(0);
	glDeleteProgram(render_prog);
	glDeleteVertexArrays(1, vao);
	glDeleteBuffers(1, vbo);
}

void DrawCommandExample::Resize(int width, int height)
{
	glViewport(0, 0 , width, height);

	aspect = float(height) / float(width);
}
