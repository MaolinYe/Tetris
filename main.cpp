#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "shader.h"

GLFWwindow *mainWindow;
const int screenWidth=512;
const int screenHeight=512;

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height); // 设置 OpenGL 渲染窗口的大小，前两个参数设置窗口左下角的位置，第三个和第四个参数以像素为单位设置渲染窗口的宽度和高度
}

void init() {
    glfwInit();  // 初始化GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // 指定创建的内容必须兼容的客户端 API 版本
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // 指定创建的内容必须兼容的客户端 API 版本
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 指定要为其创建内容的 OpenGL 配置文件
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // 指定 OpenGL 上下文是否应向前兼容
    mainWindow = glfwCreateWindow(screenWidth, screenHeight, "LearnOpenGL", nullptr, nullptr);
    if (mainWindow == nullptr) {
        std::cout << "Failed to create GLFW mainWindow" << std::endl;
        glfwTerminate();
        exit(-1);
    }
    glfwMakeContextCurrent(mainWindow); // 告诉 GLFW 将窗口的内容作为当前线程上的主要内容
    glfwSetFramebufferSizeCallback(mainWindow, framebuffer_size_callback); // 注册窗口调整调用函数
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) { // 初始化 GLAD
        std::cout << "Failed to initialize GLAD" << std::endl;
        exit(-1);
    }
}

int main() {
    init();
    Shader shader(R"(C:\Users\Yezi\Desktop\Tetris\shaders\shader.fs)",
                  R"(C:\Users\Yezi\Desktop\Tetris\shaders\shader.fs)");
    while (!glfwWindowShouldClose(mainWindow)) {
        glfwSwapBuffers(mainWindow); // 交换在此渲染迭代期间用于渲染的颜色缓冲区
        glfwPollEvents(); // 检查是否触发了任何事件（如键盘输入或鼠标移动事件）
    }
    glfwTerminate(); // 删除所有已分配的 GLFW 资源
    return 0;
}