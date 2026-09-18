#include "raster/triangle_raster.hpp"
#include <algorithm>

long long edgeAtSample(
    const Point& a,
    const Point& b,
    long long sampleX4,
    long long sampleY4
) {
    long long ax4 = 4LL * a.x;
    long long ay4 = 4LL * a.y;
    long long bx4 = 4LL * b.x;
    long long by4 = 4LL * b.y;

    return (bx4 - ax4) * (sampleY4 - ay4)
         - (by4 - ay4) * (sampleX4 - ax4);
}

int clampInt(
    int value, 
    int maximum
){
    if (value < 0) { return 0; }
    if (value > maximum) { return maximum; }
    return value;
}

bool isTopLeftEdge(const Point& from, const Point& to) {
    int dx = to.x - from.x;
    int dy = to.y - from.y;

    return dy < 0 || (dy == 0 && dx > 0);
}

bool passesEdge(long long edgeValue, bool isTopLeft) {
    return edgeValue > 0 ||
           (edgeValue == 0 && isTopLeft);
}

TriSetup setupTriangle(
    const Triangle& triangle
){
    const int framebufferMaxX = static_cast<int>(Framebuffer::kWidth) - 1;
    const int framebufferMaxY = static_cast<int>(Framebuffer::kHeight) - 1;
    
    TriSetup setup = {};
    
    int x[3] = {}, y[3] = {};
    x[0] = triangle.a.x; x[1] = triangle.b.x; x[2] = triangle.c.x;
    y[0] = triangle.a.y; y[1] = triangle.b.y; y[2] = triangle.c.y;
    
    int minX = std::min({x[0], x[1], x[2]});
    int minY = std::min({y[0], y[1], y[2]});
    int maxX = std::max({x[0], x[1], x[2]});
    int maxY = std::max({y[0], y[1], y[2]});
    
    long long area = 0;
    
    bool offscreen;
    offscreen = (maxX < 0) || (minX > framebufferMaxX) 
             || (maxY < 0) || (minY > framebufferMaxY);
             
    minX = clampInt(minX, framebufferMaxX);  maxX = clampInt(maxX, framebufferMaxX);
    minY = clampInt(minY, framebufferMaxY);  maxY = clampInt(maxY, framebufferMaxY);
    
    setup.minX = minX; setup.maxX = maxX;
    setup.minY = minY; setup.maxY = maxY;
    
    int startX4 = 4LL * setup.minX + 2;
    int startY4 = 4LL * setup.minY + 2;
    
    Point from[3] = {triangle.b, triangle.c, triangle.a};
    Point to[3]   = {triangle.c, triangle.a, triangle.b};
    
    for (int i = 0; i < 3; ++i) {
        setup.a[i] = 16LL * (from[i].y - to[i].y);
        setup.b[i] = 16LL * (to[i].x - from[i].x);
    }
    
    area = edgeAtSample(
        triangle.a,
        triangle.b,
        4LL * triangle.c.x,
        4LL * triangle.c.y
    );
        
    setup.e0[0] = edgeAtSample(triangle.b, triangle.c, startX4, startY4);
    setup.e0[1] = edgeAtSample(triangle.c, triangle.a, startX4, startY4);
    setup.e0[2] = edgeAtSample(triangle.a, triangle.b, startX4, startY4);
    
    for (int i = 0; i < 3; ++i) {
        if (area < 0) {
            setup.topLeft[i] = isTopLeftEdge(to[i], from[i]);
        } else {
            setup.topLeft[i] = isTopLeftEdge(from[i], to[i]);
        }
    }
    
    if (area < 0){
        for (int i = 0; i<3; i++){
            setup.e0[i] = -setup.e0[i];
            setup.a[i]  = -setup.a[i];
            setup.b[i]  = -setup.b[i];
        }
    }
    
    setup.empty = (area==0) || offscreen;
    
    return setup;    
}

void drawTriangle(
    Framebuffer& framebuffer,
    const TriSetup& setup
) {
    if (setup.empty) {
        return;
    }

    Pixel white;
    white.on = true;

    long long rowE[3] = {
        setup.e0[0],
        setup.e0[1],
        setup.e0[2]
    };

    for (int y = setup.minY; y <= setup.maxY; ++y) {
        long long e[3] = {
            rowE[0],
            rowE[1],
            rowE[2]
        };

        for (int x = setup.minX; x <= setup.maxX; ++x) {
            bool inside =
                passesEdge(e[0], setup.topLeft[0]) &&
                passesEdge(e[1], setup.topLeft[1]) &&
                passesEdge(e[2], setup.topLeft[2]);

            if (inside) {
                framebuffer.write(x, y, white);
            }

            for (int i = 0; i < 3; ++i) {
                e[i] += setup.a[i];
            }
        }

        for (int i = 0; i < 3; ++i) {
            rowE[i] += setup.b[i];
        }
    }
}