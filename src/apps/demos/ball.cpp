#include "ball.h"

struct Point3D {
    float x, y, z;
};

BallApp::BallApp() : App("Ball") {
}

void BallApp::run() {
    // Perspective settings
    float fl = 250.0f; // focal length
    float vpX = canvas->width() / 2;
    float vpY = canvas->height() / 2;

    // Generate points
    const int numLayers = 120;
    const int rectSize = 120;
    const float zStep = 240.0f;

    const uint16_t palette[] = {
        0x0320, // very dark green (deepest)
        0x0640, // dark green
        0x0C80, // medium green
        0x19E0, // light green
        0x3FE0, // yellow-green
        0xFFE0 // bright yellow (closest)
    };
    const int paletteSize = sizeof(palette) / sizeof(palette[0]);

    Point3D points[numLayers * 4];
    for (int i = 0; i < numLayers; i++) {
        int idx = i * 4;
        points[idx + 0] = {-rectSize, -rectSize, zStep * (i + 1)};
        points[idx + 1] = {rectSize, -rectSize, zStep * (i + 1)};
        points[idx + 2] = {rectSize, rectSize, zStep * (i + 1)};
        points[idx + 3] = {-rectSize, rectSize, zStep * (i + 1)};
    }

    // Precompute theoretical z range and corresponding scale range
    const float minZ = 1.0f;
    const float maxZ = numLayers * zStep; // matches your reset value
    const float scaleMin = fl / (fl + maxZ); // smallest possible scale (farthest)
    const float scaleMax = fl / (fl + minZ); // largest possible scale (nearest)

    float dx = 0, dy = 0; // camera offsets
    uint64_t prevRenderTime = millis();
    uint64_t startTime = millis();

    while (1) {
        float delta = (millis() - prevRenderTime) / 1000.0f;

        // Controller input
        lilka::State state = lilka::controller.getState();
        if (state.a.justPressed) {
            return;
        } else if (state.up.pressed) {
            dy -= 200 * delta;
        } else if (state.down.pressed) {
            dy += 200 * delta;
        } else if (state.left.pressed) {
            dx -= 200 * delta;
        } else if (state.right.pressed) {
            dx += 200 * delta;
        }

        // Move all points forward in Z
        for (int i = 0; i < numLayers * 4; i++) {
            points[i].z -= 800 * delta;
            if (points[i].z < 1) {
                points[i].z = numLayers * zStep; // reset behind
            }
        }

        // --- Z-SORT (draw farthest first) ---
        std::sort(points, points + numLayers * 4, [](const Point3D& a, const Point3D& b) {
            return a.z > b.z; // bigger z first
        });

        // Draw
        canvas->fillScreen(lilka::colors::Black);

        for (int i = 0; i < numLayers * 4; i++) {
            Point3D p = points[i];

            // Perspective scale
            float scale = fl / (fl + p.z);
            float x2d = vpX + (p.x + dx * 4) * scale;
            float y2d = vpY + (p.y + dy * 4) * scale;
            int radius = 30 * scale;

            // --- normalize scale into [0..1] using precomputed range ---
            float normalized = 0.0f;
            if (scaleMax != scaleMin) {
                normalized = (scale - scaleMin) / (scaleMax - scaleMin);
            }
            // clamp robustly
            normalized = std::max(0.0f, std::min(1.0f, normalized));

            // map to palette index (round for better distribution)
            int colorIndex = (int)roundf(normalized * (paletteSize - 1));
            colorIndex = std::max(0, std::min(paletteSize - 1, colorIndex));

            uint16_t color = palette[colorIndex];

            canvas->fillCircle(x2d, y2d, radius, color);
        }

        // FPS counter
        canvas->setCursor(8, 20);
        canvas->println("FPS: " + String(1000 / (millis() - prevRenderTime)));

        prevRenderTime = millis();
        queueDraw();
    }
}