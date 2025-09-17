#pragma once

struct Point3D {
    float x, y, z;
};

struct ProjectedPoint {
    float x2d;
    float y2d;
    float scale;
    bool valid;
};

struct TransformedPoint {
    float x, y, z;
    bool valid;
};