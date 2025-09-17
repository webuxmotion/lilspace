#include "camera.h"
#include "camerautils.h"

TransformedPoint Camera::transform(const Point3D& p) const {
    return transformPoint(p, *this);
}