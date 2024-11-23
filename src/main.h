#ifndef MAIN_H
#define MAIN_H

#define STB_IMAGE_IMPLEMENTATION

#include "utils/gl.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double mouse_x, double mouse_y);
void scroll_callback(GLFWwindow* window, double dx, double dy);
void processInput(GLFWwindow* window);

#endif
