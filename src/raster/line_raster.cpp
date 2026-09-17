#include "raster/line_raster.hpp"
#include "core/pixel.hpp"
#include "core/point.hpp"

void drawLine(Framebuffer& framebuffer, Point start, Point end) {
    int x = start.x;
    int y = start.y;

    int dx = std::abs(end.x - start.x);
    int sx = (start.x < end.x) ? 1 : -1;

    int dy = -std::abs(end.y - start.y);
    int sy = (start.y < end.y) ? 1 : -1;

    int error = dx + dy;

    Pixel white;
    white.on = true;

    while (true) {
        framebuffer.write(x, y, white);

        if (x == end.x && y == end.y) {
            break;
        }

        int twiceError = 2 * error;

        if (twiceError >= dy) {
            error += dy;
            x += sx;
        }

        if (twiceError <= dx) {
            error += dx;
            y += sy;
        }
    }
}