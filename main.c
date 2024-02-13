#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <math.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define windowCount 2

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
    glTexCoord2f(0.0f, 0.0f); glVertex2f(-0.25f + x, -0.25f + y);
    glTexCoord2f(1.0f, 0.0f); glVertex2f(0.25f + x, -0.25f + y);
    glTexCoord2f(1.0f, 1.0f); glVertex2f(0.25f + x, 0.25f + y);
    glTexCoord2f(0.0f, 1.0f); glVertex2f(-0.25f + x, 0.25f + y);
  glEnd();

  glBindTexture(GL_TEXTURE_2D, 0); // Отвязка текстуры
}

int main(void) {
  GLFWwindow* windows[windowCount];
  GLFWwindow* firstWin;

  if (!glfwInit()) {
    return -1;
  }

  // Создание двух окон
  firstWin = NULL;
  for (int i = 0; i < windowCount; ++i) {
    windows[i] = glfwCreateWindow(640, 480, "Moving Square Window", NULL, firstWin);
    firstWin = windows[0];
    if (!windows[i]) {
      glfwTerminate();
      return -1;
    }
  }

  glfwMakeContextCurrent(windows[0]);
  glewExperimental = GL_TRUE;
  GLenum err = glewInit();
  if (GLEW_OK != err) {
    // Обработка ошибки
    fprintf(stderr, "Ошибка: %s\n", glewGetErrorString(err));
    exit(-1);
  }

  // Загрузка текстуры
  GLuint textureID = loadTexture("texture.jpg");

  // Главный цикл
  while (!glfwWindowShouldClose(windows[0]) && !glfwWindowShouldClose(windows[1])) {
    for (int i = 0; i < windowCount; ++i) {
      glfwMakeContextCurrent(windows[i]);

      glClear(GL_COLOR_BUFFER_BIT);

      double time = glfwGetTime();
      drawMovingSquare(windows[i], time, textureID);

      glfwSwapBuffers(windows[i]);
    }

    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}

