#pragma once
#include "camera.h"
#include <cmath>
#include <string>

struct Point3D {
    float x, y, z;
};

struct TransformedPoint {
    float x, y, z;
    bool valid; // true if the point is on the "right" side
};

// Return the closest point on a line (x1,y1)-(x2,y2) to point (px, py)
struct ClosestPointResult {
    float x, y;
    float t;
};

inline ClosestPointResult closestPointOnLine(float x1, float y1, float x2, float y2, float px, float py) {
    float dx = x2 - x1;
    float dy = y2 - y1;
    float t = ((px - x1) * dx + (py - y1) * dy) / (dx * dx + dy * dy);
    float cx = x1 + t * dx;
    float cy = y1 + t * dy;
    return { cx, cy, t };
}

// Determine which side of the line (x1,y1)-(x2,y2) the point (px, py) is on
inline std::string pointSide(float x1, float y1, float x2, float y2, float px, float py) {
    float dx = x2 - x1;
    float dy = y2 - y1;
    float dxp = px - x1;
    float dyp = py - y1;
    float cross = dx * dyp - dy * dxp;

    if (cross > 0) return "left";
    else if (cross < 0) return "right";
    else return "on the line";
}

TransformedPoint transformPoint(const Point3D& p, const Camera& camera);

// Transform a point relative to camera
inline TransformedPoint transformPoint(const Point3D& p, const Camera& camera) {
    
    ClosestPointResult closest = closestPointOnLine(
        camera.p1().x, camera.p1().z,
        camera.p2().x, camera.p2().z,
        p.x, p.z
    );

    std::string side = pointSide(
        camera.p1().x, camera.p1().z,
        camera.p2().x, camera.p2().z,
        p.x, p.z
    );

    if (side != "right") return {0, 0, 0, false};

    float dx = camera.x - closest.x;
    float dz = camera.z - closest.y;
    float distance = std::sqrt(dx * dx + dz * dz);
    if (closest.t <= 0.5f) distance = -distance;

    float dx2 = p.x - closest.x;
    float dz2 = p.z - closest.y;
    float distanceForZ = std::sqrt(dx2 * dx2 + dz2 * dz2);

    return { distance, p.y, distanceForZ, true };
}