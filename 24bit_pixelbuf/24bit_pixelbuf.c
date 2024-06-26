#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>

#define WIDTH 640
#define HEIGHT 480

// Объявляем массив для хранения пикселей
unsigned char pixels[WIDTH * HEIGHT * 3];

void initPixels(int i) {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            int index = (y * WIDTH + x) * 3;
            pixels[index] = (x + y + i) % 256; // Красный
            pixels[index + 1] = (-x + y * 2) % 256; // Зеленый
            pixels[index + 2] = (x * 4 + y * 4 + 2 * i) % 256; // Синий
        }
    }
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, 1);
    }
}

int main(void) {
    GLFWwindow* window;

    if (!glfwInit()) return -1;

    window = glfwCreateWindow(WIDTH, HEIGHT, "8-Bit Raster", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSwapInterval(0);

    if (glewInit() != GLEW_OK) {
        printf("GLEW initialization failed.\n");
        return -1;
    }

    initPixels(0);
    int i = 0;

    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        glClear(GL_COLOR_BUFFER_BIT);
        // Отрисовка пикселей
        glDrawPixels(WIDTH, HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, pixels);

        glfwSwapBuffers(window);
        glfwWaitEventsTimeout(1.0 / 60);
        glfwPollEvents();
        initPixels(++i);
    }

    glfwTerminate();
    return 0;
}

