#include "ModelGroup.hpp"

#include "utils.hpp"

ModelGroup::ModelGroup() {
  this->models = {};
  this->subModelgroups = {};
  this->static_transformations = {};
  this->rotations = {};
  this->translates = {};
  this->order = {};
}

ModelGroup::ModelGroup(std::vector<Model> models,
                       std::vector<ModelGroup> subgroups,
                       std::vector<glm::mat4> static_transformations,
                       std::vector<TimeRotations> rotations,
                       std::vector<TimeTranslations> translates,
                       std::vector<Transformations> order) {
  this->models = models;
  this->subModelgroups = subgroups;
  this->static_transformations = static_transformations;
  this->rotations = rotations;
  this->translates = translates;
  this->order = order;
}

glm::mat4 applyTransformations(std::vector<Transformations> order,
                               std::vector<glm::mat4> static_transformations,
                               std::vector<TimeRotations> rotations,
                               std::vector<TimeTranslations> translates,
                               float speed_factor) {
  float elapsed_time = speed_factor * glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
  int t = 0;
  int r = 0;
  int s = 0;

  glm::mat4 matrix = glm::mat4(1.0f);

  for (Transformations type : order) {
    switch (type) {
      case TIMEROTATION:
        matrix *= rotations[r].applyTimeRotation(elapsed_time);
        r++;
        break;
      case TIMETRANSLATE:
        matrix *= translates[t].applyTimeTranslations(elapsed_time);
        t++;
        break;
      case STATIC:
        matrix *= static_transformations[s];
        s++;
        break;
    }
  }

  glMultMatrixf(&matrix[0][0]);
  return matrix;
}

void ModelGroup::drawGroup(bool lights, bool normals, float speed_factor,
                           int& nr_models) {
  glPushMatrix();

  glm::mat4 matrix =
      applyTransformations(this->order, this->static_transformations,
                           this->rotations, this->translates, speed_factor);

  for (Model& model : this->models) {
    if (lights) {
      setupMaterial(model.material);
    }
    nr_models++;
    model.drawModel();
    if (normals) model.drawNormals();
  }

  for (ModelGroup& sub : this->subModelgroups) {
    sub.drawGroup(lights, normals, speed_factor, nr_models);
  }

  glPopMatrix();
}
