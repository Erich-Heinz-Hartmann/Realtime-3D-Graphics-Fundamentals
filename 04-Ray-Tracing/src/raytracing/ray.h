#ifndef RAYTRACING_RAY_H
#define RAYTRACING_RAY_H

#include "Eigen/Dense"

#include "utils/tools.h"

namespace RayTracing {

class Ray {
public:
    vecd3 origin;
    vecd3 direction;
    double time;

    Ray() {}
    
    Ray(const vecd3& origin, const vecd3& direction, double time = 0.0)
        : origin(origin), direction(direction), time(time) {}
    
    vecd3 at(double t) const {
        return origin + t * direction;
    }
};
}

#endif // !RAYTRACING_RAY_H
