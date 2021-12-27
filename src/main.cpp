// clang-format off
#include <glad/glad.h>
// clang-format on
#include <GLFW/glfw3.h>

#include <iostream>

// void processInput(GLFWwindow* window) {
//   if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
//     glfwSetWindowShouldClose(window, true);
//   }
// }

int main(void) {
  GLFWwindow* window;

  /* Initialize the library */
  if (!glfwInit()) return -1;

  glfwWindowHint(GLFW_VERSION_MAJOR,4);
  glfwWindowHint(GLFW_VERSION_MINOR,1);
  //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  /* Create a windowed mode window and its OpenGL context */
  window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
  if (!window) {
    glfwTerminate();
    return -1;
  }

  /* Make the window's context current */
  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize OpenGL context" << std::endl;
    return -1;
  }

  printf("OpenGL Version: %s Renderer: %s \n", glGetString(GL_VERSION), glGetString(GL_RENDERER));

  // Triangle positions
  float pos[] = {-0.5f, -0.5f, 0.0f, 0.0f, 0.5f, 0.0f, 0.5f, -0.5f, 0.0f};

  unsigned int vao;
  glGenVertexArrays(1, &vao);

  unsigned int vbo;
  glGenBuffers(1, &vbo);

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  //glNamedBufferData(vbo, sizeof(pos), pos, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  // clang-format off
  const char* vertexShaderSource = "#version 330 core\n"
      "layout(location = 0) in vec3 aPos;\n"
      "void main()\n"
      "{\n" 
      "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
      "}\0";

  // clang-format on
  unsigned int vertexshader;
  vertexshader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexshader, 1, &vertexShaderSource, NULL);
  glCompileShader(vertexshader);

  // Check if shader was compiled successufully
  int success;
  char infoLog[512];
  glGetShaderiv(vertexshader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertexshader, 512, NULL, infoLog);
    std::cout << "ERROR COMPILING VERTEX SHADER: \n" << infoLog << std::endl;
  }

  // clang-format off
  const char* fragmentShaderSource = "#version 330 core\n"
  "out vec4 FragColor;\n"
  "void main()\n"
  "{\n"
  " FragColor = vec4(1.0f,0.5f,0.2f,1.0f);\n"
  "}\0";
  // clang-format on
  unsigned int fragmentshader;
  fragmentshader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentshader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentshader);

  glGetShaderiv(fragmentshader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fragmentshader, 512, NULL, infoLog);
    std::cout << "ERROR COMPILING FRAGMENT SHADER: \n" << infoLog << std::endl;
  }

  unsigned int shaderprogram;
  shaderprogram = glCreateProgram();
  glAttachShader(shaderprogram, vertexshader);
  glAttachShader(shaderprogram, fragmentshader);
  glLinkProgram(shaderprogram);

  glGetProgramiv(shaderprogram, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(shaderprogram, 512, NULL, infoLog);
    std::cout << "ERROR LINKING SHADERS: \n" << infoLog << std::endl;
  }

  glUseProgram(shaderprogram);
  glDeleteShader(vertexshader);
  glDeleteShader(fragmentshader);

  /* Loop until the user closes the window */
  while (!glfwWindowShouldClose(window)) {
    // processInput(window);

    /* Render here */
    glClearColor(0.2f, 0.4f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shaderprogram);
    glBindVertexArray(vao);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    /* Swap front and back buffers */
    glfwSwapBuffers(window);

    /* Poll for and process events */
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}
