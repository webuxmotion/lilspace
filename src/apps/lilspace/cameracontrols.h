#pragma once
#include "camera.h"
#include "lilka.h"  // for lilka::State

// Updates camera movement based on controller state
void handleCameraMovement(Camera& camera, const lilka::State& state);