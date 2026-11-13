# 第二次作业：光照模型与阴影贴图

《实时三维图形基础》第二次作业的个人实现。基于 OpenGL 3.3，实现 MVP 变换、Blinn-Phong 着色和 Shadow Mapping。

## 实现内容

- `src/utils/transform.cpp`：缩放、平移、绕任意轴旋转、model 矩阵，以及 perspective、orthographic、look_at
- `src/shaders/light_shadow.frag`：环境光、漫反射、镜面反射（Blinn-Phong 半程向量）、距离衰减和阴影因子
- `src/main.cpp`：用 1024×1024 的深度纹理作为 shadow map，先渲染光源视角的深度，再从相机视角采样比较
- 场景里有 10 只绕不同轴旋转的小牛，地面使用棋盘格纹理（`plane.obj` 的 UV 基本落在同一处，实际接近单色）
- 空格切换阴影开关

## 构建

依赖在 `deps/`（glfw、glad、imgui、eigen、stb）。

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

目标名为 `rendering`。

## 操作

- `W/A/S/D` 前后左右，`Q/E` 上下，鼠标移动转视角，滚轮缩放
- `空格` 开关阴影
- 右侧面板显示 view/projection 矩阵和 FPS
- `Esc` 退出

## 说明

- 光照参数在 `main.cpp` 里：环境光 0.2，镜面系数 0.8，点光源在 (0, 10, 0)，辐射亮度 (200, 200, 200)。
- 阴影比较加了 bias，避免物体表面出现自阴影条纹。
- 工程框架、相机、模型加载和 GL 封装由课程提供，变换矩阵、光照着色和 shadow mapping 是自己实现的。
