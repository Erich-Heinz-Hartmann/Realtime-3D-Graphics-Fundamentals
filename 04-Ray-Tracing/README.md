# 第四次作业：光线追踪

《实时三维图形基础》第四次作业的个人实现。实现一个 CPU 光线追踪器，渲染 RTIOW 的经典场景，并实时显示结果。

## 实现内容

- `src/raytracing/hit.cpp`
  - 球体求交，支持运动模糊（按时间插值球心）
  - 四边形求交
  - 用 6 个四边形拼盒子
  - 常量密度介质的体积散射
  - 平移和绕 Y 轴旋转
  - BVH 节点求交，用轴对齐包围盒剪枝
- `src/raytracing/camera.cpp`：递归的 `ray_color`，累加自发光和散射衰减
- `src/main.cpp`：搭建场景、多线程按行渲染、把结果上传为纹理实时显示，渲染完成后保存 PNG

## 渲染参数

- 分辨率 800×800，每像素 10000 个样本，最大反弹深度 10，FOV 40°
- 相机位置 (478, 278, -600)，看向 (278, 278, 0)，无景深
- 场景包含 20×20 的随机高度盒子（BVH 加速）、发光四边形、金属球、玻璃球、介质球、贴图球和噪声纹理球

## 构建

依赖在 `deps/`（glfw、glad、stb、eigen）。

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

MSVC 下会开启 `/arch:AVX2 /fp:fast`。目标名为 `raytracing`。Debug 模式下渲染会很慢，建议用 Release。

## 运行

直接运行可执行文件，窗口会实时显示渲染进度（每 100 帧刷新一次纹理），完成后在 `resources/result.png` 生成图片。`Esc` 退出。

## 说明

- 渲染线程数取 `hardware_concurrency() * 3 / 4`，至少 1 个；核心数少于 4 时也不会出现 0 线程。
- 代码里保留了单线程的 `render_image`，想离线慢慢渲染可以换成它。
- 渲染器骨架、材质、纹理和场景搭建来自课程和《Ray Tracing in One Weekend》，球体/四边形/介质/变换/BVH 求交和 `ray_color` 是自己实现的。
