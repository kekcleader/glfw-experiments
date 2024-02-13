#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <math.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

const char* vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec2 aTexCoord;\n"
    "out vec2 TexCoord;\n"
    "void main() {\n"
    "   gl_Position = vec4(aPos, 1.0);\n"
    "   TexCoord = aTexCoord;\n"
    "}\0";

const char* fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "in vec2 TexCoord;\n"
    "uniform sampler2D texture1;\n"
    "void main() {\n"
    "   vec2 texCoord = TexCoord;\n"
    "   texCoord.x += sin(texCoord.y * 20.0 + glfwGetTime()) * 0.01;\n"
    "   FragColor = texture(texture1, texCoord);\n"
    "}\0";

GLuint compileShader(GLenum type, const char* source) {
  // Создание шейдера
  GLuint shader = glCreateShader(type);
  glShaderSource(shader, 1, &source, NULL);
  glCompileShader(shader);

  // Проверка на ошибки компиляции
  GLint success;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    char infoLog[512];
    glGetShaderInfoLog(shader, 512, NULL, infoLog);
    fprintf(stderr, "Ошибка компиляции шейдера: %s\n", infoLog);
  }

  return shader;
}

GLuint createShaderProgram(const char* vertexSource, const char* fragmentSource) {
  // Компиляция шейдеров
  GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexSource);
  GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSource);

  // Создание программы и прикрепление шейдеров к ней
  GLuint shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);

  // Проверка на ошибки линковки
  GLint success;
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
    char infoLog[512];
    glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
    fprintf(stderr, "Ошибка линковки программы: %s\n", infoLog);
  }

  // Удаление шейдеров после линковки
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  return shaderProgram;
}

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

void drawMovingSquare(GLFWwindow* window, double time, GLuint textureID) {
  float x = (float)sin(time) * 0.5f; // Простая анимация движения
  float y = (float)cos(time) * 0.5f;

  glBindTexture(GL_TEXTURE_2D, textureID); // Привязка текстуры
  glEnable(GL_TEXTURE_2D); // Включение текстурирования

  glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f); glVertex2f(-0.25f + x, -0.4f + y);
    glTexCoord2f(1.0f, 1.0f); glVertex2f(0.25f + x, -0.4f + y);
    glTexCoord2f(1.0f, 0.0f); glVertex2f(0.25f + x, 0.4f + y);
    glTexCoord2f(0.0f, 0.0f); glVertex2f(-0.25f + x, 0.4f + y);
  glEnd();

  glBindTexture(GL_TEXTURE_2D, 0); // Отвязка текстуры
}

int main(void) {
  GLFWwindow* win;

  if (!glfwInit()) {
    return -1;
  }

  // Получение основного монитора
  GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
  // Получение режима видео для основного монитора
  const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);

  // Создание окна
  glfwWindowHint(GLFW_DECORATED, GLFW_FALSE); // Окно без рамки
  glfwWindowHint(GLFW_AUTO_ICONIFY, GLFW_FALSE); // Без автосворачивания
  win = glfwCreateWindow(mode->width, mode->height, "Fullscreen", primaryMonitor, NULL);
  if (!win) {
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(win);
  glewExperimental = GL_TRUE;
  GLenum err = glewInit();
  if (GLEW_OK != err) {
    // Обработка ошибки
    fprintf(stderr, "Ошибка: %s\n", glewGetErrorString(err));
    exit(-1);
  }

  glfwSwapInterval(1);

  // Загрузка текстуры
  GLuint textureID = loadTexture("texture.jpg");

  // Шейдер
  GLuint shaderProgram = createShaderProgram(vertexShaderSource, fragmentShaderSource);

  // Главный цикл
  int done = 0;
  while (!glfwWindowShouldClose(win)) {
    glfwMakeContextCurrent(win);

    glClearColor(0.1, 0.4, 0.7, 0.0);

    glUseProgram(shaderProgram);

    double time = glfwGetTime();
    drawMovingSquare(win, time, textureID);

    glfwSwapBuffers(win);

    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}

