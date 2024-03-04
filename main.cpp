#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "shader.h"
#include "glm/detail/type_mat4x4.hpp"
#include "glm/detail/func_trigonometric.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

GLFWwindow *mainWindow;
const int screenWidth = 600; // cube x 12
const int screenHeight = 1100; // cube x 22


void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height); // 设置 OpenGL 渲染窗口的大小，前两个参数设置窗口左下角的位置，第三个和第四个参数以像素为单位设置渲染窗口的宽度和高度
}

void init() {
    glfwInit();  // 初始化GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // 指定创建的内容必须兼容的客户端 API 版本
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // 指定创建的内容必须兼容的客户端 API 版本
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 指定要为其创建内容的 OpenGL 配置文件
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // 指定 OpenGL 上下文是否应向前兼容
    mainWindow = glfwCreateWindow(screenWidth, screenHeight, "俄罗斯方块", nullptr, nullptr);
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

const int cube_size = 50; // 方块大小
const int cube_num_w = 10; // 宽度方块数量
const int cube_num_h = 20; // 高度方块数量
const int line_points_num = 2 * (cube_num_h + cube_num_w + 2); // 网格线点数量
unsigned int line_points_VAO; // 线VAO
unsigned int line_points_VBO; // 线点位置VBO
unsigned int line_points_colors_VBO; // 线点颜色VBO
const int cube_points_num=cube_num_h*cube_num_w*6; // 方块三角形顶点数量
bool cube_filled[cube_num_w][cube_num_h]; // 存在方块与否
glm::vec2 cube_position={5,19}; // 四个方块中心
glm::vec2 cube4[4]; // 四个方块
glm::vec2 cubeL[4][4] = // 四种旋转L方块相对于中心的位置偏移
        {{glm::vec2(0, 0), glm::vec2(-1,0), glm::vec2(1, 0), glm::vec2(-1,-1)},
         {glm::vec2(0, 1), glm::vec2(0, 0), glm::vec2(0,-1), glm::vec2(1, -1)},
         {glm::vec2(1, 1), glm::vec2(-1,0), glm::vec2(0, 0), glm::vec2(1,  0)},
         {glm::vec2(-1,1), glm::vec2(0, 1), glm::vec2(0, 0), glm::vec2(0, -1)}};
void initGame() {
    glm::vec4 line_points[line_points_num]; // 线点集
    for (int i = 0; i <= cube_num_w; i++) { // 竖线
        line_points[2 * i] = {cube_size * (i + 1), cube_size, 0, 1}; // 线下端点
        line_points[2 * i + 1] = line_points[2 * i] + glm::vec4{0, cube_num_h * cube_size, 0, 0}; // 线上端点
    }
    int points_h_num = 2 * (cube_num_w + 1); // 竖线点数量
    for (int i = 0; i <= cube_num_h; i++) { // 横线
        line_points[points_h_num + 2 * i] = {cube_size, cube_size * (i + 1), 0, 1}; // 线左端点
        line_points[points_h_num + 2 * i + 1] =
                line_points[points_h_num + 2 * i] + glm::vec4{cube_num_w * cube_size, 0, 0, 0}; // 线右端点
    }
    glGenVertexArrays(1, &line_points_VAO);
    glBindVertexArray(line_points_VAO);
    glGenBuffers(1, &line_points_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, line_points_VBO);
    glBufferData(GL_ARRAY_BUFFER, line_points_num * sizeof(glm::vec4), line_points, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    glm::vec4 line_points_colors[line_points_num]; // 线点颜色集
    for (auto &line_points_color: line_points_colors) {
        line_points_color = {1, 1, 1, 1}; // 白点
    }
    glGenBuffers(1, &line_points_colors_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, line_points_colors_VBO);
    glBufferData(GL_ARRAY_BUFFER, line_points_num * sizeof(glm::vec4), line_points_colors, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);
}

int main() {
    init();
    initGame();
    Shader shader(R"(C:\Users\Yezi\Desktop\Tetris\shaders\shader.vs)",
                  R"(C:\Users\Yezi\Desktop\Tetris\shaders\shader.fs)");
    shader.use();
    shader.setInt("xsize", screenWidth);
    shader.setInt("ysize", screenHeight);
    while (!glfwWindowShouldClose(mainWindow)) {
        glBindVertexArray(line_points_VAO);
        glDrawArrays(GL_LINES, 0, line_points_num);
        glfwSwapBuffers(mainWindow); // 交换在此渲染迭代期间用于渲染的颜色缓冲区
        glfwPollEvents(); // 检查是否触发了任何事件（如键盘输入或鼠标移动事件）
    }
    glfwTerminate(); // 删除所有已分配的 GLFW 资源
    return 0;
}