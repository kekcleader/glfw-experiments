#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// Вершинный шейдер
const char* vertexShaderSource = "#version 330 core\n"
  "layout (location = 0) in vec3 aPos;\n"
  "layout (location = 1) in vec2 aTexCoord;\n"
  "out vec2 TexCoord;\n"
  "void main()\n"
  "{\n"
  "   gl_Position = vec4(aPos, 1.0);\n"
  "   TexCoord = aTexCoord;\n"
  "}\0";

// Фрагментный шейдер
const char* fragmentShaderSource = "#version 330 core\n"
  "out vec4 FragColor;\n"
  "in vec2 TexCoord;\n"
  "uniform sampler2D texture1;\n"
  "void main()\n"
  "{\n"
  "   FragColor = texture(texture1, TexCoord);\n"
  "}\0";

GLuint loadTexture(const char* filename) {
  int width, height, channels;
  unsigned char* data = stbi_load(filename, &width, &height, &channels, 0);
  if (data == NULL) {
    printf("Error loading texture '%s'\n", filename);
    exit(1);
  }

  GLuint textureID;
  glGenTextures(1, &textureID);
  glBindTexture(GL_TEXTURE_2D, textureID);

  // Установка параметров текстуры
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // Загрузка изображения в текстуру
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);

  // Освобождение памяти и отвязка текстуры
  stbi_image_free(data);
  glBindTexture(GL_TEXTURE_2D, 0);

  return textureID;
}

int main() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
  if (window == NULL) {
    printf("Failed to create GLFW window\n");
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);

  if (glewInit() != GLEW_OK) {
    printf("Failed to initialize GLEW\n");
    return -1;
  }

  // Компиляция шейдеров
  unsigned int vertexShader, fragmentShader;
  vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
  glCompileShader(vertexShader);

  fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShader);

  // Создание программы шейдеров
  unsigned int shaderProgram;
  shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  // Определение вершин прямоугольника и текстурных координат
  float vertices[] = {
    // позиции    // текстурные координаты
     0.25f,  0.4f, 0.0f,  1.0f, 0.0f, // верхний правый угол
     0.25f, -0.4f, 0.0f,  1.0f, 1.0f, // нижний правый угол
    -0.25f, -0.4f, 0.0f,  0.0f, 1.0f, // нижний левый угол
    -0.25f,  0.4f, 0.0f,  0.0f, 0.0f  // верхний левый угол
  };
  unsigned int indices[] = {
    0, 1, 3, // первый треугольник
    1, 2, 3  // второй треугольник
  };

  unsigned int VBO, VAO, EBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  // Позиционные атрибуты
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  // Текстурные атрибуты
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  // Загрузка текстуры
  GLuint textureID = loadTexture("texture.jpg");
  glBindTexture(GL_TEXTURE_2D, textureID);

  while (!glfwWindowShouldClose(window)) {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);
  glDeleteProgram(shaderProgram);

  glfwTerminate();
  return 0;
}

