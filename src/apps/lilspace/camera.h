#pragma once
#include <cmath>

class Camera {
public:
    Camera() :
        angle(M_PI + M_PI / 2),
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

    // Public fields (match JS style for easy access)
    float angle;
    float thrust;
    float vr; // rotational velocity
    float vx, vy, vz;
    float x, y, z;

private:
    float _halfWidth;
};