#ifndef GROUP_HPP
#define GROUP_HPP

#include <array>  // Include for std::array
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <vector>

#include "Model.hpp"
#include "catmullCurves.hpp"
#include "utils.hpp"

class ModelGroup {
 public:
  std::vector<Model> models;
  std::vector<ModelGroup> subModelgroups;
  std::vector<glm::mat4> static_transformations;

  std::vector<TimeRotations> rotations;
  std::vector<TimeTranslations> translates;
  std::vector<Transformations> order;

  ModelGroup();
  ModelGroup(std::vector<Model> models, std::vector<ModelGroup> subgroups,
             std::vector<glm::mat4> static_transformations,
             std::vector<TimeRotations> rotations,
             std::vector<TimeTranslations> translates,
             std::vector<Transformations> order);  // Updated constructor

  void translate(float x, float y, float z);

  void scale(float x, float y, float z);

  void rotate(float angle, float x, float y, float z);

  void drawGroup(bool lights, bool normals, float elapsed_time, int& nr_models);
};

#endif  // GROUP_HPP
