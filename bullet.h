#ifndef BULLET_CLASS_H
#define BULLET_CLASS_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

class Bullet
{
public:
    Bullet(glm::vec3 pos, glm::vec3 dir);

    void update(float deltaTime);

    glm::vec3 getPosition() const;
    glm::quat getRotation() const;

    glm::vec3 position;
    glm::vec3 direction;
    glm::mat4 modelMatrix;
    float speed = 20.0f;

private:
    void updateModelMatrix();
};

#endif
