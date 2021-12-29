// clang-format off
#include "core.h"
// clang-format on

#include <GLFW/glfw3.h>

#include <iostream>

#include "shader.h"

void processInput(GLFWwindow* window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }
}

int main(void) {
  GLFWwindow* window;

  /* Initialize the library */
  if (!glfwInit()) return -1;

  /* Create a windowed mode window and its OpenGL context */
  window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
  if (!window) {
    glfwTerminate();
    return -1;
  }

  /* Make the window's context current */
  glfwMakeContextCurrent(window);

  int version = gladLoadGL(glfwGetProcAddress);
  if (version == 0) {
    printf("Failed to initialize OpenGL context\n");
    return -1;
  }

  // Successfully loaded OpenGL
  spdlog::info("Loaded OpenGL", GLAD_VERSION_MAJOR(version),
               GLAD_VERSION_MINOR(version));

  // Triangle positions
  // clang-format off
  float pos[] = {
    -0.5f,-0.5f,0.0f, // bottom-left
    -0.5f, 0.5f,0.0f, // top-left
     0.5f, 0.5f,0.0f, // top-right
     0.5f,-0.5f,0.0f  // bottom-right
    };

  unsigned int indices[] = {
    0,1,3, // traingle below
    1,2,3 // triangle above
  };

  // clang-format on
  unsigned int vao;
  glGenVertexArrays(1, &vao);

  unsigned int vbo;
  glGenBuffers(1, &vbo);
  glBindVertexArray(vao);

  unsigned int ebo;
  glGenBuffers(1, &ebo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(pos), pos, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  Shader* shader =
      new Shader("../data/shaders/basic.vert", "../data/shaders/basic.frag");

  // glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

  /* Loop until the user closes the window */
  while (!glfwWindowShouldClose(window)) {
    processInput(window);

    /* Render here */
    glClearColor(0.2f, 0.4f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    shader->Bind();
    double time = glfwGetTime();
    float greenValue = sin(time) / 2.0f + 0.5f;
    shader->SetUniform4f("ourColor", 0, greenValue, 0, 0);

    glBindVertexArray(vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    /* Swap front and back buffers */
    glfwSwapBuffers(window);

    /* Poll for and process events */
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}
