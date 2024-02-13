#include <GLFW/glfw3.h>

int main(void) {
  GLFWwindow* window;

  // Инициализация библиотеки
  if (!glfwInit()) {
    return -1;
  }

  // Создание окна и его контекста OpenGL
  window = glfwCreateWindow(640, 480, "Простое GLFW окно", NULL, NULL);
  if (!window) {
    glfwTerminate();
    return -1;
  }

  // Сделать контекст окна текущим
  glfwMakeContextCurrent(window);

  // Цикл, пока пользователь не закроет окно
  while (!glfwWindowShouldClose(window)) {
    // Очистить буфер цвета
    glClear(GL_COLOR_BUFFER_BIT);

    // Переключить передний и задний буферы
    glfwSwapBuffers(window);

    // Опросить и обработать события
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}

