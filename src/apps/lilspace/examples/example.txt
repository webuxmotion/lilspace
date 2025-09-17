#include "lilspace.h"
#include "camera.h"
#include "cameracontrols.h"

LilSpaceApp::LilSpaceApp() : App("LilSpace") {
}

void LilSpaceApp::run() {
    float vpX = canvas->width() / 2;
    float vpY = canvas->height() / 2;

    Point3D p1 = {0, 0, 0};
    Point3D p2 = {0, 200, 200};

    Camera camera;

    while (1) {
        lilka::State state = lilka::controller.getState();

        handleCameraMovement(camera, state);

        if (state.start.justPressed) {
            return;
        }

        // Transform
        TransformedPoint tp1 = camera.transform(p1);
        TransformedPoint tp2 = camera.transform(p2);

        // Project
        ProjectedPoint sp1 = camera.project(tp1, vpX, vpY);
        ProjectedPoint sp2 = camera.project(tp2, vpX, vpY);

        // Draw
        canvas->fillScreen(lilka::colors::Black);

        if (sp1.valid && sp2.valid) {
            canvas->drawLine(sp1.x2d, sp1.y2d, sp2.x2d, sp2.y2d, lilka::colors::White);
        }

        camera.update();

        queueDraw();
    }
}