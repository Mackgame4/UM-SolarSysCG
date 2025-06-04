#include "cameraController.hpp"

#include "process_input.hpp"

/**
 * Converts spherical coordinates to cartesian coordinates
 * @param vec Spherical coordinates (rho, theta, phi)
 * @return Cartesian coordinates (x, y, z)
 */
glm::vec3 spherical_to_cartesian(glm::vec3 vec) {
  const float rho = vec.x;    // Radial distance
  const float theta = vec.y;  // Azimuthal angle
  const float phi = vec.z;    // Polar angle

  return glm::vec3{
      std::sin(theta) * std::sin(phi) * rho,  // x
      std::cos(phi) * rho,                    // y
      std::sin(phi) * rho * std::cos(theta)   // z
  };
}

/**
 * Converts cartesian coordinates to spherical coordinates
 * @param vec Cartesian coordinates (x, y, z)
 * @return Spherical coordinates (rho, theta, phi)
 */
glm::vec3 cartesian_to_spherical(glm::vec3 vec) {
  const float x = vec.x;
  const float y = vec.y;
  const float z = vec.z;

  const float radius = std::sqrt(x * x + y * y + z * z);

  // Handle the case when radius is close to zero to avoid division by zero
  if (radius < 1e-6f) {
    return glm::vec3{0.0f, 0.0f, 0.0f};
  }

  return glm::vec3{
      radius,                // rho (radial distance)
      std::atan2(x, z),      // theta (azimuthal angle)
      std::acos(y / radius)  // phi (polar angle)
  };
}

/**
 * Updates camera position and orientation based on user input
 * @param delta_time Time elapsed since last frame in seconds
 * @param imgui Reference to ImGui visibility flag
 */
void CameraController::update(float delta_time, bool& imgui) {
  // Calculate the right vector from the camera's forward and up vectors
  const glm::vec3 right =
      glm::normalize(glm::cross(m_camera.forward, m_camera.up));

  // Update movement speed based on whether shift is pressed
  m_speed = Input::is_down(Keyboard::LShift) ? sprint_speed : normal_speed;

  // Toggle ImGui visibility
  if (Input::is_down(Keyboard::F1)) {
    imgui = true;
  }
  if (Input::is_down(Keyboard::F2)) {
    imgui = false;
  }

  // Update camera position based on WASD keys
  if (Input::is_down(Keyboard::W)) {
    m_camera.position += m_camera.forward * m_speed * delta_time;
  }
  if (Input::is_down(Keyboard::S)) {
    m_camera.position -= m_camera.forward * m_speed * delta_time;
  }
  if (Input::is_down(Keyboard::D)) {
    m_camera.position += right * m_speed * delta_time;
  }
  if (Input::is_down(Keyboard::A)) {
    m_camera.position -= right * m_speed * delta_time;
  }

  // Update camera direction with arrow keys
  if (Input::is_down(Keyboard::Up)) {
    glm::vec3 spherical_forward = cartesian_to_spherical(m_camera.forward);
    spherical_forward.z += delta_time * m_sensitivity;
    // Clamp polar angle to avoid gimbal lock
    spherical_forward.z = glm::clamp(spherical_forward.z, 0.01f, 3.14f);
    m_camera.forward = spherical_to_cartesian(spherical_forward);
  }
  if (Input::is_down(Keyboard::Down)) {
    glm::vec3 spherical_forward = cartesian_to_spherical(m_camera.forward);
    spherical_forward.z -= delta_time * m_sensitivity;
    // Clamp polar angle to avoid gimbal lock
    spherical_forward.z = glm::clamp(spherical_forward.z, 0.01f, 3.14f);
    m_camera.forward = spherical_to_cartesian(spherical_forward);
  }
  if (Input::is_down(Keyboard::Right)) {
    glm::vec3 spherical_forward = cartesian_to_spherical(m_camera.forward);
    spherical_forward.y += delta_time * m_sensitivity;
    m_camera.forward = spherical_to_cartesian(spherical_forward);
  }
  if (Input::is_down(Keyboard::Left)) {
    glm::vec3 spherical_forward = cartesian_to_spherical(m_camera.forward);
    spherical_forward.y -= delta_time * m_sensitivity;
    m_camera.forward = spherical_to_cartesian(spherical_forward);
  }

  // Update camera direction with mouse when right mouse button is pressed
  if (Input::is_down(Mouse::Right)) {
    glm::vec3 spherical_forward = cartesian_to_spherical(m_camera.forward);
    glm::vec2 mouse_delta = Input::mouse_delta() * delta_time * m_sensitivity;

    spherical_forward.y -= mouse_delta.x;  // Horizontal rotation
    spherical_forward.z += mouse_delta.y;  // Vertical rotation

    // Clamp polar angle to avoid gimbal lock
    spherical_forward.z = glm::clamp(spherical_forward.z, 0.01f, 3.14f);

    m_camera.forward = spherical_to_cartesian(spherical_forward);
  }

  // Normalize forward vector to prevent drift
  m_camera.forward = glm::normalize(m_camera.forward);
}