#include "lilspace.h"
#include "camera.h"
#include "camerautils.h"

struct DrawObject {
    float x2d;
    float y2d;
    float radius;
    uint16_t color;
    float z; // for z-sorting
};

struct ColorPoint {
    Point3D point;
    uint16_t color;
};

LilSpaceApp::LilSpaceApp() : App("LilSpace") {
}

uint16_t makeColor(uint8_t r, uint8_t g, uint8_t b) {
    return ((r & 0x1F) << 11) | ((g & 0x3F) << 5) | (b & 0x1F);
}

void LilSpaceApp::run() {
    // Perspective settings
    float fl = 250.0f; // focal length
    float vpX = canvas->width() / 2;
    float vpY = canvas->height() / 2;

    // Generate points
    const int numLayers = 20;
    const int rectSize = 120;
    const float zStep = 240.0f;

    const int numPoints = 32;
    std::vector<ColorPoint> colorPoints(numPoints);
    const float zStepColor = 70.0f;
    const float x = 0.0f, y = 0.0f;

    for (int r = 0; r < numPoints; r++) {
        colorPoints[r].point = {x, y, zStepColor * r};
        colorPoints[r].color = makeColor(r, 0, 0);
    }

    colorPoints.erase(colorPoints.begin());

    // how to delete the first element, where the color is 0?

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

    uint64_t prevRenderTime = millis();
    uint64_t startTime = millis();

    Camera camera;

    while (1) {
        float delta = (millis() - prevRenderTime) / 1000.0f;

        // Controller input
        lilka::State state = lilka::controller.getState();

        // Forward/back
        if (state.up.pressed) {
            camera.thrust = -2.0f; // accelerate forward
        } else if (state.down.pressed) {
            camera.thrust = 2.0f; // accelerate backward
        } else {
            camera.thrust = 0.0f;
        }

        // Rotate left/right
        if (state.d.pressed) {
            camera.vr = -0.05f;
        } else if (state.a.pressed) {
            camera.vr = 0.05f;
        } else {
            camera.vr = 0.0f;
        }

        // Vertical move
        if (state.c.pressed) {
            camera.vy = 10.0f; // go up
        } else if (state.b.pressed) {
            camera.vy = -10.0f; // go down
        } else {
            camera.vy = 0.0f;
        }

        if (state.start.justPressed) {
            return;
        }

        std::vector<DrawObject> objectsToDraw;

        for (const auto& cp : colorPoints) {
            TransformedPoint tPoint = transformPoint(cp.point, camera);

            if (!tPoint.valid) continue;

            float scale = fl / (fl + tPoint.z);
            float x2d = vpX + tPoint.x * scale;
            float y2d = vpY + (tPoint.y + camera.y) * scale;
            float radius = (zStepColor / 2) * scale;

            uint16_t color = cp.color;

            objectsToDraw.push_back(DrawObject{x2d, y2d, radius, color, tPoint.z});
        }

        // Sort farthest first
        std::sort(objectsToDraw.begin(), objectsToDraw.end(), [](const DrawObject& a, const DrawObject& b) {
            return a.z > b.z;
        });

        // Draw
        canvas->fillScreen(lilka::colors::Black);

        for (const auto& obj : objectsToDraw) {
            //canvas->fillCircle(obj.x2d, obj.y2d, obj.radius, obj.color);

            float rectWidth = obj.radius * 2.0f + 10;
            float rectHeight = obj.radius * 40.0f;
            float rectX = obj.x2d - rectWidth / 2.0f; // shift to top-left
            float rectY = obj.y2d - rectHeight / 2.0f; // shift to top-left

            canvas->fillRect(rectX, rectY, rectWidth, rectHeight, obj.color);
        }

        // FPS counter
        canvas->setFont(FONT_9x15);

        canvas->setCursor(2, 20);
        canvas->println("FPS: " + String(1000 / (millis() - prevRenderTime)));

        canvas->setCursor(2, 20 + 15 * 1);
        canvas->println("Exit->'Start'");

        // canvas->setCursor(2, 20 + 15 * 2);
        // canvas->println("x:" + String(camera.x));
        // canvas->setCursor(2, 20 + 15 * 3);
        // canvas->println("y:" + String(camera.y));
        // canvas->setCursor(2, 20 + 15 * 4);
        // canvas->println("z:" + String(camera.z));
        // canvas->setCursor(2, 20 + 15 * 5);
        // canvas->println("vr:" + String(camera.vr));
        // canvas->setCursor(2, 20 + 15 * 6);
        // canvas->println("vy:" + String(camera.vy));
        // canvas->setCursor(2, 20 + 15 * 7);
        // canvas->println("vz:" + String(camera.vz));

        camera.update();

        prevRenderTime = millis();
        queueDraw();
    }
}