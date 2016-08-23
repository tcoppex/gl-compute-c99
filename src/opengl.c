#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "opengl.h"

/* ========================================================================== */

static
int checkExtensions(const char **extensions) {
  unsigned int i = 0u;
  int valid = 1;

  for (i=0u; extensions[i] != NULL; ++i) {
    if (!glfwExtensionSupported(extensions[i])) {
      fprintf(stderr, "warning : Extension \"%s\" is not supported.\n", extensions[i]);
      valid = 0;
    }
  }
  return valid;
}

#ifndef USE_GLEW

static
GLFWglproc getAddress(const char *name) {
  GLFWglproc ptr = glfwGetProcAddress(name);

  if (ptr == 0) {
    fprintf(stderr, "error: Extension function %s not found.\n", name);
  }

  return ptr;
}

#include "ext/_extensions.inl"

#endif

/* ========================================================================== */

extern
void InitGL() {
  /* ~TODO : too app specifics atm */
  const char *s_extensions[] = {
    "GL_ARB_compute_shader",
    "GL_ARB_separate_shader_objects",
    "GL_ARB_shader_image_load_store",
    NULL
  };

  /* Check if specific extensions exists */
  checkExtensions(s_extensions);

#ifdef USE_GLEW
  /* Load GLEW */
  glewExperimental = GL_TRUE;
  GLenum result = glewInit();

  /* flush doubtful error */
  glGetError();

  if (GLEW_OK != result) {
    fprintf(stderr, "Error: %s\n", glewGetErrorString(result));
  }
#else
  /* Load function pointer */
  LoadExtensionFuncPtrs();
#endif
}

void CheckShaderStatus(GLuint shader) {
  GLint status = 0;

  glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
  if (status != GL_TRUE) {
    char buffer[1024];
    glGetShaderInfoLog(shader, 1024, 0, buffer);
    fprintf(stderr, "%s\n", buffer);
  }
}

void CheckProgramStatus(GLuint program) {
  GLint status = 0;

  glGetProgramiv(program, GL_LINK_STATUS, &status);
  if (status != GL_TRUE) {
    char buffer[1024];
    glGetProgramInfoLog(program, 1024, 0, buffer);
    fprintf(stderr, "%s\n", buffer);
  }

  glValidateProgram(program);
  glGetProgramiv(program, GL_VALIDATE_STATUS, &status);
  if (status != GL_TRUE) {
    fprintf(stderr, "Program failed to be validated.\n");
    exit(EXIT_FAILURE);
  }
}

static
const char* GetErrorString(GLenum err) {
#define STRINGIFY(x) #x
  switch (err)
  {
    case GL_NO_ERROR:
      return STRINGIFY(GL_NO_ERROR);
      
    case GL_INVALID_ENUM:
      return STRINGIFY(GL_INVALID_ENUM);
      
    case GL_INVALID_VALUE:
      return STRINGIFY(GL_INVALID_VALUE);
      
    case GL_INVALID_OPERATION:
      return STRINGIFY(GL_INVALID_OPERATION);
      
    case GL_STACK_OVERFLOW:
      return STRINGIFY(GL_STACK_OVERFLOW);
      
    case GL_STACK_UNDERFLOW:
      return STRINGIFY(GL_STACK_UNDERFLOW);
      
    case GL_OUT_OF_MEMORY:
      return STRINGIFY(GL_OUT_OF_MEMORY);
      
      
    default:
      return "GetErrorString : Unknown constant";
  }
#undef STRINGIFY
    
  return "";
}

void CheckGLError(const char* file, const int line, const char* errMsg, int bExitOnFail) {
  GLenum err = glGetError();

  if (err != GL_NO_ERROR) {
    fprintf(stderr,
            "OpenGL error @ \"%s\" [%d] : %s [%s].\n",
            file, line, errMsg, GetErrorString(err));

    if (bExitOnFail != 0) {
      exit(EXIT_FAILURE);
    }
  }
}

/* ========================================================================== */