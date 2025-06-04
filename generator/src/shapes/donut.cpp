#include <cmath>
#include <fstream>
#include <iostream>
#include <save3dFile.hpp>
#include <vector>

#include "utils.hpp"

/**
 * @brief Generates a torus/donut shape divided into triangles
 *
 * This function creates a torus (donut shape) with the specified parameters.
 * The torus is defined by a major radius (distance from center of tube to
 * center of torus) and a minor radius (the radius of the tube itself).
 *
 * @param majorRadius Distance from center of torus to center of tube
 * @param minorRadius Radius of the tube itself
 * @param sides Number of divisions around the tube (minor circle)
 * @param rings Number of divisions around the torus (major circle)
 * @return A pair containing: pair(vertices, normals) and texture coordinates
 */
std::pair<std::pair<std::vector<Point>, std::vector<Point>>,
          std::vector<Point2D>>
donutAllPoints(float majorRadius, float minorRadius, int sides, int rings) {
  std::vector<Point> vertexArray;   // Store all vertices
  std::vector<Point> normalArray;   // Store all normal vectors
  std::vector<Point2D> textureMap;  // Store all texture coordinates

  // Loop through all rings around the torus (major circle)
  for (int ringIdx = 0; ringIdx < rings; ++ringIdx) {
    // Calculate angles for this ring and the next
    float outerAngle1 = static_cast<float>(ringIdx) * 2.0f *
                        static_cast<float>(M_PI) / static_cast<float>(rings);
    float outerAngle2 = static_cast<float>(ringIdx + 1) * 2.0f *
                        static_cast<float>(M_PI) / static_cast<float>(rings);

    // Loop through all sides around the tube (minor circle)
    for (int tubeIdx = 0; tubeIdx < sides; ++tubeIdx) {
      // Calculate angles for this side and the next
      float innerAngle1 = static_cast<float>(tubeIdx) * 2.0f *
                          static_cast<float>(M_PI) / static_cast<float>(sides);
      float innerAngle2 = static_cast<float>(tubeIdx + 1) * 2.0f *
                          static_cast<float>(M_PI) / static_cast<float>(sides);

      // Calculate the four vertices of the current quad
      // Vertex 1: (ringIdx, tubeIdx)
      float vx1 = (majorRadius + minorRadius * std::cos(innerAngle1)) *
                  std::cos(outerAngle1);
      float vy1 = minorRadius * std::sin(innerAngle1);
      float vz1 = (majorRadius + minorRadius * std::cos(innerAngle1)) *
                  std::sin(outerAngle1);

      // Vertex 2: (ringIdx, tubeIdx+1)
      float vx2 = (majorRadius + minorRadius * std::cos(innerAngle2)) *
                  std::cos(outerAngle1);
      float vy2 = minorRadius * std::sin(innerAngle2);
      float vz2 = (majorRadius + minorRadius * std::cos(innerAngle2)) *
                  std::sin(outerAngle1);

      // Vertex 3: (ringIdx+1, tubeIdx)
      float vx3 = (majorRadius + minorRadius * std::cos(innerAngle1)) *
                  std::cos(outerAngle2);
      float vy3 = minorRadius * std::sin(innerAngle1);
      float vz3 = (majorRadius + minorRadius * std::cos(innerAngle1)) *
                  std::sin(outerAngle2);

      // Vertex 4: (ringIdx+1, tubeIdx+1)
      float vx4 = (majorRadius + minorRadius * std::cos(innerAngle2)) *
                  std::cos(outerAngle2);
      float vy4 = minorRadius * std::sin(innerAngle2);
      float vz4 = (majorRadius + minorRadius * std::cos(innerAngle2)) *
                  std::sin(outerAngle2);

      // Create the first triangle: 1-2-4
      vertexArray.push_back(Point(vx1, vy1, vz1));
      vertexArray.push_back(Point(vx2, vy2, vz2));
      vertexArray.push_back(Point(vx4, vy4, vz4));

      // Create the second triangle: 1-4-3
      vertexArray.push_back(Point(vx1, vy1, vz1));
      vertexArray.push_back(Point(vx4, vy4, vz4));
      vertexArray.push_back(Point(vx3, vy3, vz3));

      // Calculate normals (pointing outward from the torus tube)
      // These are calculated by subtracting the ring center from each vertex
      Point norm1 = Point(vx1 - majorRadius * std::cos(outerAngle1), vy1,
                          vz1 - majorRadius * std::sin(outerAngle1));

      Point norm2 = Point(vx2 - majorRadius * std::cos(outerAngle1), vy2,
                          vz2 - majorRadius * std::sin(outerAngle1));

      Point norm3 = Point(vx3 - majorRadius * std::cos(outerAngle2), vy3,
                          vz3 - majorRadius * std::sin(outerAngle2));

      Point norm4 = Point(vx4 - majorRadius * std::cos(outerAngle2), vy4,
                          vz4 - majorRadius * std::sin(outerAngle2));

      // Add normals for the first triangle: 1-2-4
      normalArray.push_back(norm1);
      normalArray.push_back(norm2);
      normalArray.push_back(norm4);

      // Add normals for the second triangle: 1-4-3
      normalArray.push_back(norm1);
      normalArray.push_back(norm4);
      normalArray.push_back(norm3);

      // Calculate texture coordinates (UV mapping)
      float texU1 = static_cast<float>(tubeIdx) / static_cast<float>(sides);
      float texU2 = static_cast<float>(tubeIdx + 1) / static_cast<float>(sides);
      float texV1 = static_cast<float>(ringIdx) / static_cast<float>(rings);
      float texV2 = static_cast<float>(ringIdx + 1) / static_cast<float>(rings);

      // Add texture coordinates for the first triangle: 1-2-4
      textureMap.push_back(Point2D(texU1, texV1));  // 1
      textureMap.push_back(Point2D(texU2, texV1));  // 2
      textureMap.push_back(Point2D(texU2, texV2));  // 4

      // Add texture coordinates for the second triangle: 1-4-3
      textureMap.push_back(Point2D(texU1, texV1));  // 1
      textureMap.push_back(Point2D(texU2, texV2));  // 4
      textureMap.push_back(Point2D(texU1, texV2));  // 3
    }
  }

  return std::pair(std::pair(vertexArray, normalArray), textureMap);
}

/**
 * @brief Generates a torus/donut mesh and saves it to a file
 *
 * @param majorRadius Distance from center of torus to center of tube
 * @param minorRadius Radius of the tube itself
 * @param sides Number of divisions around the tube (minor circle)
 * @param rings Number of divisions around the torus (major circle)
 * @param filepath Path where the mesh file should be saved
 * @param advanced If true, saves with normals and textures; if false, saves
 * only vertices
 * @return True if file was successfully saved
 */
bool generateDonutTriangles(float majorRadius, float minorRadius, int sides,
                            int rings, const char* filepath, bool advanced) {
  std::pair<std::pair<std::vector<Point>, std::vector<Point>>,
            std::vector<Point2D>>
      torusData = donutAllPoints(majorRadius, minorRadius, sides, rings);

  if (advanced) {
    save3DAdvancedfile(torusData.first.first, torusData.first.second,
                       torusData.second, filepath);
  } else {
    saveToFile(torusData.first.first, filepath);
  }

  return true;
}