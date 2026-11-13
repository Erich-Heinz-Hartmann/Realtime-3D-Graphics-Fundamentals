#ifndef RAYTRACING_TEXTURE_H
#define RAYTRACING_TEXTURE_H

#include <memory>

#include "utils/tools.h"

namespace Utils {
    class Image;
}

namespace RayTracing {

class Texture {
public:
    virtual ~Texture() = default;
    virtual vecd3 value(double u, double v, const vecd3& p) const = 0;
};

class SolidColor : public Texture {
public:
    SolidColor(const vecd3& albedo) : albedo(albedo) {}
    SolidColor(double red, double green, double blue) : SolidColor(vecd3(red,green,blue)) {}

    vecd3 value(double u, double v, const vecd3& p) const override {
        return albedo;
    }

private:
    vecd3 albedo;
};


class CheckerTexture : public Texture {
public:
    CheckerTexture(double scale, const vecd3& c1, const vecd3& c2) : inv_scale(1.0 / scale), 
        even(std::make_shared<SolidColor>(c1)), odd(std::make_shared<SolidColor>(c2)) { }

    vecd3 value(double u, double v, const vecd3& p) const override {
        auto xInteger = int(std::floor(inv_scale * p.x()));
        auto yInteger = int(std::floor(inv_scale * p.y()));
        auto zInteger = int(std::floor(inv_scale * p.z()));

        bool isEven = (xInteger + yInteger + zInteger) % 2 == 0;

        return isEven ? even->value(u, v, p) : odd->value(u, v, p);
    }

private:
    double inv_scale;
    std::shared_ptr<Texture> even;
    std::shared_ptr<Texture> odd;
};


class ImageTexture : public Texture {
public:
    ImageTexture(const char* filename);

    vecd3 value(double u, double v, const vecd3& p) const override;

private:
    std::unique_ptr<Utils::Image> image;
};


class NoiseTexture : public Texture {
private:
    class Perlin {
    public:
        Perlin() {
            for (int i = 0; i < point_count; i++) {
                randvec[i] = unit_vector(random_vecd3(-1, 1));
            }

            perlin_generate_perm(perm_x);
            perlin_generate_perm(perm_y);
            perlin_generate_perm(perm_z);
        }

        double noise(const vecd3& p) const {
            auto u = p.x() - std::floor(p.x());
            auto v = p.y() - std::floor(p.y());
            auto w = p.z() - std::floor(p.z());

            auto i = int(std::floor(p.x()));
            auto j = int(std::floor(p.y()));
            auto k = int(std::floor(p.z()));
            vecd3 c[2][2][2];

            for (int di=0; di < 2; di++)
                for (int dj=0; dj < 2; dj++)
                    for (int dk=0; dk < 2; dk++)
                        c[di][dj][dk] = randvec[
                            perm_x[(i+di) & 255] ^
                            perm_y[(j+dj) & 255] ^
                            perm_z[(k+dk) & 255]
                        ];

            return perlin_interp(c, u, v, w);
        }

        double turb(const vecd3& p, int depth) const {
            auto accum = 0.0;
            auto temp_p = p;
            auto weight = 1.0;

            for (int i = 0; i < depth; i++) {
                accum += weight * noise(temp_p);
                weight *= 0.5;
                temp_p *= 2.0;
            }

            return std::fabs(accum);
        }

    private:
        static const int point_count = 256;
        vecd3 randvec[point_count];
        int perm_x[point_count];
        int perm_y[point_count];
        int perm_z[point_count];

        static void perlin_generate_perm(int* p) {
            for (int i = 0; i < point_count; i++)
                p[i] = i;

            permute(p, point_count);
        }

        static void permute(int* p, int n) {
            for (int i = n-1; i > 0; i--) {
                int target = random_int(0, i);
                int tmp = p[i];
                p[i] = p[target];
                p[target] = tmp;
            }
        }

        static double perlin_interp(const vecd3 c[2][2][2], double u, double v, double w) {
            auto uu = u*u*(3-2*u);
            auto vv = v*v*(3-2*v);
            auto ww = w*w*(3-2*w);
            auto accum = 0.0;

            for (int i=0; i < 2; i++)
                for (int j=0; j < 2; j++)
                    for (int k=0; k < 2; k++) {
                        vecd3 weight_v(u-i, v-j, w-k);
                        accum += (i*uu + (1-i)*(1-uu))
                               * (j*vv + (1-j)*(1-vv))
                               * (k*ww + (1-k)*(1-ww))
                               * c[i][j][k].dot(weight_v);
                    }

            return accum;
        }
    } noise;

public:
    NoiseTexture(double scale) : scale(scale) {}

    vecd3 value(double u, double v, const vecd3& p) const override {
        return vecd3(.5, .5, .5) * (1.0 + std::sin(scale * p.z() + 10.0 * noise.turb(p, 7)));
    }

private:
    double scale;
};

}

#endif // !RAYTRACING_TEXTURE_H