#ifndef OPENGL_H_
#define OPENGL_H_

#ifdef USE_GLEW
#include "GL/glew.h"
#endif

#include "GLFW/glfw3.h"

#ifndef USE_GLEW
/* Automatically generated macros to extension's function */
#include "ext/_extensions.h"
#endif

void InitGL();
void CheckShaderStatus(GLuint shader);
void CheckProgramStatus(GLuint program);
void CheckGLError(const char* file, const int line, const char* errMsg, int bExitOnFail);

#define CHECKGLERROR()   CheckGLError(__FILE__, __LINE__, "", 0)

#endif