# Tetris 俄罗斯方块
C++和OpenGL实现的俄罗斯方块，实现了方块的生成、旋转、移动和消除等关键功能，最难的地方在于框架本身的设计，考虑覆盖，分别渲染三部分：网格线、全部方块和四个方块，方块的旋转和形状由数组事先写好，方块的放置通过更新全部方块的颜色实现，消除方块则是移动全部方块的颜色   
<img src="images/俄罗斯方块.gif" alt="俄罗斯方块" style="width: 375px; height: 725px;">
<img src="images/Tetris.gif" alt="Tetris" style="width: 375px; height: 725px;">
## 棋盘网格
网格线点的位置
```c++
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
```
网格线点的颜色全白
```c++
    glm::vec4 line_points_colors[line_points_num]; // 线点颜色集
    for (auto &line_points_color: line_points_colors) {
        line_points_color = {1, 1, 1, 1}; // 白点
    }
    glGenBuffers(1, &line_points_colors_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, line_points_colors_VBO);
    glBufferData(GL_ARRAY_BUFFER, line_points_num * sizeof(glm::vec4), line_points_colors, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);
```
![img.png](images/Grid.png)
## 方块
计算出全部方块点的位置，一个方块两个三角形六个顶点
```c++
        glm::vec4 cube_all_points[cube_points_num]; // 全部方块点位置
        for(int i=0;i<cube_num_h;i++){
            for(int j=0;j<cube_num_w;j++){
                glm::vec4 p1={cube_size*(j+1),cube_size*(i+1),0,1};
                glm::vec4 p2=p1+glm::vec4 {0,cube_size,0,0};
                glm::vec4 p3=p1+glm::vec4 {cube_size,0,0,0};
                glm::vec4 p4=p1+glm::vec4 {cube_size,cube_size,0,0};
                int number=6*(cube_num_w*i+j); // 一个方块两个三角形六个顶点
                cube_all_points[number]=p1;
                cube_all_points[number+1]=p2;
                cube_all_points[number+2]=p3;
                cube_all_points[number+3]=p2;
                cube_all_points[number+4]=p3;
                cube_all_points[number+5]=p4;
            }
        }
        glGenVertexArrays(1, &cube_all_VAO);
        glBindVertexArray(cube_all_VAO);
        glGenBuffers(1, &cube_all_VBO);
        glBindBuffer(GL_ARRAY_BUFFER, cube_all_VBO);
        glBufferData(GL_ARRAY_BUFFER, cube_points_num * sizeof(glm::vec4), cube_all_points, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(0);
```
方块点颜色
```c++
        for(auto&cube_all_color:cube_all_colors){
            cube_all_color={0,1,1,1}; // 全部方块点的颜色
        }
        glGenBuffers(1, &cube_all_colors_VBO);
        glBindBuffer(GL_ARRAY_BUFFER, cube_all_colors_VBO);
        glBufferData(GL_ARRAY_BUFFER, cube_points_num * sizeof(glm::vec4), cube_all_colors, GL_STATIC_DRAW);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(1);
```
![img.png](images/cube_all.png)
## 俄罗斯方块
先开辟顶点对象
```c++
    // 画俄罗斯的四个方块
    {
        glGenVertexArrays(1, &Tetris_VAO); // 俄罗斯方块
        glBindVertexArray(Tetris_VAO);
        glGenBuffers(1, &Tetris_VBO); // 方块位置
        glBindBuffer(GL_ARRAY_BUFFER,Tetris_VBO);
        glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(glm::vec4), nullptr, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(0);
        glGenBuffers(1,&Tetris_colors_VBO);
        glBindBuffer(GL_ARRAY_BUFFER, Tetris_colors_VBO); // 方块点颜色
        glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(glm::vec4), nullptr, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(1);
    }
```
更新俄罗斯方块的位置，由方块中心位置和相对偏移计算出每个方块顶点的位置
```c++
// 更新俄罗斯方块的位置
void updateTetrisPosition() {
    glBindBuffer(GL_ARRAY_BUFFER, Tetris_VBO);
    for (int i=0;i<4;i++) { // 计算四个方块的位置
        // 由相对位置计算出方块的位置
        float x = TetrisPosition.x + TetrisCubes[i].x;
        float y = TetrisPosition.y + TetrisCubes[i].y;
        // 计算方块四个点位置
        glm::vec4 p1 = {cube_size * (x + 1), cube_size * (y + 1), 0, 1};
        glm::vec4 p2 = p1 + glm::vec4{0, cube_size, 0, 0};
        glm::vec4 p3 = p1 + glm::vec4{cube_size, 0, 0, 0};
        glm::vec4 p4 = p1 + glm::vec4{cube_size, cube_size, 0, 0};
        glm::vec4 tetris_points[6]={p1,p2,p3,p2,p3,p4};
        glBufferSubData(GL_ARRAY_BUFFER, i*6*sizeof(glm::vec4), 6*sizeof(glm::vec4), tetris_points);
    }
}
```
生成新的俄罗斯方块，随机旋转方向，确定方块颜色，更新方块位置
```c++
// 生成新的俄罗斯方块
void newTetris(){
    TetrisPosition={5, 18}; // 初始位置中心
    std::random_device rd;  // 使用随机设备作为种子
    std::mt19937 gen(rd()); // 使用 Mersenne Twister 作为随机数引擎
    std::uniform_int_distribution<int> dis(0, 3); // 生成 [0,3] 范围内的随机整数
    int rotation=dis(gen); // 随机旋转方向
    for(int i=0;i<4;i++){ // 生成一种俄罗斯方块
        TetrisCubes[i]=Tetris_L[rotation][i];
    }
    glm::vec4 TetrisColors[24];
    for (auto & TetrisColor : TetrisColors)
        TetrisColor = {1,0,0,1};
    glBindBuffer(GL_ARRAY_BUFFER, Tetris_colors_VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(TetrisColors), TetrisColors);
    updateTetrisPosition();
}
```
![img.png](images/L_Tetirs.png)
## 旋转俄罗斯方块
检测方块位置合法性
```c++
bool isPositionValid(glm::vec2 cubePosition) {
    if (cubePosition.x >= 0 && cubePosition.x < cube_num_w && cubePosition.y >= 0 && cubePosition.y < cube_num_h)
        return true;
    return false;
}
```
俄罗斯方块不同的旋转方式已经使用数组的方式存储下来，改变旋转方式只需要改变俄罗斯方块的位置索引
```c++
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
```
处理键盘输入，向上（up或者w）旋转方块，使用glfwGetKey会在短时间内多次读取键盘输入状态，使用glfwSetKeyCallback结合press和repeat则不会
```c++
// 处理键盘输入事件
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode) {
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        if (key == GLFW_KEY_ESCAPE)
            glfwSetWindowShouldClose(window, true);
        else if (key == GLFW_KEY_W) {
            rotateTetris();
        } else if (key == GLFW_KEY_S) {}
        else if (key == GLFW_KEY_A) {}
        else if (key == GLFW_KEY_D) {}
    }
}
```
## 移动俄罗斯方块
移动俄罗斯方块中心位置，判断移动后位置是否合法
```c++
// 移动俄罗斯方块
void moveTetris(glm::vec2 move) {
    glm::vec2 newPosition[4];
    for (int i = 0; i < 4; i++) {
        newPosition[i] = TetrisPosition + move + TetrisCubes[i];
        if (!isPositionValid(newPosition[i]))
            return;
    }
    TetrisPosition += move;
    updateTetrisPosition();
}
```
更新键盘输入处理
```c++
// 处理键盘输入事件
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode) {
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        if (key == GLFW_KEY_ESCAPE)
            glfwSetWindowShouldClose(window, true);
        else if (key == GLFW_KEY_W) {
            rotateTetris();
        } else if (key == GLFW_KEY_S) {
            moveTetris({0,-1});
        } else if (key == GLFW_KEY_A) {
            moveTetris({-1,0});
        } else if (key == GLFW_KEY_D) {
            moveTetris({1,0});
        }
    }
}
```
![img.gif](images/移动旋转俄罗斯方块.gif)
## 放置俄罗斯方块
放置俄罗斯方块就是改变某个位置方块的颜色，对于一个方块的位置，更新其对应六个顶点的颜色
```c++
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
```
一个俄罗斯方块由四个方块组成，更新这四个方块位置的颜色，并标记位置为已经填充
```c++
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
```
更新下落事件，当无法下落说明这里就是该方块的归宿
```c++
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
```
增加方块位置合法性检测，让方块叠起来，如果该位置已经被方块填充，位置无效
```c++
// 检查方块位置合法性
bool isPositionValid(glm::vec2 cubePosition) {
    if (cubePosition.x >= 0 && cubePosition.x < cube_num_w && cubePosition.y >= 0 && cubePosition.y < cube_num_h &&
        !cube_filled[(int) cubePosition.x][(int) cubePosition.y])
        return true;
    return false;
}
```
![img.gif](images/放置层叠俄罗斯方块.gif)
## 七种俄罗斯方块
I、T、O、J、L、Z、S
![img.png](images/Tetris7Types.png)
使用相对于中心的位移来表示每种俄罗斯方块的形状，并表示出四种旋转方式的相对位移
![img.png](images/Tetris.png)
```c++
// 七种俄罗斯方块，四种旋转方式，相对于中心的位置偏移
glm::vec2 TetrisTypes[7][4][4] = {
        {{glm::vec2(0, 0),  glm::vec2(-1, 0), glm::vec2(1, 0),  glm::vec2(-1, -1)},    //   L
                {glm::vec2(0, 1),  glm::vec2(0, 0),  glm::vec2(0, -1), glm::vec2(1, -1)},   //
                {glm::vec2(1, 1),  glm::vec2(-1, 0), glm::vec2(0, 0),  glm::vec2(1, 0)},   //
                {glm::vec2(-1, 1),  glm::vec2(0, 1),  glm::vec2(0, 0),  glm::vec2(0, -1)}},

        {{glm::vec2(0, 0),  glm::vec2(-1, 0), glm::vec2(0, -1), glm::vec2(-1, -1)},   // O
                {glm::vec2(0, 0),  glm::vec2(-1, 0), glm::vec2(0, -1), glm::vec2(-1, -1)},
                {glm::vec2(0, 0),  glm::vec2(-1, 0), glm::vec2(0, -1), glm::vec2(-1, -1)},
                {glm::vec2(0, 0),   glm::vec2(-1, 0), glm::vec2(0, -1), glm::vec2(-1, -1)}},

        {{glm::vec2(-2, 0), glm::vec2(-1, 0), glm::vec2(0, 0),  glm::vec2(1, 0)},    // I
                {glm::vec2(0, 1),  glm::vec2(0, 0),  glm::vec2(0, -1), glm::vec2(0, -2)},   //
                {glm::vec2(-2, 0), glm::vec2(-1, 0), glm::vec2(0, 0),  glm::vec2(1, 0)},   //
                {glm::vec2(0, 1),   glm::vec2(0, 0),  glm::vec2(0, -1), glm::vec2(0, -2)}},

        {{glm::vec2(0, 0),  glm::vec2(1, 0),  glm::vec2(0, -1), glm::vec2(-1, -1)},    // S
                {glm::vec2(0, 1),  glm::vec2(0, 0),  glm::vec2(1, 0),  glm::vec2(1, -1)},   //
                {glm::vec2(0, 0),  glm::vec2(1, 0),  glm::vec2(0, -1), glm::vec2(-1, -1)},   //
                {glm::vec2(0, 1),   glm::vec2(0, 0),  glm::vec2(1, 0),  glm::vec2(1, -1)}},

        {{glm::vec2(0, 0),  glm::vec2(-1, 0), glm::vec2(0, -1), glm::vec2(1, -1)},    // Z
                {glm::vec2(0, -1), glm::vec2(0, 0),  glm::vec2(1, 0),  glm::vec2(1, 1)},   //
                {glm::vec2(0, 0),  glm::vec2(-1, 0), glm::vec2(0, -1), glm::vec2(1, -1)},   //
                {glm::vec2(0, -1),  glm::vec2(0, 0),  glm::vec2(1, 0),  glm::vec2(1, 1)}},

        {{glm::vec2(0, 0),  glm::vec2(-1, 0), glm::vec2(1, 0),  glm::vec2(1, -1)},    // J
                {glm::vec2(0, 1),  glm::vec2(0, 0),  glm::vec2(0, -1), glm::vec2(1, 1)},   //
                {glm::vec2(-1, 1), glm::vec2(-1, 0), glm::vec2(0, 0),  glm::vec2(1, 0)},   //
                {glm::vec2(-1, -1), glm::vec2(0, 1),  glm::vec2(0, 0),  glm::vec2(0, -1)}},

        {{glm::vec2(0, 0),  glm::vec2(-1, 0), glm::vec2(1, 0),  glm::vec2(0, -1)},    //  T
                {glm::vec2(0, 1),  glm::vec2(0, 0),  glm::vec2(0, -1), glm::vec2(1, 0)},   //
                {glm::vec2(0, 1),  glm::vec2(-1, 0), glm::vec2(0, 0),  glm::vec2(1, 0)},   //
                {glm::vec2(-1, 0),  glm::vec2(0, 1),  glm::vec2(0, 0),  glm::vec2(0, -1)}}
};
```
七种对应颜色
```c++
glm::vec4 red = {1, 0, 0, 1};
glm::vec4 blue = {0, 0, 1, 1};
glm::vec4 yellow = {0.9, 0.9, 0, 1};
glm::vec4 cyan = {0, 1, 1, 1};
glm::vec4 orange = {1, 0.5, 0, 1};
glm::vec4 green = {0, 1, 0, 1};
glm::vec4 purple = {1, 0, 1, 1};
glm::vec4 TetrisTypeColors[7] = {orange, yellow, cyan, red, green, blue, purple};
```
随机形状和颜色
```c++
// 生成新的俄罗斯方块
void newTetris() {
    TetrisPosition = {5, 18}; // 初始位置中心
    std::random_device rd;  // 使用随机设备作为种子
    std::mt19937 gen(rd()); // 使用 Mersenne Twister 作为随机数引擎
    std::uniform_int_distribution<int> disRotation(0, 3); // 生成 [0,3] 范围内的随机整数
    rotation = disRotation(gen); // 随机旋转方向
    std::uniform_int_distribution<int> disType(0, 6); // 生成 [0,6] 范围内的随机整数
    Type = disType(gen); // 随机形状
    for (int i = 0; i < 4; i++) { // 生成一种俄罗斯方块
        TetrisCubes[i] = TetrisTypes[Type][rotation][i];
    }
    glm::vec4 TetrisColors[24];
    for (auto &TetrisColor: TetrisColors)
        TetrisColor = TetrisTypeColors[Type];
    glBindBuffer(GL_ARRAY_BUFFER, Tetris_colors_VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(TetrisColors), TetrisColors);
    updateTetrisPosition();
}
```
![img.gif](images/七种俄罗斯方块.gif)
## 一定速度下落方块
我们希望能够以一定速度让俄罗斯方块自然下落，并且这个速度可以改变，可以记录帧之间的间隔时间，当这个帧间隔时间大于设置的时间就下落方块
```c++
    double dropTime=1; // 方块下落速度
    double lastTime=glfwGetTime(); // 上一帧的时间
    while (!glfwWindowShouldClose(mainWindow)) {
        if(glfwGetTime()-lastTime>dropTime){ // 以一定速度下落俄罗斯方块
            if (!moveTetris({0, -1})) { // 下落俄罗斯方块
                settleTetris();
                newTetris();
            }
            lastTime=glfwGetTime();
        }
        rendering();
        glfwSwapBuffers(mainWindow); // 交换在此渲染迭代期间用于渲染的颜色缓冲区
        glfwPollEvents(); // 检查是否触发了任何事件（如键盘输入或鼠标移动事件）
    }
```
## 消除方块
之前用的x和y坐标，x是列序，y是行序，就是20×10的存储成10×20的了，方块的消除是按行的，也就是固定y，检测每一行能否可以消除，抹去可以消除的方块存在痕迹，下移方块
```c++
// 消除方块
void eliminate(int row) {
    // 如果这一行有缺口直接返回
    for (auto &i: cube_filled) {
        if (!i[row])
            return;
    }
    // 抹去这一行方块的存在痕迹
    for (auto &i: cube_filled) {
        i[row] = false;
    }
    // 将上面的方块搬下来，每行拷贝上一行的颜色
    for (int i = row; i < cube_num_h - 1; i++) {
        for (int j = 0; j < cube_num_w; j++) {
            cube_filled[j][i] = cube_filled[j][i + 1];
            int number = cube_num_w * i + j;
            glm::vec4 color = cube_all_colors[6 * (number + cube_num_w)];
            for (int k = 0; k < 6; k++) {
                cube_all_colors[6 * number + k] = color;
            }
            changeCubeColor({j, i}, color);
        }
    }
    // 最上面一行无法拷贝，直接改黑
    for (int i = 0; i < cube_num_w; i++) {
        cube_filled[i][cube_num_h - 1] = false;
        for (int j = 0; j < 6; j++) {
            cube_all_colors[6 * (cube_num_w * 19 + i) + j] = black;
        }
        changeCubeColor({i, cube_num_h - 1}, black);
    }
}
```
放置方块之后检查每一行是否可以消除，注意必须从上到下检测每一行，如果从下到上检测会导致因消除而下落的行被跳过检测
```c++
// 放置俄罗斯方块
void settleTetris() {
    for (int i = 0; i < 4; i++) {
        glm::vec2 position = TetrisPosition + TetrisCubes[i];
        int x = position.x;
        int y = position.y;
        cube_filled[x][y] = true;
        changeCubeColor(position, TetrisTypeColors[Type]);
    }
    // 从上到下检测是否可消除
    for(int i=cube_num_h-1;i>=0;i--){
        eliminate(i);
    }
}
```
<img src="images/俄罗斯方块.gif" alt="俄罗斯方块" style="width: 375px; height: 725px;">
<img src="images/Tetris.gif" alt="Tetris" style="width: 375px; height: 725px;">  

## 游戏结束
如果游戏结束，弹窗提示
```c++
bool GameOver = false;
// 游戏结束
void gameOverShow() {
    std::string message = "GameOver!";
    MessageBox(GetForegroundWindow(), message.c_str(), "o.O?",MB_OK);
    exit(EXIT_SUCCESS);
}
```
在生成新的方块的时候检查是否有方块的位置无效，无效则说明游戏应该结束了
```c++
void updateTetrisPosition() {
    glBindBuffer(GL_ARRAY_BUFFER, Tetris_VBO);
    for (int i = 0; i < 4; i++) {
        // 计算四个方块的位置
        // 由相对位置计算出方块的位置
        float x = TetrisPosition.x + TetrisCubes[i].x;
        float y = TetrisPosition.y + TetrisCubes[i].y;
        if (!isPositionValid({x, y})) // 检查是否窗口被占满无法出现新的方块
            GameOver = true;
        // 计算方块四个点位置
        glm::vec4 p1 = {cube_size * (x + 1), cube_size * (y + 1), 0, 1};
        glm::vec4 p2 = p1 + glm::vec4{0, cube_size, 0, 0};
        glm::vec4 p3 = p1 + glm::vec4{cube_size, 0, 0, 0};
        glm::vec4 p4 = p1 + glm::vec4{cube_size, cube_size, 0, 0};
        glm::vec4 tetris_points[6] = {p1, p2, p3, p2, p3, p4};
        glBufferSubData(GL_ARRAY_BUFFER, i * 6 * sizeof(glm::vec4), 6 * sizeof(glm::vec4), tetris_points);
    }
}
```
如果按下Q键也结束
```c++
// 处理键盘输入事件
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode) {
        else if (key == GLFW_KEY_Q) {
            GameOver = true;
        } 
}
```
帧渲染中检查
```c++
// 帧渲染
void rendering() {
    if (GameOver)
        gameOverShow();
    glBindVertexArray(cube_all_VAO); // 画全部方块
    glDrawArrays(GL_TRIANGLES, 0, cube_points_num);
    glBindVertexArray(Tetris_VAO); // 画俄罗斯方块
    glDrawArrays(GL_TRIANGLES, 0, 24);
    glBindVertexArray(line_points_VAO); // 画网格线
    glDrawArrays(GL_LINES, 0, line_points_num);
}
```
## 游戏重新开始
弹窗二次确认，重新初始化
```c++
// 重新开始游戏
void gameRestart() {
    int answer = MessageBox(GetForegroundWindow(), "Do you want to restart the game ?", "o.O?",MB_YESNO);
    if (answer == IDYES)
        initGame();
}
```
新增键盘映射
```c++
// 处理键盘输入事件
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode) {
        else if (key == GLFW_KEY_R) {
            gameRestart();
        }
}
```
## 快速下落
新增键盘映射，按下空格就不断下移直到不能下移
```c++
// 处理键盘输入事件
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode) {
        else if(key==GLFW_KEY_SPACE) {
            while(moveTetris({0,-1}));
        }
}
```
## 暂停游戏
新增键盘映射，按下P键暂停游戏，可以选择继续游戏，也可以选择退出游戏
```c++
// 处理键盘输入事件
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode) {
         else if (key == GLFW_KEY_P) {
            // 暂停游戏
            int answer = MessageBox(GetForegroundWindow(), "Do you want to continue the game ?",
                "Game Pause",MB_YESNO);
            if (answer == IDNO)
                exit(EXIT_SUCCESS);
        }
}
```
## 游戏计分，难度递增
```c++
// 关卡提升
void levelUp() {
    int answer = MessageBox(GetForegroundWindow(), "Do you want to continue?",
                            "LEVEL UP!!!",MB_YESNO);
    if (answer == IDYES) {
        ++level;
        dropTime *= 0.75;
    } else
        gameOverShow();
}
```