#include "Camera.hpp"

#include <sstream>
#include <string>

Camera::Camera() {
  this->position = glm::vec3();
  this->lookAt = glm::vec3();
  this->up = glm::vec3();
  this->fov = 0;
  this->nearPlane = 0;
  this->farPlane = 0;
}

Camera::Camera(glm::vec3 position, glm::vec3 lookAt, glm::vec3 up, int fov,
               float near, float far) {
  this->position = position;
  this->lookAt = lookAt;
  this->up = up;

  glm::vec3 direction = glm::normalize(position - lookAt);
  this->right = glm::normalize(glm::cross(up, direction));
  this->real_up = glm::normalize(glm::cross(direction, right));
  this->forward = glm::normalize(lookAt - position);

  this->fov = fov;
  this->nearPlane = near;
  this->farPlane = far;
}

Camera::Camera(const Camera& other) {
  this->position = other.position;
  this->lookAt = other.lookAt;
  this->up = other.up;
  this->fov = other.fov;
  this->nearPlane = other.nearPlane;
  this->farPlane = other.farPlane;
}