#include "transform.h"

namespace Utils::Transform {

matf4 perspective(float fov_y, float aspect, float z_near, float z_far) noexcept {
    assert(fov_y > 0 && aspect > 0 && z_near >= 0 && z_far > z_near);

	// TODO 2.1.2 : Create the perspective projection matrix.
    matf4 m = matf4::Identity();
    float tan_half_fov_y = tan(fov_y / 2.0f);

    m(0, 0) = 1.0f / (aspect * tan_half_fov_y);
    m(1, 1) = 1.0f / tan_half_fov_y;
    m(2, 2) = -(z_far + z_near) / (z_far - z_near);
    m(2, 3) = -(2.0f * z_far * z_near) / (z_far - z_near);
    m(3, 2) = -1.0f;
    m(3, 3) = 0.0f;
    return m;
}

matf4 orthographic(float width, float height, float z_near, float z_far) noexcept {
    assert(width > 0 && height > 0 && z_near >= 0 && z_far > z_near);

	// TODO 2.1.2 : Create the orthographic projection matrix.
	matf4 m = matf4::Identity();
    m(0, 0) = 2.0f / width;
    m(1, 1) = 2.0f / height;
    m(2, 2) = -2.0f / (z_far - z_near);
    m(2, 3) = -(z_far + z_near) / (z_far - z_near);
    return m;
}

matf4 look_at(const vecf3& pos, const vecf3& target, const vecf3& up) noexcept {
    assert(abs(up.dot(up) - 1.0f) < 1e-6);

    // TODO 2.1.2 : Create the lookat view matrix for the camera.
	matf4 m = matf4::Identity();
    vecf3 f = (target - pos).normalized();
    vecf3 r = f.cross(up).normalized();
    vecf3 u = r.cross(f);

    m(0, 0) = r.x(); m(0, 1) = r.y(); m(0, 2) = r.z();
    m(1, 0) = u.x(); m(1, 1) = u.y(); m(1, 2) = u.z();
    m(2, 0) = -f.x(); m(2, 1) = -f.y(); m(2, 2) = -f.z();

    m(0, 3) = -r.dot(pos);
    m(1, 3) = -u.dot(pos);
    m(2, 3) = f.dot(pos);
    return m;
}

matf4 get_scale_matrix(const vecf3& scale) noexcept {
    // TODO 2.1.1 : Implement the function that returns a scale matrix.
    matf4 m = matf4::Identity();
    m(0, 0) = scale.x();
    m(1, 1) = scale.y();
    m(2, 2) = scale.z();
    return m;
}

matf4 get_trans_matrix(const vecf3& trans) noexcept {
    // TODO 2.1.1 : Implement the function that returns a translation matrix.
    matf4 m = matf4::Identity();
    m(0, 3) = trans.x();
    m(1, 3) = trans.y();
    m(2, 3) = trans.z();
    return m;
}

matf4 rotate_with(float theta, const vecf3& axis) noexcept {
    assert(abs(axis.dot(axis) - 1.0f) < 1e-6);

	// TODO 2.1.1 : Create the rotation matrix with the given theta and axis.
    // Note that the parameter 'theta' is expressed in the radian system.
	matf4 m = matf4::Identity();
    float cos_theta = cos(theta);
    float sin_theta = sin(theta);
    float one_minus_cos = 1.0f - cos_theta;

    m(0, 0) = cos_theta + axis.x() * axis.x() * one_minus_cos;
    m(0, 1) = axis.x() * axis.y() * one_minus_cos - axis.z() * sin_theta;
    m(0, 2) = axis.x() * axis.z() * one_minus_cos + axis.y() * sin_theta;

    m(1, 0) = axis.y() * axis.x() * one_minus_cos + axis.z() * sin_theta;
    m(1, 1) = cos_theta + axis.y() * axis.y() * one_minus_cos;
    m(1, 2) = axis.y() * axis.z() * one_minus_cos - axis.x() * sin_theta;

    m(2, 0) = axis.z() * axis.x() * one_minus_cos - axis.y() * sin_theta;
    m(2, 1) = axis.z() * axis.y() * one_minus_cos + axis.x() * sin_theta;
    m(2, 2) = cos_theta + axis.z() * axis.z() * one_minus_cos;
    return m;
}

matf4 generate_model_matrix(const vecf3& pos, const vecf3& scale, float theta, const vecf3& axis) noexcept {
	// TODO 2.1.1 : Implement the function that returns a model matrix.
	matf4 m = matf4::Identity();
    matf4 trans_mat = get_trans_matrix(pos);
    matf4 rot_mat = rotate_with(theta, axis);
    matf4 scale_mat = get_scale_matrix(scale);

    m = trans_mat * rot_mat * scale_mat;
	return m;
}

matf4 generate_model_matrix(const vecf3& pos, const vecf3& scale, const matf4& rotate) noexcept {
    matf4 m = get_trans_matrix(pos) * get_scale_matrix(scale) * rotate;

    return m;
}

} // namespace Utils::Transform