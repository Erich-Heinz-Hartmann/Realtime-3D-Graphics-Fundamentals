#include "mesh_simplification.h"
using namespace Eigen;
Vector4f compute_plane(const vecf3& v1, const vecf3& v2, const vecf3& v3) {
    Vector3f e1(v1[0], v1[1], v1[2]);
    Vector3f e2(v2[0], v2[1], v2[2]);
    Vector3f e3(v3[0], v3[1], v3[2]);
    Vector3f edge1 = e2 - e1;
    Vector3f edge2 = e3 - e1;
    Vector3f normal = edge1.cross(edge2);
    normal.normalize();
    float d = -normal.dot(e1);
    return Vector4f(normal(0), normal(1), normal(2), d);
}
Matrix4f compute_Kp(const Vector4f& plane) {
    Matrix4f Kp;
    Kp << plane[0] * plane[0], plane[0] * plane[1], plane[0] * plane[2], plane[0] * plane[3],
        plane[1] * plane[0], plane[1] * plane[1], plane[1] * plane[2], plane[1] * plane[3],
        plane[2] * plane[0], plane[2] * plane[1], plane[2] * plane[2], plane[2] * plane[3],
        plane[3] * plane[0], plane[3] * plane[1], plane[3] * plane[2], plane[3] * plane[3];
    return Kp;
}

std::pair<vecf3, float> compute_optimal_vertex_and_cost(const Matrix4f& Q1, const Matrix4f& Q2,
    const vecf3& v1, const vecf3& v2) {
    Matrix4f Q = Q1 + Q2;
    Matrix4f A = Q;
    A(3, 0) = A(3, 1) = A(3, 2) = 0;
    A(3, 3) = 1;

    vecf3 optimal_pos;
    float min_cost = std::numeric_limits<float>::max();
    if (std::abs(A.determinant()) > EPSILON) {
        Matrix4f A_inv = A.inverse();
        Vector4f v_homo(A_inv(0, 3), A_inv(1, 3), A_inv(2, 3), A_inv(3, 3));
        if (std::abs(v_homo[3]) > EPSILON) {
            optimal_pos = vecf3(v_homo[0] / v_homo[3], v_homo[1] / v_homo[3], v_homo[2] / v_homo[3]);
            Vector4f v_vec(optimal_pos[0], optimal_pos[1], optimal_pos[2], 1.0f);
            min_cost = v_vec.transpose() * Q * v_vec;
        }
    }
    Vector4f v1_vec(v1[0], v1[1], v1[2], 1.0f);
    Vector4f v2_vec(v2[0], v2[1], v2[2], 1.0f);
    vecf3 midpoint = vecf3(
        (v1[0] + v2[0]) * 0.5f,
        (v1[1] + v2[1]) * 0.5f,
        (v1[2] + v2[2]) * 0.5f
    );
    Vector4f mid_vec(midpoint[0], midpoint[1], midpoint[2], 1.0f);

    float cost_v1 = v1_vec.transpose() * Q * v1_vec;
    float cost_v2 = v2_vec.transpose() * Q * v2_vec;
    float cost_mid = mid_vec.transpose() * Q * mid_vec;

    if (cost_v1 < min_cost) {
        min_cost = cost_v1;
        optimal_pos = v1;
    }
    if (cost_v2 < min_cost) {
        min_cost = cost_v2;
        optimal_pos = v2;
    }
    if (cost_mid < min_cost) {
        min_cost = cost_mid;
        optimal_pos = midpoint;
    }

    return std::make_pair(optimal_pos, min_cost);
}

Model* simplify_mesh(
    const std::vector<vecf3>& _vertices,
    const std::vector<veci3>& _faces,
    float ratio
) {

    std::vector<vecf3> vertices = _vertices;
    std::vector<veci3> faces = _faces;

    std::deque<bool> vertices_deleted(vertices.size(), false);

    std::vector<std::vector<int>> faces_of_vertices(vertices.size());
    for (int i = 0; i < faces.size(); ++i) {
        for (int j = 0; j < 3; ++j) {
            faces_of_vertices[faces[i][j]].push_back(i);
        }
    }
    for (int i = 0; i < vertices.size(); ++i) {
        if (faces_of_vertices[i].empty()) {
            vertices_deleted[i] = true;
        }
    }

    std::vector<Matrix4f> Q_matrices(vertices.size(), Matrix4f::Zero());

    for (int i = 0; i < faces.size(); ++i) {
        const veci3& face = faces[i];
        Vector4f plane = compute_plane(vertices[face[0]], vertices[face[1]], vertices[face[2]]);
        Matrix4f Kp = compute_Kp(plane);

        for (int j = 0; j < 3; ++j) {
            Q_matrices[face[j]] += Kp;
        }
    }

    std::priority_queue<Edge, std::vector<Edge>, EdgeComparator> edge_heap;
    std::map<std::pair<int, int>, Edge> edge_map;
    unsigned edge_version = 0;

    for (int i = 0; i < faces.size(); ++i) {
        const veci3& face = faces[i];
        for (int j = 0; j < 3; ++j) {
            int v1 = face[j];
            int v2 = face[(j + 1) % 3];

            if (v1 > v2) std::swap(v1, v2);

            std::pair<int, int> edge_key = std::make_pair(v1, v2);

            if (edge_map.find(edge_key) != edge_map.end()) continue;

            if (vertices_deleted[v1] || vertices_deleted[v2]) continue;

            auto [optimal_pos, cost] = compute_optimal_vertex_and_cost(
                Q_matrices[v1], Q_matrices[v2], vertices[v1], vertices[v2]);

            Edge edge;
            edge.first = v1;
            edge.second = v2;
            edge.cost = cost;
            edge.new_pos = optimal_pos;
            edge.version = ++edge_version;

            edge_map[edge_key] = edge;
            edge_heap.push(edge);
        }
    }

    uint32_t face_cnt = faces.size();
    uint32_t target_face_cnt = static_cast<uint32_t>(face_cnt * ratio);

    while (face_cnt > target_face_cnt && !edge_heap.empty()) {
        Edge min_edge = edge_heap.top();
        edge_heap.pop();

        int v1 = min_edge.first;
        int v2 = min_edge.second;

        if (vertices_deleted[v1] || vertices_deleted[v2]) continue;

        std::pair<int, int> edge_key = std::make_pair(v1, v2);
        auto current_edge = edge_map.find(edge_key);
        if (current_edge == edge_map.end() || current_edge->second.version != min_edge.version) {
            continue;
        }

        // 边可能已经随着退化面一起消失，确认它仍属于至少一个有效面
        bool edge_alive = false;
        for (int face_idx : faces_of_vertices[v1]) {
            if (faces[face_idx][0] == -1) continue;
            const veci3& face = faces[face_idx];
            for (int j = 0; j < 3; ++j) {
                int a = face[j];
                int b = face[(j + 1) % 3];
                if ((a == v1 && b == v2) || (a == v2 && b == v1)) {
                    edge_alive = true;
                    break;
                }
            }
            if (edge_alive) break;
        }
        if (!edge_alive) {
            edge_map.erase(current_edge);
            continue;
        }

        edge_map.erase(current_edge);

        vertices[v1] = min_edge.new_pos;
        Q_matrices[v1] = Q_matrices[v1] + Q_matrices[v2];
        vertices_deleted[v2] = true;

        std::vector<int> faces_to_update = faces_of_vertices[v2];
        for (int face_idx : faces_to_update) {
            if (faces[face_idx][0] == -1) continue;
            for (int j = 0; j < 3; ++j) {
                if (faces[face_idx][j] == v2) {
                    faces[face_idx][j] = v1;
                }
            }
            if (std::find(faces_of_vertices[v1].begin(), faces_of_vertices[v1].end(), face_idx) == faces_of_vertices[v1].end()) {
                faces_of_vertices[v1].push_back(face_idx);
            }
        }
        faces_of_vertices[v2].clear();
        std::vector<int> updated_faces_v1;
        for (int face_idx : faces_of_vertices[v1]) {
            if (faces[face_idx][0] == -1) continue;

            veci3& face = faces[face_idx];
            if (face[0] == face[1] || face[1] == face[2] || face[2] == face[0]) {
                face = veci3(-1, -1, -1);
                face_cnt--;
            }
            else {
                updated_faces_v1.push_back(face_idx);
            }
        }
        faces_of_vertices[v1] = updated_faces_v1;
        for (int face_idx : faces_of_vertices[v1]) {
            const veci3& face = faces[face_idx];
            for (int j = 0; j < 3; ++j) {
                int neighbor = face[j];
                if (neighbor == v1 || vertices_deleted[neighbor]) continue;

                int edge_v1 = std::min(v1, neighbor);
                int edge_v2 = std::max(v1, neighbor);
                std::pair<int, int> new_edge_key = std::make_pair(edge_v1, edge_v2);

                // Compute new cost
                auto [optimal_pos, cost] = compute_optimal_vertex_and_cost(
                    Q_matrices[edge_v1], Q_matrices[edge_v2],
                    vertices[edge_v1], vertices[edge_v2]);

                Edge new_edge;
                new_edge.first = edge_v1;
                new_edge.second = edge_v2;
                new_edge.cost = cost;
                new_edge.new_pos = optimal_pos;
                new_edge.version = ++edge_version;

                edge_map[new_edge_key] = new_edge;
                edge_heap.push(new_edge);
            }
        }
    }
    int new_vert_cnt = 0;
    int new_face_cnt = 0;
    std::vector<int> vertex_mapping(vertices.size(), -1);
    for (auto i = 0; i < vertices.size(); ++i) {
        if (!vertices_deleted[i]) {
            vertices[new_vert_cnt] = vertices[i];
            vertex_mapping[i] = new_vert_cnt;
            new_vert_cnt += 1;
        }
    }
    for (int i = 0; i < faces.size(); ++i) {
        if (faces[i][0] != -1) {
            int v0 = vertex_mapping[faces[i][0]];
            int v1 = vertex_mapping[faces[i][1]];
            int v2 = vertex_mapping[faces[i][2]];
            if (v0 != v1 && v1 != v2 && v2 != v0) {
                faces[new_face_cnt] = veci3(v0, v1, v2);
                new_face_cnt += 1;
            }
        }
    }

    vertices.resize(new_vert_cnt);
    faces.resize(new_face_cnt);

    return Model::load(vertices, faces);
}