# 实时三维图形基础 作业归档

《实时三维图形基础》课程四次作业的代码归档，使用 C++17 和 OpenGL 3.3。四个目录都是独立可构建的工程，依赖已经放在各自的 `deps/` 中。

## 目录

| 目录 | 内容 | 可执行目标 |
|---|---|---|
| `01-Rasterization` | Bresenham 直线、中点椭圆的光栅化 | `rasterization` |
| `02-Lighting-and-Shadow` | MVP 变换、Blinn-Phong 光照、Shadow Mapping | `rendering` |
| `03-Mesh-Simplification` | QEM 网格简化与逐级查看 | `mesh-simplification` |
| `04-Ray-Tracing` | CPU 光线追踪、BVH、介质散射 | `raytracing` |

每个目录的 README 里有实现内容、操作方式和构建说明。

## 构建

需要 Windows、Visual Studio 2022（MSVC）和 CMake 3.16 以上。以第一次作业为例：

```bash
cd 01-Rasterization
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

其余三个目录的构建方式相同，只是目标名不同。也可以直接用 Visual Studio 打开目录，仓库里的 `CMakeSettings.json` 已经配好 Debug/Release。

GLFW、GLAD、ImGui、Eigen、stb 都在各目录的 `deps/` 下，不需要额外下载。因为四份依赖各存了一份，仓库体积约 92MB。

## 说明

- 课程提供的工程框架、相机、模型加载、GL 封装和部分材质/场景代码不是本人写的；每个作业 README 的“说明”一节写明了哪些部分是课程框架、哪些是自己实现的。
- 第四次作业的渲染器结构参考了《Ray Tracing in One Weekend》，球体、四边形、介质、变换、BVH 求交和 `ray_color` 是自己实现的。
- 代码里的 `TODO x.x` 是课程作业的要求标记，下面通常已经是实现，不代表未完成。
- 第四次作业默认 800×800、每像素 10000 个样本，完整渲染大约需要一小时；`04-Ray-Tracing/resources/result.png` 是跑完后的结果。

## 使用声明

这个仓库是个人作业存档，供学习参考。请先独立完成作业，不要直接复制提交。课程框架、第三方库和《Ray Tracing in One Weekend》相关内容遵循各自原始许可。
