#include "cameracontrols.h"

void handleCameraMovement(Camera& camera, const lilka::State& state) {
    // Forward/back
    if (state.up.pressed) {
        camera.thrust = -400.0f; // accelerate forward
    } else if (state.down.pressed) {
        camera.thrust = 400.0f;  // accelerate backward
    } else {
        camera.thrust = 0.0f;
    }

    // Rotate left/right
    if (state.d.pressed) {
        camera.vr = -10.0f;
    } else if (state.a.pressed) {
        camera.vr = 10.0f;
    } else {
        camera.vr = 0.0f;
    }

    // Vertical move
    if (state.c.pressed) {
        camera.vy = 50.0f; // go up
    } else if (state.b.pressed) {
        camera.vy = -50.0f; // go down
    } else {
        camera.vy = 0.0f;
    }
}