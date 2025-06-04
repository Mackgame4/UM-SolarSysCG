#include "shapes/patches.hpp"

#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

#include "save3dFile.hpp"
#include "utils.hpp"

Point bezierPatch(const std::vector<Point>& controlPoints, float u, float v);
Point bezierPatchDU(const std::vector<Point>& controlPoints, float u, float v);
Point bezierPatchDV(const std::vector<Point>& controlPoints, float u, float v);
float bernstein(int i, float t);

std::pair<std::pair<std::vector<Point>, std::vector<Point>>,
          std::vector<Point2D>>
patchTriangles(const char* inputFile, const int tessellationLevel) {
  std::vector<Point> controlPoints;

  // printing actual directory
  std::cout << "Current working directory: " << std::filesystem::current_path()
            << std::endl;
  std::cout << "Input file: " << inputFile << std::endl;

  // Open and read the input file
  std::ifstream inputStream(inputFile);
  if (!inputStream.is_open()) {
    std::cerr << "Error opening input file\n";
    return {{{}, {}}, {}};
  }

  // Read number of patches
  size_t patchCount;
  inputStream >> patchCount;

  // Read patch indices (16 control points per patch)
  std::vector<std::vector<size_t>> patches(patchCount, std::vector<size_t>(16));
  for (auto& patch : patches) {
    for (auto& index : patch) {
      inputStream >> index;
      inputStream.ignore();
    }
  }

  // Read all control points
  size_t controlPointCount;
  inputStream >> controlPointCount;
  controlPoints.reserve(controlPointCount);

  for (size_t i = 0; i < controlPointCount; ++i) {
    float x, y, z;
    char separator;
    inputStream >> x >> separator >> y >> separator >> z;
    controlPoints.emplace_back(x, y, z);
  }
  inputStream.close();

  // Generate triangles for each patch
  std::vector<Point> vertices;
  std::vector<Point> normals;
  std::vector<Point2D> texCoords;

  for (const auto& patchIndices : patches) {
    // Get the 16 control points for this patch
    std::vector<Point> currentPatchPoints;
    for (auto index : patchIndices) {
      currentPatchPoints.push_back(controlPoints[index]);
    }

    // Tessellate the patch
    for (int uStep = 0; uStep < tessellationLevel; ++uStep) {
      for (int vStep = 0; vStep < tessellationLevel; ++vStep) {
        // Calculate parameter values
        float u1 = static_cast<float>(uStep) / tessellationLevel;
        float u2 = static_cast<float>(uStep + 1) / tessellationLevel;
        float v1 = static_cast<float>(vStep) / tessellationLevel;
        float v2 = static_cast<float>(vStep + 1) / tessellationLevel;

        // Calculate four corner points
        Point p1 = bezierPatch(currentPatchPoints, u1, v1);
        Point p2 = bezierPatch(currentPatchPoints, u2, v1);
        Point p3 = bezierPatch(currentPatchPoints, u1, v2);
        Point p4 = bezierPatch(currentPatchPoints, u2, v2);

        // Add two triangles (forming a quad)
        vertices.insert(vertices.end(), {p1, p3, p2, p2, p3, p4});

        // Calculate normals (using surface derivatives)
        auto calculateNormal = [&](float u, float v) {
          Point du = bezierPatchDU(currentPatchPoints, u, v);
          Point dv = bezierPatchDV(currentPatchPoints, u, v);
          Point normal = dv.cross(du);
          normal.normalize();
          return normal;
        };

        Point n1 = calculateNormal(u1, v1);
        Point n2 = calculateNormal(u2, v1);
        Point n3 = calculateNormal(u1, v2);
        Point n4 = calculateNormal(u2, v2);

        // Add normals (matching vertex order)
        normals.insert(normals.end(), {n1, n3, n2, n2, n3, n4});

        // Add texture coordinates
        texCoords.insert(texCoords.end(),
                         {Point2D(u1, v1), Point2D(u1, v2), Point2D(u2, v1),
                          Point2D(u2, v1), Point2D(u1, v2), Point2D(u2, v2)});
      }
    }
  }

  return {{vertices, normals}, texCoords};
}

Point bezierPatch(const std::vector<Point>& controlPoints, float u, float v) {
  Point result(0, 0, 0);
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      float basis = bernstein(i, u) * bernstein(j, v);
      result.x += controlPoints[i * 4 + j].x * basis;
      result.y += controlPoints[i * 4 + j].y * basis;
      result.z += controlPoints[i * 4 + j].z * basis;
    }
  }
  return result;
}

Point bezierPatchDU(const std::vector<Point>& controlPoints, float u, float v) {
  Point result(0, 0, 0);
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      float basis = bernsteinDerivative(i, u) * bernstein(j, v);
      result.x += controlPoints[i * 4 + j].x * basis;
      result.y += controlPoints[i * 4 + j].y * basis;
      result.z += controlPoints[i * 4 + j].z * basis;
    }
  }
  return result;
}

Point bezierPatchDV(const std::vector<Point>& controlPoints, float u, float v) {
  Point result(0, 0, 0);
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      float basis = bernstein(i, u) * bernsteinDerivative(j, v);
      result.x += controlPoints[i * 4 + j].x * basis;
      result.y += controlPoints[i * 4 + j].y * basis;
      result.z += controlPoints[i * 4 + j].z * basis;
    }
  }
  return result;
}

float bernstein(int i, float t) {
  switch (i) {
    case 0:
      return (1 - t) * (1 - t) * (1 - t);  // (1-t)³
    case 1:
      return 3 * t * (1 - t) * (1 - t);  // 3t(1-t)²
    case 2:
      return 3 * t * t * (1 - t);  // 3t²(1-t)
    case 3:
      return t * t * t;  // t³
    default:
      return 0;
  }
}

float bernsteinDerivative(int i, float t) {
  switch (i) {
    case 0:
      return -3 * (1 - t) * (1 - t);
    case 1:
      return 3 * (1 - t) * (1 - t) - 6 * t * (1 - t);
    case 2:
      return 6 * t * (1 - t) - 3 * t * t;
    case 3:
      return 3 * t * t;
    default:
      return 0;
  }
}

bool generatePatchTriangles(const char* inputFile, const int tessellationLevel,
                            const char* outputFile, bool advancedMode) {
  auto patchData = patchTriangles(inputFile, tessellationLevel);

  if (advancedMode) {
    save3DAdvancedfile(patchData.first.first, patchData.first.second,
                       patchData.second, outputFile);
  } else {
    saveToFile(patchData.first.first, outputFile);
  }

  return true;
}