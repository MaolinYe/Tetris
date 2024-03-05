# Tetris 俄罗斯方块
OpenGL实现的俄罗斯方块
![img.png](images/Tetris.png)
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