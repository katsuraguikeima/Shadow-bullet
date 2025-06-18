#include "Bullet.h"

Bullet::Bullet(glm::vec3 pos, glm::vec3 dir)
    : position(pos), direction(glm::normalize(dir))
{
    updateModelMatrix();
}

void Bullet::update(float deltaTime)
{
    position += direction * speed * deltaTime;
    updateModelMatrix();
}

glm::vec3 Bullet::getPosition() const
{
    return position;
}

glm::quat Bullet::getRotation() const
{
    glm::vec3 forward = glm::normalize(direction);
    glm::vec3 up = glm::vec3(0, 1, 0);
    glm::vec3 right = glm::normalize(glm::cross(up, forward));
    up = glm::cross(forward, right);
    glm::mat3 rotMatrix(right, up, forward);
    return glm::quat_cast(rotMatrix);
}

void Bullet::updateModelMatrix()
{
    glm::quat rot = getRotation();
    modelMatrix = glm::translate(glm::mat4(1.0f), position) * glm::toMat4(rot) * glm::scale(glm::mat4(1.0f), glm::vec3(0.2f));
}
