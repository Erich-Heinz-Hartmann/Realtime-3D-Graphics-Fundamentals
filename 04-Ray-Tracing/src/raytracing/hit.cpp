#include "raytracing/hit.h"

namespace RayTracing {


bool HittableList::hit(const Ray& r, Interval ray_t, Record& rec) const {
    Record temp_rec;
    bool hit_anything = false;
    auto closest_so_far = ray_t.max;

    for (const auto& object : objects) {
        if (object->hit(r, Interval(ray_t.min, closest_so_far), temp_rec)) {
            hit_anything = true;
            closest_so_far = temp_rec.t;
            rec = temp_rec;
        }
    }

    return hit_anything;
}


bool Sphere::hit(const Ray& r, Interval ray_t, Record& rec) const {
    // TODO: 4.3 Sphere & ray intersection
    //return false;
    vecd3 current_center = center.at(r.time);
    vecd3 oc = current_center - r.origin;
    auto a = r.direction.squaredNorm();
    auto h = r.direction.dot(oc);
    auto c = oc.squaredNorm() - radius * radius;

    auto discriminant = h * h - a * c;

    if (discriminant < 0) {
        return false;
    }

    auto sqrtd = std::sqrt(discriminant);

    auto root = (h - sqrtd) / a;
    if (!ray_t.surrounds(root)) {
        root = (h + sqrtd) / a;
        if (!ray_t.surrounds(root)) {
            return false;
        }
    }

    rec.t = root;
    rec.p = r.at(rec.t);

    vecd3 outward_normal = (rec.p - current_center) / radius;
    rec.set_face_normal(r, outward_normal);

    double u, v;
    get_sphere_uv(outward_normal, u, v);
    rec.u = u;
    rec.v = v;

    rec.mat = mat;

    return true;
}


bool Quad::hit(const Ray& r, Interval ray_t, Record& rec) const {
    // TODO: 4.3 Quad & ray intersection
    //return false;
    double denom = normal.dot(r.direction);

    if (std::fabs(denom) < 1e-8) {
        return false;
    }

    double t = (D - normal.dot(r.origin)) / denom;

    if (!ray_t.surrounds(t)) {
        return false;
    }

    vecd3 intersection = r.at(t);

    vecd3 p = intersection - Q;
    double alpha = w.dot(p.cross(v));
    double beta = w.dot(u.cross(p));

    if (!is_interior(alpha, beta, rec)) {
        return false;
    }

    rec.t = t;  
    rec.p = intersection;
    rec.mat = mat;
    rec.set_face_normal(r, normal);

    return true;
}

// Returns the 3D box (six sides) that contains the two opposite vertices a & b.
std::shared_ptr<HittableList> box(const vecd3& a, const vecd3& b, std::shared_ptr<Material> mat) {
    auto sides = std::make_shared<HittableList>();

    // TODO: Use the Quad class to create the six sides of the box.
    // `a` and `b` are the two opposite vertices of the box.

    //return sides;

    double min_x = std::min(a.x(), b.x());
    double min_y = std::min(a.y(), b.y());
    double min_z = std::min(a.z(), b.z());
    double max_x = std::max(a.x(), b.x());
    double max_y = std::max(a.y(), b.y());
    double max_z = std::max(a.z(), b.z());

    vecd3 min_point(min_x, min_y, min_z);
    vecd3 max_point(max_x, max_y, max_z);

    sides->add(std::make_shared<Quad>(
        vecd3(min_x, min_y, min_z),
        vecd3(max_x - min_x, 0, 0),
        vecd3(0, 0, max_z - min_z),
        mat));

    sides->add(std::make_shared<Quad>(
        vecd3(min_x, max_y, max_z),
        vecd3(max_x - min_x, 0, 0),
        vecd3(0, 0, min_z - max_z),
        mat));

    sides->add(std::make_shared<Quad>(
        vecd3(min_x, min_y, max_z),
        vecd3(max_x - min_x, 0, 0),
        vecd3(0, max_y - min_y, 0),
        mat));

    sides->add(std::make_shared<Quad>(
        vecd3(max_x, min_y, min_z),
        vecd3(min_x - max_x, 0, 0),
        vecd3(0, max_y - min_y, 0),
        mat));

    sides->add(std::make_shared<Quad>(
        vecd3(min_x, min_y, max_z),
        vecd3(0, 0, min_z - max_z),
        vecd3(0, max_y - min_y, 0),
        mat));

    sides->add(std::make_shared<Quad>(
        vecd3(max_x, min_y, min_z),
        vecd3(0, 0, max_z - min_z),
        vecd3(0, max_y - min_y, 0),
        mat));

    return sides;
}


bool ConstantMedium::hit(const Ray& r, Interval ray_t, Record& rec) const {
    // TODO: 4.3 Constant Medium & ray intersection
    //return false;
    Record rec1, rec2;

    if (!boundary->hit(r, Interval(-infinity, infinity), rec1)) {
        return false;
    }

    if (!boundary->hit(r, Interval(rec1.t + 0.0001, infinity), rec2)) {
        return false;
    }

    if (rec1.t < ray_t.min) rec1.t = ray_t.min;
    if (rec2.t > ray_t.max) rec2.t = ray_t.max;

    if (rec1.t >= rec2.t) {
        return false;
    }

    if (rec1.t < 0) rec1.t = 0;

    double ray_length = r.direction.norm();
    double distance_inside_boundary = (rec2.t - rec1.t) * ray_length;
    double hit_distance = neg_inv_density * std::log(random_double());

    if (hit_distance > distance_inside_boundary) {
        return false;
    }

    rec.t = rec1.t + hit_distance / ray_length;
    rec.p = r.at(rec.t);

    rec.normal = vecd3(1, 0, 0);
    rec.front_face = true;
    rec.mat = phase_function;

    return true;
}


bool Translate::hit(const Ray& r, Interval ray_t, Record& rec) const {
    // Move the ray backwards by the offset
    Ray offset_r(r.origin - offset, r.direction, r.time);

    // Determine whether an intersection exists along the offset ray (and if so, where)
    if (!object->hit(offset_r, ray_t, rec)) {
        return false;
    }

    // Move the intersection point forwards by the offset
    rec.p += offset;

    return true;
}


bool RotateY::hit(const Ray& r, Interval ray_t, Record& rec) const {
    // Transform the ray from world space to object space.
    auto origin = vecd3(
        (cos_theta * r.origin.x()) - (sin_theta * r.origin.z()),
        r.origin.y(),
        (sin_theta * r.origin.x()) + (cos_theta * r.origin.z())
    );

    auto direction = vecd3(
        (cos_theta * r.direction.x()) - (sin_theta * r.direction.z()),
        r.direction.y(),
        (sin_theta * r.direction.x()) + (cos_theta * r.direction.z())
    );

    Ray rotated_r(origin, direction, r.time);

    // Determine whether an intersection exists in object space (and if so, where).
    if (!object->hit(rotated_r, ray_t, rec))
        return false;

    // Transform the intersection from object space back to world space.
    rec.p = vecd3(
        (cos_theta * rec.p.x()) + (sin_theta * rec.p.z()),
        rec.p.y(),
        (-sin_theta * rec.p.x()) + (cos_theta * rec.p.z())
    );

    rec.normal = vecd3(
        (cos_theta * rec.normal.x()) + (sin_theta * rec.normal.z()),
        rec.normal.y(),
        (-sin_theta * rec.normal.x()) + (cos_theta * rec.normal.z())
    );

    return true;
}


bool BVHNode::hit(const Ray& r, Interval ray_t, Record& rec) const {
    // TODO: 4.4 Using BVH to accelerate ray tracing
    //return false;
    if (!bbox.hit(r, ray_t)) {
        return false;
    }

    bool hit_left = left->hit(r, ray_t, rec);

    Interval right_ray_t = hit_left ? Interval(ray_t.min, rec.t) : ray_t;
    bool hit_right = right->hit(r, right_ray_t, rec);

    return hit_left || hit_right;
}

}