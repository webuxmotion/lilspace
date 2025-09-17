#pragma once
#include <cmath>
#include "types.h"

class Camera {
public:
    Camera() :
        angle(M_PI + M_PI / 2),
        fl(250.0f),
        thrust(0.0f),
        vr(0.0f),
        vx(0.0f),
        vy(0.0f),
        vz(0.0f),
        x(0.0f),
        y(0.0f),
        z(0.0f),
        _halfWidth(250.0f) {
    }

    TransformedPoint transform(const Point3D& p) const;

    void update() {
        angle += vr;

        float ax = std::cos(angle) * thrust;
        float az = std::sin(angle) * thrust;
        vx += ax;
        vz += az;

        if (std::fabs(vx) > 0.99f) {
            vx *= 0.98f;
            x += vx;
        }
        if (std::fabs(vz) > 0.99f) {
            vz *= 0.98f;
            z += vz;
        }
        y += vy;
    }

    struct Point {
        float x, z;
    };

    Point p1() const {
        return {x + std::cos(angle + M_PI / 2) * _halfWidth, z + std::sin(angle + M_PI / 2) * _halfWidth};
    }

    Point p2() const {
        return {x + std::cos(angle - M_PI / 2) * _halfWidth, z + std::sin(angle - M_PI / 2) * _halfWidth};
    }

    ProjectedPoint project(const TransformedPoint& p, float vpX, float vpY) const {
        ProjectedPoint result{};

        if (!p.valid) {
            result.valid = false;
            return result;
        }

        float scale = fl / (fl + p.z);
        result.x2d = vpX + p.x * scale;
        result.y2d = vpY + (p.y + y) * scale;
        result.scale = scale;
        result.valid = true;

        return result;
    }

    float fl;
    float angle;
    float thrust;
    float vr; // rotational velocity
    float vx, vy, vz;
    float x, y, z;

private:
    float _halfWidth;
};