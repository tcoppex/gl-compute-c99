#include <stdlib.h>
#include <stdio.h>
#include "GLFW/glfw3.h"

#include "app.h"

void window_size_callback(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
}

int main(int argc, char *argv[]) {
  GLFWwindow *window = 0;

  /* Initialize the API */
  if (!glfwInit()) {
    return EXIT_FAILURE;
  }

  /* Initialize OpenGL context flags */
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  /* Compute window resolution from the main monitor's */
  {
    const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    const int width  = mode->width / 2;
    const int height = mode->height / 2;

    /* Create the window and OpenGL context */
    window = glfwCreateWindow(width, height, "demo", NULL, NULL);
    if (!window) {
      fprintf(stderr, "error: failed to create window.\n");
      glfwTerminate();
      return EXIT_FAILURE;
    }
    glfwSetWindowSizeCallback(window, window_size_callback);
  }

  /* Make the window's context current */
  glfwMakeContextCurrent(window);

  /* Initialize app specific environment */
  AppSetup();

  /* Mainloop */
  while (!glfwWindowShouldClose(window)) {
    /* Update and render one frame */
    AppFrame();

    /* Swap front & back buffers */
    glfwSwapBuffers(window);

    /* Manage events */
    glfwPollEvents();
  }

  glfwTerminate();
  return EXIT_SUCCESS;
}
