#ifndef CONFIGURATION_HPP
#define CONFIGURATION_HPP

#include <string>
#include <vector>

#include "Camera.hpp"
#include "Configuration.hpp"
#include "ModelGroup.hpp"
#include "Window.hpp"

class Configuration {
 public:
  Window window;
  Camera camera;
  ModelGroup modelGroup;
  std::vector<Light> lights;

  Configuration() = default;
  Configuration(const Window &window, const Camera &camera,
                const ModelGroup &group);
  Configuration(const Window &window, const Camera &camera,
                const ModelGroup &group, const std::vector<Light> &lights);

  bool addLight(const Light &light);
  bool removeLight(int index);
  // std::string toString();
};

#endif  // CONFIGURATION_HPP