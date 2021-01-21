#ifndef __VGL_H__
#define __VGL_H__

#include <GL3/gl3.h>
#include <GL3/gl3w.h>

#include <GLFW/glfw3.h>

#define BUFFER_OFFSET(a) ((void*)(a))

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

#endif /* __VGL_H__ */
