#include "save3dFile.hpp"

void save3DAdvancedfile(const std::vector<Point>& points,
                        const std::vector<Point>& normals,
                        const std::vector<Point2D>& textures,
                        const char* filepath) {
  // Prepend /models/ to the filepath
  std::string path(filepath);
  std::string modelsPath = "./models/";
  std::string newPath = modelsPath + path.substr(path.find_last_of('/') + 1);
  std::cout << "Saving to: " << newPath << std::endl;

  // Open the file for writing
  std::ofstream file(newPath);
  if (!file.is_open()) {
    std::cerr << "Error opening file" << std::endl;
    return;
  }

  file << "# " << points.size() << std::endl;

  for (size_t i = 0; i < points.size(); i++) {
    file << "p " << points[i].x << " " << points[i].y << " " << points[i].z
         << " " << normals[i].x << " " << normals[i].y << " " << normals[i].z
         << " " << textures[i].x << " " << textures[i].y << std::endl;
  }

  file.close();
}