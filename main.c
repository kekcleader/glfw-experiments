#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define bufW 320
#define bufH 200

int winW, winH;
GLint screenSizeLocation;
//float projectionMatrix[16];

// Textures
GLuint manTextureID;
GLuint cursorTextureID;

char *load_shader_file(const char* fileName) {
  FILE *fp;
  long size = 0;
  char *shaderContent;

  /* Read File to get size */
  fp = fopen(fileName, "rb");
  if (fp == NULL) {
    printf("Failed to load shader content\n");
    return "";
  }
  fseek(fp, 0L, SEEK_END);
  size = ftell(fp) + 1;
  fclose(fp);

  /* Read File for Content */
  fp = fopen(fileName, "r");
  shaderContent = memset(malloc(size), '\0', size);
  fread(shaderContent, 1, size - 1, fp);
  fclose(fp);

  return shaderContent;
}

GLuint loadTexture(const char *filename) {
  int width, height, channels;
  unsigned char *data = stbi_load(filename, &width, &height, &channels, 0);
  if (!data) {
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
  GLenum format = GL_RGB;
  if (channels == 1) format = GL_RED;
  else if (channels == 4) format = GL_RGBA;

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, format, GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);

  // Освобождение памяти и отвязка текстуры
  stbi_image_free(data);
  glBindTexture(GL_TEXTURE_2D, 0);

  return textureID;
}

/*
void makeProjection(float *m, float left, float right, float top, float bottom) {
  float near = -1.0;
  float far = 1.0;
  memset(m, 0, 16 * sizeof(float));

  // Главная диагональ
  m[0] = 2.0f / (right - left);
  m[5] = 2.0f / (top - bottom);
  m[10] = -2.0f / (far - near);
  m[15] = 1.0f;

  // Последний столбец
  m[3] = -(right + left) / (right - left);
  m[7] = -(top + bottom) / (top - bottom);
  m[11] = -(far + near) / (far - near);
}
*/

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
  winW = width;
  winH = height;
  glUniform2f(screenSizeLocation, (float)width, (float)height);

  float bufRatio = (float)bufW / bufH;
  float screenRatio = (float)width / height;

  float w, h, offX, offY;

  if (screenRatio > bufRatio) {
    w = bufW * (float)width / height;
    h = bufH;
    offX = (w - bufW) / 2;
    offY = 0;
  } else {
    w = bufW;
    h = bufH * (float)height / width;
    offX = 0;
    offY = (h - bufH) / 2;
  }

  //makeProjection(projectionMatrix, -offX, w - offX, -offY, h - offY);
}

void init_graph() {
  glfwInit();
}

GLFWwindow *create_window() {
  GLFWwindow *win;

  // Получение основного монитора
  GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
  // Получение режима видео для основного монитора
  const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);

  // Создание окна
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  //glfwWindowHint(GLFW_DECORATED, GLFW_FALSE); // Окно без рамки
  glfwWindowHint(GLFW_AUTO_ICONIFY, GLFW_FALSE); // Без автосворачивания
  winW = mode->width;
  winH = mode->height;
  //win = glfwCreateWindow(mode->width, mode->height, "Program", primaryMonitor, NULL);
  win = glfwCreateWindow(1000, 500, "Program", NULL, NULL);
  if (!win) {
    printf("Failed to create GLFW window\n");
    glfwTerminate();
    return NULL;
  }
  glfwSetFramebufferSizeCallback(win, framebuffer_size_callback);
  glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
  glfwMakeContextCurrent(win);
  glfwSwapInterval(0);

  if (glewInit() != GLEW_OK) {
    printf("Failed to initialize GLEW\n");
    return NULL;
  }

  return win;
}

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

GLuint createShaderProgram() {
  char *vertexSource = load_shader_file("shaders/vertex.txt");
  char *fragmentSource = load_shader_file("shaders/fragment.txt");

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

void close_buffers(GLuint *VAO, GLuint *VBO, GLuint *EBO) {
  glDeleteVertexArrays(1, VAO);
  glDeleteBuffers(1, VBO);
  glDeleteBuffers(1, EBO);
}

void init_buffers(GLuint *VAO, GLuint *VBO, GLuint *EBO) {
  // Определение вершин прямоугольника и текстурных координат
  float vertices[] = {
    // позиции | текстурные координаты
     1,  1,      1, 0, // верхний правый угол
     1, -1,      1, 1, // нижний правый угол
    -1, -1,      0, 1, // нижний левый угол
    -1,  1,      0, 0  // верхний левый угол
  };
  unsigned int indices[] = {
    0, 1, 3, // первый треугольник
    1, 2, 3  // второй треугольник
  };

  glGenVertexArrays(1, VAO);
  glGenBuffers(1, VBO);
  glGenBuffers(1, EBO);

  glBindVertexArray(*VAO);

  glBindBuffer(GL_ARRAY_BUFFER, *VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  // Позиционные атрибуты
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  // Текстурные атрибуты
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
  glEnableVertexAttribArray(1);
}

void run(GLFWwindow *win, GLuint shaderProgram, GLuint VAO) {
  GLint timeLocation = glGetUniformLocation(shaderProgram, "time");
  GLint cursorPosLocation = glGetUniformLocation(shaderProgram, "cursorPos");
  GLint screenLocation = glGetUniformLocation(shaderProgram, "screen");
  GLint cursorLocation = glGetUniformLocation(shaderProgram, "cursor");
  //GLint projectionLocation = glGetUniformLocation(shaderProgram, "projection");
  double x, y;

  screenSizeLocation = glGetUniformLocation(shaderProgram, "screenSize");

  //makeProjection(projectionMatrix, 0, bufW, 0, bufH);

  glUseProgram(shaderProgram);
  int w, h;
  glfwGetFramebufferSize(win, &w, &h);
  framebuffer_size_callback(win, w, h);

  while (!glfwWindowShouldClose(win)) {
    glClearColor(0.2, 0.6, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);

    glUniform1i(screenLocation, 0);
    glUniform1i(cursorLocation, 1);

    //glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, projectionMatrix);

    glUniform1f(timeLocation, glfwGetTime());
    glfwGetCursorPos(win, &x, &y);
    x = x / winW * 2.0 - 1.0;
    y = -(y / winH * 2.0 - 1.0);
    glUniform2f(cursorPosLocation, (float)x, (float)y);

    // Привязка текстур
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, manTextureID);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, cursorTextureID);

    // Прорисовка
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // Отображение результата
    glfwSwapBuffers(win);

    glfwWaitEventsTimeout(1.0 / 60);
  }
}

int main() {
  GLFWwindow *win;
  GLuint VBO, VAO, EBO;

  init_graph();
  win = create_window();
  if (!win) return 1;

  // Шейдер
  GLuint shaderProgram = createShaderProgram();

  init_buffers(&VAO, &VBO, &EBO);

  // Загрузка текстур
  manTextureID = loadTexture("images/man.jpg");
  cursorTextureID = loadTexture("images/arrow.png");

  run(win, shaderProgram, VAO);

  close_buffers(&VAO, &VBO, &EBO);
  glDeleteProgram(shaderProgram);

  glfwTerminate();
  return 0;
}

