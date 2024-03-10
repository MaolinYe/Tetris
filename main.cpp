#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "shader.h"
#include "glm/detail/type_mat4x4.hpp"
#include "glm/detail/func_trigonometric.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <random>
#include <unistd.h>

GLFWwindow *mainWindow;
const int screenWidth = 600; // cube x 12
const int screenHeight = 1100; // cube x 22
const int cube_size = 50; // 方块大小
const int cube_num_w = 10; // 宽度方块数量
const int cube_num_h = 20; // 高度方块数量
const int line_points_num = 2 * (cube_num_h + cube_num_w + 2); // 网格线点数量
unsigned int line_points_VAO; // 线VAO
unsigned int line_points_VBO; // 线点位置VBO
unsigned int line_points_colors_VBO; // 线点颜色VBO
unsigned int cube_all_VAO; // 所有方块VAO
unsigned int cube_all_VBO; // 所有方块点位置VBO
unsigned int cube_all_colors_VBO; // 所有方块点颜色VBO
const int cube_points_num = cube_num_h * cube_num_w * 6; // 方块三角形顶点数量
glm::vec4 cube_all_colors[cube_points_num]; // 所有方块点颜色
bool cube_filled[cube_num_w][cube_num_h]; // 存在方块与否
unsigned int Tetris_VAO; // 俄罗斯四个方块VAO
unsigned int Tetris_VBO; // 俄罗斯四个方块位置VBO
unsigned int Tetris_colors_VBO; // 俄罗斯方块点颜色
glm::vec2 TetrisPosition = {5, 18}; // 四个方块中心
glm::vec2 TetrisCubes[4]; // 俄罗斯四个方块相互的位置
int rotation = 0;
glm::vec2 Tetris_L[4][4] = // L方块四种旋转相对于中心的位置偏移
        {{glm::vec2(0, 0),  glm::vec2(-1, 0), glm::vec2(1, 0),  glm::vec2(-1, -1)},
         {glm::vec2(0, 1),  glm::vec2(0, 0),  glm::vec2(0, -1), glm::vec2(1, -1)},
         {glm::vec2(1, 1),  glm::vec2(-1, 0), glm::vec2(0, 0),  glm::vec2(1, 0)},
         {glm::vec2(-1, 1), glm::vec2(0, 1),  glm::vec2(0, 0),  glm::vec2(0, -1)}};
glm::vec4 red = {1, 0, 0, 1};

// 更新俄罗斯方块的位置
void updateTetrisPosition() {
    glBindBuffer(GL_ARRAY_BUFFER, Tetris_VBO);
    for (int i = 0; i < 4; i++) { // 计算四个方块的位置
        // 由相对位置计算出方块的位置
        float x = TetrisPosition.x + TetrisCubes[i].x;
        float y = TetrisPosition.y + TetrisCubes[i].y;
        // 计算方块四个点位置
        glm::vec4 p1 = {cube_size * (x + 1), cube_size * (y + 1), 0, 1};
        glm::vec4 p2 = p1 + glm::vec4{0, cube_size, 0, 0};
        glm::vec4 p3 = p1 + glm::vec4{cube_size, 0, 0, 0};
        glm::vec4 p4 = p1 + glm::vec4{cube_size, cube_size, 0, 0};
        glm::vec4 tetris_points[6] = {p1, p2, p3, p2, p3, p4};
        glBufferSubData(GL_ARRAY_BUFFER, i * 6 * sizeof(glm::vec4), 6 * sizeof(glm::vec4), tetris_points);
    }
}

// 生成新的俄罗斯方块
void newTetris() {
    TetrisPosition = {5, 18}; // 初始位置中心
    std::random_device rd;  // 使用随机设备作为种子
    std::mt19937 gen(rd()); // 使用 Mersenne Twister 作为随机数引擎
    std::uniform_int_distribution<int> dis(0, 3); // 生成 [0,3] 范围内的随机整数
    rotation = dis(gen); // 随机旋转方向
    for (int i = 0; i < 4; i++) { // 生成一种俄罗斯方块
        TetrisCubes[i] = Tetris_L[rotation][i];
    }
    glm::vec4 TetrisColors[24];
    for (auto &TetrisColor: TetrisColors)
        TetrisColor = {1, 0, 0, 1};
    glBindBuffer(GL_ARRAY_BUFFER, Tetris_colors_VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(TetrisColors), TetrisColors);
    updateTetrisPosition();
}

// 检查方块位置合法性
bool isPositionValid(glm::vec2 cubePosition) {
    if (cubePosition.x >= 0 && cubePosition.x < cube_num_w && cubePosition.y >= 0 && cubePosition.y < cube_num_h &&
        !cube_filled[(int) cubePosition.x][(int) cubePosition.y])
        return true;
    return false;
}

// 旋转俄罗斯方块
void rotateTetris() {
    int nextRotation = (rotation + 1) % 4;
    for (int i = 0; i < 4; i++) {
        if (!isPositionValid(TetrisPosition + Tetris_L[nextRotation][i]))
            return;
    }
    rotation = nextRotation;
    for (int i = 0; i < 4; i++) { // 生成一种俄罗斯方块
        TetrisCubes[i] = Tetris_L[rotation][i];
    }
    updateTetrisPosition();
}

// 移动俄罗斯方块
bool moveTetris(glm::vec2 move) {
    glm::vec2 newPosition[4];
    for (int i = 0; i < 4; i++) {
        newPosition[i] = TetrisPosition + move + TetrisCubes[i];
        if (!isPositionValid(newPosition[i]))
            return false;
    }
    TetrisPosition += move;
    updateTetrisPosition();
    return true;
}


// 改变单个方块的颜色
void changeCubeColor(glm::vec2 cubePosition, glm::vec4 color) {
    int offset = 6 * (cubePosition.x + cubePosition.y * cube_num_w);
    glm::vec4 colors[6] = {color, color, color, color, color, color};
    glBindBuffer(GL_ARRAY_BUFFER, cube_all_colors_VBO);
    glBufferSubData(GL_ARRAY_BUFFER, offset * sizeof(glm::vec4), sizeof(colors), colors);
    for (int i = 0; i < 4; i++) {
        cube_all_colors[offset + i] = color;
    }
}

// 放置俄罗斯方块
void settleTetris() {
    for (int i = 0; i < 4; i++) {
        glm::vec2 position = TetrisPosition + TetrisCubes[i];
        int x = position.x;
        int y = position.y;
        cube_filled[x][y] = true;
        changeCubeColor(position, red);
    }
}

// 处理键盘输入事件
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode) {
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        if (key == GLFW_KEY_ESCAPE)
            glfwSetWindowShouldClose(window, true);
        else if (key == GLFW_KEY_W) {
            rotateTetris();
        } else if (key == GLFW_KEY_S) { // 如果不能下落说明到位置放置了
            if (!moveTetris({0, -1})) {
                settleTetris();
                newTetris();
            }
        } else if (key == GLFW_KEY_A) {
            moveTetris({-1, 0});
        } else if (key == GLFW_KEY_D) {
            moveTetris({1, 0});
        }
    }
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height); // 设置 OpenGL 渲染窗口的大小，前两个参数设置窗口左下角的位置，第三个和第四个参数以像素为单位设置渲染窗口的宽度和高度
}

// OpenGL初始化
void init() {
    // 初始化GLFW窗口和GLAD
    {
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
        if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) { // 初始化 GLAD
            std::cout << "Failed to initialize GLAD" << std::endl;
            exit(-1);
        }
    }
    glfwSetFramebufferSizeCallback(mainWindow, framebuffer_size_callback); // 注册窗口调整调用函数
    glfwSetKeyCallback(mainWindow, key_callback); // 注册键盘输入事件
}

// 游戏初始化
void initGame() {
    // 画网格线
    {
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
        glGenVertexArrays(1, &line_points_VAO); // 网格线
        glBindVertexArray(line_points_VAO);
        glGenBuffers(1, &line_points_VBO);
        glBindBuffer(GL_ARRAY_BUFFER, line_points_VBO);
        glBufferData(GL_ARRAY_BUFFER, line_points_num * sizeof(glm::vec4), line_points, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(0);
        glm::vec4 line_points_colors[line_points_num]; // 线点颜色集
        for (auto &line_points_color: line_points_colors) {
            line_points_color = {1, 1, 1, 1}; // 白点
        }
        glGenBuffers(1, &line_points_colors_VBO);
        glBindBuffer(GL_ARRAY_BUFFER, line_points_colors_VBO);
        glBufferData(GL_ARRAY_BUFFER, line_points_num * sizeof(glm::vec4), line_points_colors, GL_STATIC_DRAW);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(1);
    }
    // 画所有的方块
    {
        glm::vec4 cube_all_points[cube_points_num]; // 全部方块点位置
        for (int i = 0; i < cube_num_h; i++) {
            for (int j = 0; j < cube_num_w; j++) {
                glm::vec4 p1 = {cube_size * (j + 1), cube_size * (i + 1), 0, 1};
                glm::vec4 p2 = p1 + glm::vec4{0, cube_size, 0, 0};
                glm::vec4 p3 = p1 + glm::vec4{cube_size, 0, 0, 0};
                glm::vec4 p4 = p1 + glm::vec4{cube_size, cube_size, 0, 0};
                int number = 6 * (cube_num_w * i + j); // 一个方块两个三角形六个顶点
                cube_all_points[number] = p1;
                cube_all_points[number + 1] = p2;
                cube_all_points[number + 2] = p3;
                cube_all_points[number + 3] = p2;
                cube_all_points[number + 4] = p3;
                cube_all_points[number + 5] = p4;
            }
        }
        glGenVertexArrays(1, &cube_all_VAO); // 全部方块
        glBindVertexArray(cube_all_VAO);
        glGenBuffers(1, &cube_all_VBO); // 方块位置
        glBindBuffer(GL_ARRAY_BUFFER, cube_all_VBO);
        glBufferData(GL_ARRAY_BUFFER, cube_points_num * sizeof(glm::vec4), cube_all_points, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(0);
        for (auto &cube_all_color: cube_all_colors) {
            cube_all_color = {0, 1, 1, 1}; // 全部方块点的颜色
        }
        glGenBuffers(1, &cube_all_colors_VBO);
        glBindBuffer(GL_ARRAY_BUFFER, cube_all_colors_VBO);
        glBufferData(GL_ARRAY_BUFFER, cube_points_num * sizeof(glm::vec4), cube_all_colors, GL_STATIC_DRAW);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(1);
    }
    // 画俄罗斯的四个方块
    {
        glGenVertexArrays(1, &Tetris_VAO); // 俄罗斯方块
        glBindVertexArray(Tetris_VAO);
        glGenBuffers(1, &Tetris_VBO); // 方块位置
        glBindBuffer(GL_ARRAY_BUFFER, Tetris_VBO);
        glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(glm::vec4), nullptr, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(0);
        glGenBuffers(1, &Tetris_colors_VBO);
        glBindBuffer(GL_ARRAY_BUFFER, Tetris_colors_VBO); // 方块点颜色
        glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(glm::vec4), nullptr, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(1);
    }
}

int main() {
    init();
    initGame();
    newTetris();
    Shader shader(R"(C:\Users\Yezi\Desktop\Tetris\shaders\shader.vs)",
                  R"(C:\Users\Yezi\Desktop\Tetris\shaders\shader.fs)");
    shader.use();
    shader.setInt("xsize", screenWidth);
    shader.setInt("ysize", screenHeight);
    while (!glfwWindowShouldClose(mainWindow)) {
        glBindVertexArray(cube_all_VAO); // 画全部方块
        glDrawArrays(GL_TRIANGLES, 0, cube_points_num);
        glBindVertexArray(Tetris_VAO); // 画俄罗斯方块
        glDrawArrays(GL_TRIANGLES, 0, 24);
        glBindVertexArray(line_points_VAO); // 画网格线
        glDrawArrays(GL_LINES, 0, line_points_num);
        glfwSwapBuffers(mainWindow); // 交换在此渲染迭代期间用于渲染的颜色缓冲区
        glfwPollEvents(); // 检查是否触发了任何事件（如键盘输入或鼠标移动事件）
    }
    glfwTerminate(); // 删除所有已分配的 GLFW 资源
    return 0;
}