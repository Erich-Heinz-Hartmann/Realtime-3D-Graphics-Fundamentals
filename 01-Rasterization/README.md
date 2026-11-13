# 第一次作业：光栅化

《实时三维图形基础》第一次作业的个人实现。在 OpenGL 3.3 窗口中用像素点完成二维图形的光栅化，支持直线和椭圆。

## 实现内容

- Bresenham 直线：整数误差项递推，处理斜率大于 1 和起点在终点右侧的情况
- 中点椭圆：利用四对称性，按切线斜率分成两个区域递推
- 课程框架自带的 DDA 保留下来作为对照
- 鼠标拖拽时实时预览，松开后把像素写入画布
- 左侧 ImGui 面板切换绘制模式

## 构建

第三方依赖已经放在 `deps/`（glfw、glad、imgui），不需要额外下载。

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

Windows 下用 Visual Studio 打开本文件夹也可以，CMake 会生成 `rasterization` 目标。编译后会把 `glfw3.dll` 复制到可执行文件旁边。

## 运行

运行生成的可执行文件，路径取决于生成器，例如 `build/Release/rasterization.exe`。

操作：

- 左侧面板选择 `line (dda)`、`line (bresenham)` 或 `ellipse`
- 按住鼠标左键拖拽绘制，松开后保留
- `Esc` 退出

## 说明

- 像素是通过 `GL_POINTS` 逐个画出来的，没有调用 OpenGL 的线段或椭圆绘制接口。
- 画布上限 10000 个像素点，超过会输出错误信息。
- 工程框架、着色器封装和 DDA 由课程提供，Bresenham 和椭圆光栅化是自己实现的。
