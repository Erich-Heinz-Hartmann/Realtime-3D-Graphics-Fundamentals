#include "raytracing/texture.h"
#include "utils/image.h"

namespace RayTracing {

ImageTexture::ImageTexture(const char* filename) {
    image = std::make_unique<Utils::Image>(filename);
}

vecd3 ImageTexture::value(double u, double v, const vecd3& p) const {
    return image->sample(u, v);
}

}