#include "lilspace.h"
#include "camera.h"
#include "cameracontrols.h"

LilSpaceApp::LilSpaceApp() : App("LilSpace") {
}

void LilSpaceApp::run() {
    float vpX = canvas->width() / 2;
    float vpY = canvas->height() / 2;

    uint64_t prevRenderTime = millis();

    Camera camera;

    float steps = 16;
    float angleStep = 2 * M_PI / steps;
    float radius1 = 500;
    float radius2 = 700;

    std::vector<Point3D> circlePoints; // no pre-size

    for (int i = 0; i < steps; i++) {
        float angle = i * angleStep;
        float radius = (i % 2 == 0) ? radius1 : radius2; // even -> radius1, odd -> radius2

        Point3D p = {std::cos(angle) * radius, std::sin(angle) * radius, 1000};
        circlePoints.push_back(p);
    }

    while (1) {
        uint64_t currentTime = millis();
        float dt = (currentTime - prevRenderTime) / 1000.0f; // delta time in seconds

        lilka::State state = lilka::controller.getState();
        handleCameraMovement(camera, state);

        if (state.start.justPressed) {
            return;
        }

        // Animate radius2
        float time = currentTime / 1000.0f; // seconds
        float animatedRadius2 = radius2 + 50.0f * std::sin(time * 8.0f);
        // 100.0f = amplitude of pulsing
        // 4.0f = speed multiplier (radians per second)

        for (int i = 0; i < steps; i++) {
            float angle = i * angleStep;
            float radius = (i % 2 == 0) ? radius1 : animatedRadius2; // animate odd vertices

            Point3D p = {std::cos(angle) * radius, std::sin(angle) * radius, 1000};
            circlePoints[i] = p; // update existing points
        }

        // Draw
        canvas->fillScreen(lilka::colors::Black);

        // Draw lines connecting points
        int len = static_cast<int>(circlePoints.size());
        for (int i = 0; i < len; i++) {
            int next = (i + 1) % len;

            // Base points
            TransformedPoint tp1 = camera.transform(circlePoints[i]);
            TransformedPoint tp2 = camera.transform(circlePoints[next]);

            // Top points (elevated in z)
            Point3D newPoint1 = {circlePoints[i].x, circlePoints[i].y, circlePoints[i].z + 200};
            Point3D newPoint2 = {circlePoints[next].x, circlePoints[next].y, circlePoints[next].z + 200};
            TransformedPoint backTp1 = camera.transform(newPoint1);
            TransformedPoint backTp2 = camera.transform(newPoint2);

            // Draw base
            ProjectedPoint sp1 = camera.project(tp1, vpX, vpY);
            ProjectedPoint sp2 = camera.project(tp2, vpX, vpY);
            if (sp1.valid && sp2.valid) {
                canvas->drawLine(sp1.x2d, sp1.y2d, sp2.x2d, sp2.y2d, lilka::colors::White);
            }

            // Draw top
            ProjectedPoint spTop1 = camera.project(backTp1, vpX, vpY);
            ProjectedPoint spTop2 = camera.project(backTp2, vpX, vpY);
            if (spTop1.valid && spTop2.valid) {
                canvas->drawLine(spTop1.x2d, spTop1.y2d, spTop2.x2d, spTop2.y2d, lilka::colors::White);
            }

            // Draw vertical edges
            if (sp1.valid && spTop1.valid) {
                canvas->drawLine(sp1.x2d, sp1.y2d, spTop1.x2d, spTop1.y2d, lilka::colors::White);
            }
            if (sp2.valid && spTop2.valid) {
                canvas->drawLine(sp2.x2d, sp2.y2d, spTop2.x2d, spTop2.y2d, lilka::colors::White);
            }
        }

        canvas->setCursor(2, 20);
        canvas->println("FPS: " + String(1000 / (millis() - prevRenderTime)));

        camera.update(dt);

        prevRenderTime = millis();

        queueDraw();
    }
}