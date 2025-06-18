#include "Raycaster.h"

bool Raycaster::IntersectAABB(const glm::vec3& origin, const glm::vec3& dir, const AABB& box, float& tMinOut) {
    float tMin = 0.0f;
    float tMax = 1000.0f;

    for (int i = 0; i < 3; ++i) {
        float invD = 1.0f / dir[i];
        float t0 = (box.min[i] - origin[i]) * invD;
        float t1 = (box.max[i] - origin[i]) * invD;

        if (invD < 0.0f) std::swap(t0, t1);

        tMin = glm::max(tMin, t0);
        tMax = glm::min(tMax, t1);

        if (tMax < tMin)
            return false;
    }

    tMinOut = tMin;
    return true;
}

int Raycaster::Raycast(const glm::vec3& origin, const glm::vec3& direction, const std::vector<AABB>& aabbs, float& hitDistance) {
    float closestHit = std::numeric_limits<float>::max();
    int hitIndex = -1;

    for (size_t i = 0; i < aabbs.size(); ++i) {
        float tMin;
        if (IntersectAABB(origin, direction, aabbs[i], tMin)) {
            if (tMin < closestHit) {
                closestHit = tMin;
                hitIndex = static_cast<int>(i);
            }
        }
    }

    hitDistance = closestHit;
    return hitIndex;
}
