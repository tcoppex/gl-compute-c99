#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "app.h"
#include "opengl.h"

/* ========================================================================== */

#define SHADERS_DIR     "../shaders/"
#define MAX_BUFFERSIZE  2048

typedef struct {
  GLuint computePgm;
  GLuint renderPgm;
  GLuint vao;
  GLuint texture;
  unsigned int tick;
} TGlobal;

static TGlobal s_Global;

/* ========================================================================== */

static
int ReadFile(const char* filename, const unsigned int maxsize, char out[]) {
  FILE* fd = 0;
  size_t nelems = 0;
  size_t nreads = 0;

  if (!(fd = fopen(filename, "r"))) {
    fprintf(stderr, "warning: \"%s\" not found.\n", filename);
    return 0;
  }

  memset(out, 0, maxsize);

  fseek(fd, 0, SEEK_END);
  nelems = ftell(fd);
  nelems = (nelems > maxsize) ? maxsize : nelems;
  fseek(fd, 0, SEEK_SET);

  nreads = fread(out, sizeof(char), nelems, fd);

  fclose(fd);

  return nreads == nelems;
}

static
GLuint setupComputeProgram(char *src_buffer) {
  GLuint pgm = 0u;

  ReadFile(SHADERS_DIR "cs_simple.glsl", MAX_BUFFERSIZE, src_buffer);
  pgm = glCreateShaderProgramv(GL_COMPUTE_SHADER, 1, (const char**)&src_buffer);

  CheckProgramStatus(pgm);

  return pgm;
}

static
GLuint setupRenderProgram(char *src_buffer) {
  GLuint pgm = 0u;
  GLuint vshader = 0u;
  GLuint fshader = 0u;

  /* Vertex Shader */
  vshader = glCreateShader(GL_VERTEX_SHADER);
  ReadFile(SHADERS_DIR "vs_mapscreen.glsl", MAX_BUFFERSIZE, src_buffer);
  glShaderSource(vshader, 1, (const char**)&src_buffer, (const GLint*)0);
  glCompileShader(vshader);
  CheckShaderStatus(vshader);

  /* Fragment Shader */
  fshader = glCreateShader(GL_FRAGMENT_SHADER);
  ReadFile(SHADERS_DIR "fs_mapscreen.glsl", MAX_BUFFERSIZE, src_buffer);
  glShaderSource(fshader, 1, (const char**)&src_buffer, (const GLint*)0);
  glCompileShader(fshader);
  CheckShaderStatus(fshader);

  pgm = glCreateProgram();
  glAttachShader(pgm, vshader); glDeleteShader(vshader);
  glAttachShader(pgm, fshader); glDeleteShader(fshader);
  glLinkProgram(pgm);
  CheckProgramStatus(pgm);

  return pgm;
}

static
GLuint setupTexture() {
  GLuint tex = 0;

  const unsigned int w = 512u;
  const unsigned int h = 512u;

  glGenTextures(1, &tex);
  glBindTexture(GL_TEXTURE_2D, tex);

  /* ComputeShader need to use immutable image space */
  glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, w, h);

  glClearTexImage(tex, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

  CHECKGLERROR();

  return tex;
}

static
void launchCompute() {
  glUseProgram(s_Global.computePgm);

  glUniform1f(glGetUniformLocation(s_Global.computePgm, "uTime"), 
              0.001f*s_Global.tick);

  glBindImageTexture(0, s_Global.texture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);
  glDispatchCompute(32, 32, 1);

  CHECKGLERROR();
}

static
void renderScreen() {
  glClear(GL_COLOR_BUFFER_BIT);

  glUseProgram(s_Global.renderPgm);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, s_Global.texture);

  glBindVertexArray(s_Global.vao);
  glDrawArrays(GL_TRIANGLES, 0, 3);


  CHECKGLERROR();
}

static
void AppFinalize() {
  glDeleteProgram(s_Global.computePgm);
  glDeleteProgram(s_Global.renderPgm);
  glDeleteTextures(1, &s_Global.texture);
  glDeleteVertexArrays(1, &s_Global.vao);

  CHECKGLERROR();
}

/* ========================================================================== */

extern
void AppSetup() {
  char *src_buffer = NULL;

  /* Initialize OpenGL extensions */
  InitGL();

  /* setting some OpenGL properties */
  glClearColor( 0.25f, 0.25f, 0.25f, 1.0f);
  glDisable(GL_DEPTH_TEST);

  /* Generate program shaders */
  src_buffer = (char*)calloc(MAX_BUFFERSIZE, sizeof(char));
  s_Global.computePgm = setupComputeProgram(src_buffer);
  s_Global.renderPgm  = setupRenderProgram(src_buffer);
  free(src_buffer);

  /* Setup programs' uniform value */
  {
    GLint loc = -1;

    loc = glGetUniformLocation(s_Global.computePgm, "uOutputImg");
    glProgramUniform1i(s_Global.computePgm, loc, 0);

    loc = glGetUniformLocation(s_Global.renderPgm, "uInputTex");
    glProgramUniform1i(s_Global.renderPgm, loc, 0);

    /* default tick value */
    s_Global.tick = 0u;
  }

  /* Texture / Image object */
  s_Global.texture = setupTexture();

  /* Simple VertexArray Object for hack screen rendering */
  glGenVertexArrays(1, &s_Global.vao);

  atexit(AppFinalize);

  CHECKGLERROR();
}

extern
void AppFrame() {
  /* Update the tick [todo : use frame control] */
  ++s_Global.tick;

  /* Update the texture via the kernel */
  launchCompute();

  /* Render texture to the screen */
  renderScreen();
}

/* ========================================================================== */