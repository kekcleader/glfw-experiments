#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>

#define WIDTH 320 // Исходная ширина растра
#define HEIGHT 200 // Исходная высота растра

GLint screenSizeLocation;
double mouseX = 0.0, mouseY = 0.0; // Глобальные переменные для координат мыши

// Вершинный шейдер
const char* vertexShaderSource = "#version 330 core\n"
  "layout (location = 0) in vec2 aPos;\n"
  "layout (location = 1) in vec2 aTexCoord;\n"
  "out vec2 TexCoord;\n"
  "void main()\n"
  "{\n"
  "   gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);\n"
  "   TexCoord = aTexCoord;\n"
  "}\0";

// Фрагментный шейдер
const char* fragmentShaderSource = "#version 330 core\n"
  "out vec4 FragColor;\n"
  "in vec2 TexCoord;\n"
  "uniform sampler2D texture1;\n"
  "uniform vec2 cursorPos;\n"
  "uniform float cursorSize;\n"
  "uniform vec2 screenSize;\n"
  "void main()\n"
  "{\n"
  "   vec2 pos = TexCoord * screenSize;\n"
  "   if(abs(pos.x - cursorPos.x) < cursorSize && abs(pos.y - cursorPos.y) < cursorSize) {\n"
  "       FragColor = vec4(1.0, 0.0, 0.0, 1.0); // Цвет курсора\n"
  "   } else {\n"
  "       FragColor = texture(texture1, TexCoord);\n"
  "   }\n"
  "}\0";

unsigned char pixels[WIDTH * HEIGHT * 3]; // Массив пикселей для текстуры

void initPixels() {
  // Заполнение массива пикселей
  int i = 0;
  for (int y = 0; y < HEIGHT; ++y) {
    for (int x = 0; x < WIDTH; ++x) {
      pixels[i++] = (y * 1024 / (x + 1)) % 256; // Красный
      pixels[i++] = x * y; // Зеленый
      pixels[i++] = (x * 1024 / (y + 1)) % 256; // Синий
    }
  }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
  glUniform2f(screenSizeLocation, (float)width, (float)height);

  float aspectRatioSource = (float)WIDTH / (float)HEIGHT;
  float aspectRatioWindow = (float)width / (float)height;

  int viewportWidth, viewportHeight;

  if (aspectRatioWindow > aspectRatioSource) {
    viewportHeight = height;
    viewportWidth = (int)(height * aspectRatioSource);
  } else {
    viewportWidth = width;
    viewportHeight = (int)(width / aspectRatioSource);
  }

  int viewportX = (width - viewportWidth) / 2;
  int viewportY = (height - viewportHeight) / 2;

  glViewport(viewportX, viewportY, viewportWidth, viewportHeight);
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
  int width, height;
  glfwGetFramebufferSize(window, &width, &height);
  mouseX = xpos / width * WIDTH;
  mouseY = HEIGHT - ypos / height * HEIGHT;
}

int main() {
  GLFWwindow* window;

  if (!glfwInit()) return -1;

  GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
  const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);

  window = glfwCreateWindow(mode->width, mode->height, "Program", primaryMonitor, NULL);
  if (!window) {
    glfwTerminate();
    return -1;
  }

  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetCursorPosCallback(window, cursor_position_callback); // Установка обработчика позиции курсора

  glfwMakeContextCurrent(window);
  if (glewInit() != GLEW_OK) {
    fprintf(stderr, "Failed to initialize GLEW\n");
    return -1;
  }

  initPixels();

  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
  glCompileShader(vertexShader);

  int success;
  char infoLog[512];
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n%s\n", infoLog);
    return -1;
  }

  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShader);

  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
    printf("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n%s\n", infoLog);
    return -1;
  }

  GLuint shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  screenSizeLocation = glGetUniformLocation(shaderProgram, "screenSize");

  float vertices[] = {
     1.0f,  1.0f,  1.0f, 1.0f,
     1.0f, -1.0f,  1.0f, 0.0f,
    -1.0f, -1.0f,  0.0f, 0.0f,
    -1.0f,  1.0f,  0.0f, 1.0f  
  };
  unsigned int indices[] = {
    0, 1, 3,
    1, 2, 3  
  };

  GLuint VAO, VBO, EBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
  glEnableVertexAttribArray(1);

  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIDTH, HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);

  GLint cursorPosLocation = glGetUniformLocation(shaderProgram, "cursorPos");
  GLint cursorSizeLocation = glGetUniformLocation(shaderProgram, "cursorSize");
  float cursorSize = 10.0; // Размер курсора

  while (!glfwWindowShouldClose(window)) {
    glUniform2f(cursorPosLocation, (float)mouseX, (float)mouseY);
    glUniform1f(cursorSizeLocation, cursorSize);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    glBindTexture(GL_TEXTURE_2D, texture);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}

