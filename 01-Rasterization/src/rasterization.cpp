#include "rasterization.h"

void draw_line_dda(Pixel start, Pixel end, std::vector<Pixel>& pixels) {
    int dx = end.x - start.x;
    int dy = end.y - start.y;
    int steps = abs(dx) > abs(dy) ? abs(dx) : abs(dy);
    float x_inc = dx / static_cast<float>(steps);
    float y_inc = dy / static_cast<float>(steps);
    float x = start.x;
    float y = start.y;

    for (int i = 0; i <= steps; i++) {
        pixels.emplace_back(x, y);
        x += x_inc;
        y += y_inc;
    }
}

void draw_line_bresenham(Pixel start, Pixel end, std::vector<Pixel>& pixels) {
    // TODO: 1.1 complete bresenham algorithm
    int x0 = start.x, y0 = start.y;
    int x1 = end.x, y1 = end.y;
    bool steep = abs(y1 - y0) > abs(x1 - x0);
    if (steep) {
        std::swap(x0, y0);
        std::swap(x1, y1);
    }
    if (x0 > x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }
    int dx = x1 - x0;
    int dy = abs(y1 - y0);
    int error = dx / 2;
    int ystep = (y0 < y1) ? 1 : -1;
    int y = y0;
    for (int x = x0; x <= x1; x++) {
        if (steep) {
            pixels.emplace_back(y, x);
        }
        else {
            pixels.emplace_back(x, y);
        }
        error -= dy;
        if (error < 0) {
            y += ystep;
            error += dx;
        }
    }
}

void draw_ellipse(Pixel start, Pixel end, std::vector<Pixel>& pixels) {
    // TODO: 1.2 complete rasterization for ellipse
    int center_x = (start.x + end.x) / 2;
    int center_y = (start.y + end.y) / 2;
    int a = abs(end.x - start.x) / 2;
    int b = abs(end.y - start.y) / 2;

    Pixel center(center_x, center_y);

    int x = 0;
    int y = b;
    int d1 = b * b - a * a * b + a * a / 4;

    while (a * a * (2 * y - 1) > 2 * b * b * (x + 1)) {
        pixels.emplace_back(center.x + x, center.y + y);
        pixels.emplace_back(center.x - x, center.y + y);
        pixels.emplace_back(center.x + x, center.y - y);
        pixels.emplace_back(center.x - x, center.y - y);

        if (d1 < 0) {
            d1 = d1 + b * b * (2 * x + 3);
            x++;
        }
        else {
            d1 = d1 + b * b * (2 * x + 3) + a * a * (-2 * y + 2);
            x++;
            y--;
        }
    }
    double d2 = double(b) * b * (x + 0.5) * (x + 0.5)
              + double(a) * a * (y - 1) * (y - 1)
              - double(a) * a * b * b;

    while (y >= 0) {
        pixels.emplace_back(center.x + x, center.y + y);
        pixels.emplace_back(center.x - x, center.y + y);
        pixels.emplace_back(center.x + x, center.y - y);
        pixels.emplace_back(center.x - x, center.y - y);

        if (d2 < 0) {
            d2 = d2 + b * b * (2 * x + 2) + a * a * (-2 * y + 3);
            x++;
            y--;
        }
        else {
            d2 = d2 + a * a * (-2 * y + 3);
            y--;
        }
    }

    // 补上左右端点，避免扁椭圆在 y = 0 附近漏掉最外侧像素
    pixels.emplace_back(center.x + a, center.y);
    pixels.emplace_back(center.x - a, center.y);
}
