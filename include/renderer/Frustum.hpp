#pragma once

#include <array>
#include <glm/glm.hpp>

struct Plane {
    glm::vec3 normal {0.0f, 0.0f, 0.0f};
    float distance = 0.0f;

    void normalize() {
        const float len = glm::length(normal);
        if (len > 0.0f) {
            normal /= len;
            distance /= len;
        }
    }

    float signedDistanceTo(const glm::vec3& p) const {
        return glm::dot(normal, p) + distance;
    }
};

struct AABB {
    glm::vec3 min {0.0f, 0.0f, 0.0f};
    glm::vec3 max {0.0f, 0.0f, 0.0f};
};

class Frustum {
public:
    enum Side : int {
        Left = 0,
        Right,
        Bottom,
        Top,
        Near,
        Far
    };

    static Frustum fromViewProjection(const glm::mat4& vp) {
        Frustum f;

        // GLM matrices are column-major.
        // rowN = (vp[0][N], vp[1][N], vp[2][N], vp[3][N])
        const glm::vec4 row0(vp[0][0], vp[1][0], vp[2][0], vp[3][0]);
        const glm::vec4 row1(vp[0][1], vp[1][1], vp[2][1], vp[3][1]);
        const glm::vec4 row2(vp[0][2], vp[1][2], vp[2][2], vp[3][2]);
        const glm::vec4 row3(vp[0][3], vp[1][3], vp[2][3], vp[3][3]);

        f.planes[Left]   = makePlane(row3 + row0);
        f.planes[Right]  = makePlane(row3 - row0);
        f.planes[Bottom] = makePlane(row3 + row1);
        f.planes[Top]    = makePlane(row3 - row1);
        f.planes[Near]   = makePlane(row3 + row2);
        f.planes[Far]    = makePlane(row3 - row2);

        return f;
    }

    bool intersects(const AABB& box) const {
        for (const Plane& plane : planes) {
            glm::vec3 positive = box.min;

            if (plane.normal.x >= 0.0f) positive.x = box.max.x;
            if (plane.normal.y >= 0.0f) positive.y = box.max.y;
            if (plane.normal.z >= 0.0f) positive.z = box.max.z;

            if (plane.signedDistanceTo(positive) < 0.0f) {
                return false;
            }
        }
        return true;
    }

private:
    std::array<Plane, 6> planes {};

    static Plane makePlane(const glm::vec4& v) {
        Plane p;
        p.normal = glm::vec3(v.x, v.y, v.z);
        p.distance = v.w;
        p.normalize();
        return p;
    }
};