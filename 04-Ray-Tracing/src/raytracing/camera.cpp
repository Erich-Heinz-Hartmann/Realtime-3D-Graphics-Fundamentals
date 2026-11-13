#include "raytracing/camera.h"
#include "raytracing/hit.h"
#include "raytracing/material.h"
#include "raytracing/record.h"

namespace RayTracing {

vecd3 Camera::ray_color(const Ray& r, int depth, const Hittable& world) const {
    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (depth <= 0)
        return vecd3(0, 0, 0);

    Record rec;

    if (!world.hit(r, Interval(0.001, infinity), rec)) {
        return background;
    }

    // TODO: 4.2 Get the color from scattered or emitted rays
    //return background;
    vecd3 emitted_color = rec.mat->emitted(rec.u, rec.v, rec.p);

    Ray scattered;
    vecd3 attenuation;

    if (!rec.mat->scatter(r, rec, attenuation, scattered)) {
        return emitted_color;
    }

    vecd3 scattered_color = attenuation.cwiseProduct(ray_color(scattered, depth - 1, world));

    return emitted_color + scattered_color;
}

}