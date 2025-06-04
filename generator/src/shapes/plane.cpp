#include <save3dFile.hpp>
#include <utility>
#include <vector>

#include "utils.hpp"

/**
 * Generates all points, normals, and texture coordinates for a plane
 *
 * @param size The length of each side of the square plane
 * @param divisions Number of subdivisions along each axis
 * @return A pair containing:
 *         - First: pair of vertex points and normals
 *         - Second: texture coordinates
 */
std::pair<std::pair<std::vector<Point>, std::vector<Point>>,
          std::vector<Point2D>>
generatePlaneData(float size, int divisions) {
  const float halfSize = size / 2.0f;
  const float stepSize = size / divisions;

  std::vector<Point> vertices;
  std::vector<Point> normals;
  std::vector<Point2D> texCoords;

  // Generate grid points
  for (int row = 0; row < divisions; row++) {
    for (int col = 0; col < divisions; col++) {
      // Calculate the four corners of the current grid cell
      float x1 = -halfSize + col * stepSize;
      float x2 = x1 + stepSize;
      float z1 = -halfSize + row * stepSize;
      float z2 = z1 + stepSize;

      // Define two triangles that make up this grid cell
      Point bottomLeft(x1, 0.0f, z1);
      Point topLeft(x1, 0.0f, z2);
      Point bottomRight(x2, 0.0f, z1);
      Point topRight(x2, 0.0f, z2);

      // First triangle (bottom-left to top-left to bottom-right)
      vertices.push_back(bottomLeft);
      vertices.push_back(topLeft);
      vertices.push_back(bottomRight);

      // Second triangle (bottom-right to top-left to top-right)
      vertices.push_back(bottomRight);
      vertices.push_back(topLeft);
      vertices.push_back(topRight);

      // Add normals (all pointing up in Y direction)
      for (int i = 0; i < 6; i++) {
        normals.push_back(Point(0.0f, 1.0f, 0.0f));
      }

      // Calculate texture coordinates
      float u1 = static_cast<float>(col) / divisions;
      float u2 = static_cast<float>(col + 1) / divisions;
      float v1 = static_cast<float>(row) / divisions;
      float v2 = static_cast<float>(row + 1) / divisions;

      // Texture coordinates for first triangle
      texCoords.push_back(Point2D(u1, v1));
      texCoords.push_back(Point2D(u1, v2));
      texCoords.push_back(Point2D(u2, v1));

      // Texture coordinates for second triangle
      texCoords.push_back(Point2D(u2, v1));
      texCoords.push_back(Point2D(u1, v2));
      texCoords.push_back(Point2D(u2, v2));
    }
  }

  return {{vertices, normals}, texCoords};
}

/**
 * Generates a plane mesh and saves it to a file
 *
 * @param size Length of each side of the plane
 * @param divisions Number of subdivisions
 * @param outputPath File path to save the plane
 * @param saveAdvanced Whether to save with normals and texture coordinates
 * @return True if successful, false otherwise
 */
bool generatePlaneTriangles(float size, int divisions, const char* outputPath,
                            bool saveAdvanced) {
  auto planeData = generatePlaneData(size, divisions);

  if (saveAdvanced) {
    save3DAdvancedfile(planeData.first.first,   // vertices
                       planeData.first.second,  // normals
                       planeData.second,        // texture coordinates
                       outputPath);
  } else {
    saveToFile(planeData.first.first, outputPath);
  }

  return true;
}