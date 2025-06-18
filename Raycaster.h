#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <limits>

struct AABB {
    glm::vec3 min;
    glm::vec3 max;
};

class Raycaster {
public:
    static int Raycast(
        const glm::vec3& origin,
        const glm::vec3& direction,
        const std::vector<AABB>& aabbs,
        float& hitDistance
    );

private:
    static bool IntersectAABB(
        const glm::vec3& origin,
        const glm::vec3& dir,
        const AABB& box,
        float& tMin
    );
};
