#include <iostream>
#include "GLFW/glfw3.h"
#include <cassert>

#define UNREFERENCED_PARAMETER(x) x

void glfw_error_callback(int error_code, const char* description)
{
    UNREFERENCED_PARAMETER(error_code);
    printf(description);
    printf("\n");
}

int main()
{
    glfwSetErrorCallback(glfw_error_callback);

    int err;
    err = glfwInit();
    assert(err != -1 && "Failed to initialize GLFW!");

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    GLFWwindow* window;
    window = glfwCreateWindow(1920, 1080, "Iceberg Application", nullptr, nullptr);
    assert(window && "Failed to create a window!");

    glfwMakeContextCurrent(window);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
    
        if (glfwGetKey(window, GLFW_KEY_ESCAPE))
        {
            glfwSetWindowShouldClose(window, 1);
        }
    }

    glfwDestroyWindow(window);

    glfwTerminate();
}