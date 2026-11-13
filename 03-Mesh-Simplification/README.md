# 第三次作业：QEM 网格简化

《实时三维图形基础》第三次作业的个人实现。实现 Garland & Heckbert 的 Quadric Error Metrics 网格简化，并在课程查看器里逐级简化模型。

## 实现内容

- 为每个三角面构造平面方程，得到 4×4 二次误差矩阵，累加到三个顶点
- 合并一条边的代价：两顶点的 Q 矩阵相加，解线性方程组求最优新顶点；方程奇异时在两端点和中点中取代价最小的
- 用 `std::priority_queue` 维护边，`std::map` 维护边索引；每次取最小代价边合并，更新顶点位置和 Q 矩阵，把退化面标记为删除，并重算受影响边的代价
- 合并后保留历史网格，界面可以前后切换查看
- ImGui 面板显示当前顶点数和面数，支持线框显示和重置

## 构建

依赖在 `deps/`（glfw、glad、imgui、eigen）。

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

目标名为 `mesh-simplification`。

## 操作

- `Ratio` 滑块设置简化比例（0.1~0.9）
- `Next >>>` 对当前网格再简化一次，保留上一级
- `<<< Prev` 返回上一级
- `Show Meshes` 显示三角网格线框
- `Reload` 重新加载原始模型
- `Esc` 退出

## 说明

- 默认模型是 `resources/squirrel.obj`。
- 每次 `Next` 都是在当前结果上继续乘比例，所以连续点击会得到越来越简化的网格；面数低于 200 时不再继续。
- 查看器和模型加载由课程提供，QEM 算法在 `src/mesh_simplification.cpp` 中自己实现。
