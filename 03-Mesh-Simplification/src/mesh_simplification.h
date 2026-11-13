#ifndef MESH_SIMPLIFICATION_H
#define MESH_SIMPLIFICATION_H

#include <vector>
#include <utility>
#include <algorithm>
#include <iostream>
#include <queue>
#include <map>
#include <set>
#include <tuple>
#include <functional>
#include <cassert>

#include "Eigen/Dense"

#include "utils/tools.h"
#include "utils/model.h"

#define EPSILON 1e-15

using Utils::Model;

Model* simplify_mesh(const std::vector<vecf3>& _vertices, const std::vector<veci3>& _faces, float ratio);

struct Edge {
    int first, second;
    float cost;
    vecf3 new_pos;
    unsigned version = 0;

    bool operator==(const Edge& other) const {
        return first == other.first && second == other.second && abs(cost - other.cost) < EPSILON;
    }
    bool operator<(const Edge& other) const {
        if (abs(cost - other.cost) < EPSILON) {
            if (first == other.first) {
                return second < other.second;
            }
            else {
                return first < other.first;
            }
        }
        else {
            return cost < other.cost;
        }
    }
};

struct EdgeComparator {
    bool operator()(const Edge& a, const Edge& b) const {
        return a.cost > b.cost;
    }
};

#endif // MESH_SIMPLIFICATION_H