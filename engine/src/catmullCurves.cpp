#include <stdlib.h>
#define _USE_MATH_DEFINES
#include <cmath>
#include <utility>
#include <vector>

#include "catmullCurves.hpp"
#include "utils.hpp"

static const std::array<std::array<float, 4>, 4> catmoll_rom_matrix{{
    {-0.5f, +1.5f, -1.5f, +0.5f},
    {+1.0f, -2.5f, +2.0f, -0.5f},
    {-0.5f, +0.0f, +0.5f, +0.0f},
    {+0.0f, +1.0f, +0.0f, +0.0f},
}};

std::pair<Point, Point> catmollRomPosition(std::vector<Point> curve,
                                           float global_time) {
  const auto basis = catmoll_rom_matrix;

  float scaled_t = global_time * curve.size();
  int segment_idx = static_cast<int>(std::floor(scaled_t));
  float local_t = scaled_t - segment_idx;

  int idx = segment_idx + curve.size() - 1;
  Point pt0 = curve[(idx + 0) % curve.size()];
  Point pt1 = curve[(idx + 1) % curve.size()];
  Point pt2 = curve[(idx + 2) % curve.size()];
  Point pt3 = curve[(idx + 3) % curve.size()];

  std::array<std::array<float, 4>, 3> geometry{{
      {pt0.x, pt1.x, pt2.x, pt3.x},
      {pt0.y, pt1.y, pt2.y, pt3.y},
      {pt0.z, pt1.z, pt2.z, pt3.z},
  }};

  std::array<float, 4> timeVector = {local_t * local_t * local_t,
                                     local_t * local_t, local_t, 1.0f};
  std::array<float, 4> timeDerivative = {3 * local_t * local_t, 2 * local_t,
                                         1.0f, 0.0f};

  std::array<float, 3> pos{};
  std::array<float, 3> deriv{};

  for (size_t axis = 0; axis < 3; ++axis) {
    std::array<float, 4> coeffs{};

    for (size_t row = 0; row < 4; ++row) {
      for (size_t col = 0; col < 4; ++col) {
        coeffs[row] += geometry[axis][col] * basis[row][col];
      }
    }

    for (size_t k = 0; k < 4; ++k) {
      pos[axis] += timeVector[k] * coeffs[k];
      deriv[axis] += timeDerivative[k] * coeffs[k];
    }
  }

  return {
      Point(pos[0], pos[1], pos[2]),
      Point(deriv[0], deriv[1], deriv[2]),
  };
}

TimeRotations::TimeRotations() {
  this->time = 0;
  this->x = 0;
  this->y = 0;
  this->z = 0;
}

TimeRotations::TimeRotations(float time, float x, float y, float z) {
  this->time = time;
  this->x = x;
  this->y = y;
  this->z = z;
}

glm::mat4 TimeRotations::applyTimeRotation(float elapsed_time) {
  if (this->time == 0) {
    return glm::mat4(1.0f);
  }
  float angle = 360.0f * (elapsed_time / this->time);
  return glm::rotate(glm::mat4(1.0f), glm::radians(angle),
                     glm::vec3(this->x, this->y, this->z));
}

TimeTranslations::TimeTranslations() {
  this->time = 0;
  this->align = false;
  this->y_axis = Point(0, 1, 0);
}

TimeTranslations::TimeTranslations(float time, bool align,
                                   std::vector<Point> curve) {
  this->time = time;
  this->align = align;
  this->curvePoints = curve;
  this->y_axis = Point(0, 1, 0);
}

std::pair<Point, Point> TimeTranslations::getLocation(float elapsed_time) {
  int total_points = this->curvePoints.size();
  float global_t = elapsed_time / this->time;
  float scaled_t = global_t * total_points;
  int idx = std::floor(scaled_t);
  float local_t = scaled_t - idx;
  return catmollRomPosition(this->curvePoints, local_t);
}

std::array<float, 16> TimeTranslations::rotationMatrix(Point x_axis,
                                                       Point y_axis,
                                                       Point z_axis) {
  return std::array<float, 16>{{
      x_axis.x,
      x_axis.y,
      x_axis.z,
      0,
      y_axis.x,
      y_axis.y,
      y_axis.z,
      0,
      z_axis.x,
      z_axis.y,
      z_axis.z,
      0,
      0,
      0,
      0,
      1,
  }};
}

glm::mat4 TimeTranslations::applyTimeTranslations(float elapsed_time) {
  if (this->time == 0) {
    return glm::mat4(1.0f);
  }

  float time_ratio = elapsed_time / this->time;
  auto [pos, dir] = catmollRomPosition(this->curvePoints, time_ratio);

  glm::mat4 transform = glm::mat4(1.0f);
  transform = glm::translate(transform, glm::vec3(pos.x, pos.y, pos.z));

  if (this->align) {
    Point forward = dir.normalize();
    Point right = forward.cross(this->y_axis).normalize();
    Point up = right.cross(forward).normalize();
    auto rot_array = this->rotationMatrix(forward, up, right);
    glm::mat4 rot_matrix = glm::make_mat4(rot_array.data());
    transform *= rot_matrix;
  }

  return transform;
}

void TimeTranslations::renderCatmullRomCurve() {
  float global_t = 0.0f;
  const int steps = 100;
  float step_size = this->time / steps;

  glBegin(GL_LINE_LOOP);
  for (int i = 0; i < steps; ++i) {
    auto [pos, _] = this->getLocation(global_t);
    glVertex3f(pos.x, pos.y, pos.z);
    global_t += step_size;
  }
  glEnd();
}

glm::mat4 Scalematrix(float x, float y, float z) {
  glm::mat4 mat = glm::mat4(1.0f);
  return glm::scale(mat, glm::vec3(x, y, z));
}

glm::mat4 Translatematrix(float x, float y, float z) {
  glm::mat4 mat = glm::mat4(1.0f);
  return glm::translate(mat, glm::vec3(x, y, z));
}

glm::mat4 Rotationmatrix(float angle, float x, float y, float z) {
  glm::mat4 mat = glm::mat4(1.0f);
  return glm::rotate(mat, glm::radians(angle), glm::vec3(x, y, z));
}
