#include <iostream>
#include <cassert>

#include <vulkan/vulkan.h>
#include "GLFW/glfw3.h"

#include <windows.h>

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
    if (err == -1)
    {
        MessageBoxA(NULL, "Failed to initialize GLFW!", "Error", MB_OK | MB_ICONERROR);
        return EXIT_FAILURE;
    }

    assert(err != -1 && "Failed to initialize GLFW!");

    if (glfwVulkanSupported() == GLFW_FALSE)
    {
        MessageBoxA(NULL, "This system doesn't support Vulkan!", "Error", MB_OK | MB_ICONERROR);
        glfwTerminate();
        return EXIT_FAILURE;
    }
    
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    GLFWwindow* window = glfwCreateWindow(1920, 1080, "Iceberg Application", nullptr, nullptr);
    if (!window)
    {
        MessageBoxA(NULL, "Failed to create a window!", "Error", MB_OK | MB_ICONERROR);
        glfwTerminate();
        return EXIT_FAILURE;
    }

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