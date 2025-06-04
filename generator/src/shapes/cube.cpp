#include <fstream>
#include <iostream>
#include <save3dFile.hpp>
#include <vector>

#include "utils.hpp"

/**
 * @brief Generates a cube divided into a grid of triangles on each face
 *
 * This function creates a cube centered at the origin with the specified length
 * and divides each face into the given number of segments.
 *
 * @param length The side length of the cube
 * @param divisions Number of divisions per face side
 * @return A pair containing: pair(vertices, normals) and texture coordinates
 */
std::pair<std::pair<std::vector<Point>, std::vector<Point>>,
          std::vector<Point2D>>
cubeAllPoints(float length, int divisions) {
  float cubeSide = length / 2.0f;       // Half length for centering at origin
  float gridSize = length / divisions;  // Size of each grid cell

  std::vector<Point> vertexArray;  // Store all vertices
  std::vector<Point> normalArray;  // Store all normal vectors
  std::vector<Point2D> uvCoords;   // Store all texture coordinates

  // Create each face of the cube
  for (int row = 0; row < divisions; ++row) {
    for (int col = 0; col < divisions; ++col) {
      // Calculate grid cell corner positions
      float posA = -cubeSide + row * gridSize;
      float posB = -cubeSide + col * gridSize;
      float nextA = posA + gridSize;
      float nextB = posB + gridSize;

      // ----- Front Face (Z+) -----
      vertexArray.push_back(Point(posA, posB, cubeSide));
      vertexArray.push_back(Point(nextA, posB, cubeSide));
      vertexArray.push_back(Point(posA, nextB, cubeSide));

      vertexArray.push_back(Point(posA, nextB, cubeSide));
      vertexArray.push_back(Point(nextA, posB, cubeSide));
      vertexArray.push_back(Point(nextA, nextB, cubeSide));

      Point frontNormal = Point(0.0f, 0.0f, 1.0f);
      for (int vertex = 0; vertex < 6; vertex++) {
        normalArray.push_back(frontNormal);
      }

      // Front face texture coordinates
      float texA = (posA + cubeSide) / length;  // Normalize to [0,1]
      float texB = (posB + cubeSide) / length;
      float texNextA = (nextA + cubeSide) / length;
      float texNextB = (nextB + cubeSide) / length;

      uvCoords.push_back(Point2D(texA, texB));
      uvCoords.push_back(Point2D(texNextA, texB));
      uvCoords.push_back(Point2D(texA, texNextB));
      uvCoords.push_back(Point2D(texA, texNextB));
      uvCoords.push_back(Point2D(texNextA, texB));
      uvCoords.push_back(Point2D(texNextA, texNextB));

      // ----- Back Face (Z-) -----
      vertexArray.push_back(Point(posA, posB, -cubeSide));
      vertexArray.push_back(Point(posA, nextB, -cubeSide));
      vertexArray.push_back(Point(nextA, posB, -cubeSide));

      vertexArray.push_back(Point(posA, nextB, -cubeSide));
      vertexArray.push_back(Point(nextA, nextB, -cubeSide));
      vertexArray.push_back(Point(nextA, posB, -cubeSide));

      Point backNormal = Point(0.0f, 0.0f, -1.0f);
      for (int vertex = 0; vertex < 6; vertex++) {
        normalArray.push_back(backNormal);
      }

      // Back face texture coordinates
      uvCoords.push_back(Point2D(texA, texB));
      uvCoords.push_back(Point2D(texA, texNextB));
      uvCoords.push_back(Point2D(texNextA, texB));
      uvCoords.push_back(Point2D(texA, texNextB));
      uvCoords.push_back(Point2D(texNextA, texNextB));
      uvCoords.push_back(Point2D(texNextA, texB));

      // ----- Left Face (X-) -----
      vertexArray.push_back(Point(-cubeSide, posA, posB));
      vertexArray.push_back(Point(-cubeSide, posA, nextB));
      vertexArray.push_back(Point(-cubeSide, nextA, posB));

      vertexArray.push_back(Point(-cubeSide, posA, nextB));
      vertexArray.push_back(Point(-cubeSide, nextA, nextB));
      vertexArray.push_back(Point(-cubeSide, nextA, posB));

      Point leftNormal = Point(-1.0f, 0.0f, 0.0f);
      for (int vertex = 0; vertex < 6; vertex++) {
        normalArray.push_back(leftNormal);
      }

      // Left face texture coordinates - using grid-based mapping
      float gridTexU = (row + 0.0f) / divisions;
      float gridTexV = (col + 0.0f) / divisions;
      float gridTexNextU = (row + 1.0f) / divisions;
      float gridTexNextV = (col + 1.0f) / divisions;

      uvCoords.push_back(Point2D(gridTexU, gridTexV));
      uvCoords.push_back(Point2D(gridTexNextU, gridTexV));
      uvCoords.push_back(Point2D(gridTexU, gridTexNextV));
      uvCoords.push_back(Point2D(gridTexNextU, gridTexV));
      uvCoords.push_back(Point2D(gridTexNextU, gridTexNextV));
      uvCoords.push_back(Point2D(gridTexU, gridTexNextV));

      // ----- Right Face (X+) -----
      vertexArray.push_back(Point(cubeSide, posA, posB));
      vertexArray.push_back(Point(cubeSide, nextA, posB));
      vertexArray.push_back(Point(cubeSide, posA, nextB));

      vertexArray.push_back(Point(cubeSide, posA, nextB));
      vertexArray.push_back(Point(cubeSide, nextA, posB));
      vertexArray.push_back(Point(cubeSide, nextA, nextB));

      Point rightNormal = Point(1.0f, 0.0f, 0.0f);
      for (int vertex = 0; vertex < 6; vertex++) {
        normalArray.push_back(rightNormal);
      }

      // Right face texture coordinates
      uvCoords.push_back(Point2D(texB, texA));
      uvCoords.push_back(Point2D(texNextB, texA));
      uvCoords.push_back(Point2D(texB, texNextA));
      uvCoords.push_back(Point2D(texB, texNextA));
      uvCoords.push_back(Point2D(texNextB, texA));
      uvCoords.push_back(Point2D(texNextB, texNextA));

      // ----- Top Face (Y+) -----
      vertexArray.push_back(Point(posA, cubeSide, posB));
      vertexArray.push_back(Point(posA, cubeSide, nextB));
      vertexArray.push_back(Point(nextA, cubeSide, posB));

      vertexArray.push_back(Point(posA, cubeSide, nextB));
      vertexArray.push_back(Point(nextA, cubeSide, nextB));
      vertexArray.push_back(Point(nextA, cubeSide, posB));

      Point topNormal = Point(0.0f, 1.0f, 0.0f);
      for (int vertex = 0; vertex < 6; vertex++) {
        normalArray.push_back(topNormal);
      }

      // Top face texture coordinates
      uvCoords.push_back(Point2D(texA, texB));
      uvCoords.push_back(Point2D(texA, texNextB));
      uvCoords.push_back(Point2D(texNextA, texB));
      uvCoords.push_back(Point2D(texA, texNextB));
      uvCoords.push_back(Point2D(texNextA, texNextB));
      uvCoords.push_back(Point2D(texNextA, texB));

      // ----- Bottom Face (Y-) -----
      vertexArray.push_back(Point(posA, -cubeSide, posB));
      vertexArray.push_back(Point(nextA, -cubeSide, posB));
      vertexArray.push_back(Point(posA, -cubeSide, nextB));

      vertexArray.push_back(Point(posA, -cubeSide, nextB));
      vertexArray.push_back(Point(nextA, -cubeSide, posB));
      vertexArray.push_back(Point(nextA, -cubeSide, nextB));

      Point bottomNormal = Point(0.0f, -1.0f, 0.0f);
      for (int vertex = 0; vertex < 6; vertex++) {
        normalArray.push_back(bottomNormal);
      }

      // Bottom face texture coordinates
      uvCoords.push_back(Point2D(texA, texB));
      uvCoords.push_back(Point2D(texNextA, texB));
      uvCoords.push_back(Point2D(texA, texNextB));
      uvCoords.push_back(Point2D(texA, texNextB));
      uvCoords.push_back(Point2D(texNextA, texB));
      uvCoords.push_back(Point2D(texNextA, texNextB));
    }
  }

  return std::pair(std::pair(vertexArray, normalArray), uvCoords);
}

/**
 * @brief Generates a cube mesh and saves it to a file
 *
 * @param length The side length of the cube
 * @param divisions Number of divisions per face side
 * @param filepath Path where the mesh file should be saved
 * @param advanced If true, saves with normals and textures; if false, saves
 * only vertices
 * @return True if file was successfully saved
 */
bool generateCubeTriangles(float length, int divisions, const char* filepath,
                           bool advanced) {
  std::pair<std::pair<std::vector<Point>, std::vector<Point>>,
            std::vector<Point2D>>
      cubeData = cubeAllPoints(length, divisions);

  if (advanced) {
    printf("Saving advanced cube to %s\n", filepath);
    save3DAdvancedfile(cubeData.first.first, cubeData.first.second,
                       cubeData.second, filepath);
  } else {
    saveToFile(cubeData.first.first, filepath);
  }

  return true;
}