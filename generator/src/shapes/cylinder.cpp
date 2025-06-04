#include "shapes/cylinder.hpp"

#include <cmath>
#include <fstream>
#include <iostream>
#include <save3dFile.hpp>
#include <vector>

#include "utils.hpp"

std::pair<std::pair<std::vector<Point>, std::vector<Point>>,
          std::vector<Point2D>>
calculateCylinder(const float radius, const float height, const int slices) {
  std::vector<Point> vertices;
  std::vector<Point> normals;
  std::vector<Point2D> textureCoords;

  Point vertex;
  Point normal;
  Point2D texCoord;

  float angleStep = 2.0f * M_PI / slices;

  for (int i = 0; i < slices; ++i) {
    // Top circle
    normal = {0.0f, 1.0f, 0.0f};
    vertex = {0.0f, height / 2.0f, 0.0f};
    texCoord = {0.4375f, 0.1875f};

    vertices.push_back(vertex);
    normals.push_back(normal);
    textureCoords.push_back(texCoord);

    vertex = {radius * sin(i * angleStep), height / 2.0f,
              radius * cos(i * angleStep)};
    texCoord = {0.4375f + 0.1875f * sin(i * angleStep),
                0.1875f + 0.1875f * cos(i * angleStep)};

    vertices.push_back(vertex);
    normals.push_back(normal);
    textureCoords.push_back(texCoord);

    vertex = {radius * sin((i + 1) * angleStep), height / 2.0f,
              radius * cos((i + 1) * angleStep)};
    texCoord = {0.4375f + 0.1875f * sin((i + 1) * angleStep),
                0.1875f + 0.1875f * cos((i + 1) * angleStep)};

    vertices.push_back(vertex);
    normals.push_back(normal);
    textureCoords.push_back(texCoord);

    // Side wall
    normal = {sin((i + 1) * angleStep), 0.0f, cos((i + 1) * angleStep)};
    vertex = {radius * sin((i + 1) * angleStep), height / 2.0f,
              radius * cos((i + 1) * angleStep)};
    texCoord = {(i + 1) / static_cast<float>(slices), 1.0f};

    vertices.push_back(vertex);
    normals.push_back(normal);
    textureCoords.push_back(texCoord);

    normal = {sin(i * angleStep), 0.0f, cos(i * angleStep)};
    vertex = {radius * sin(i * angleStep), height / 2.0f,
              radius * cos(i * angleStep)};
    texCoord = {i / static_cast<float>(slices), 1.0f};

    vertices.push_back(vertex);
    normals.push_back(normal);
    textureCoords.push_back(texCoord);

    vertex = {radius * sin(i * angleStep), -height / 2.0f,
              radius * cos(i * angleStep)};
    texCoord = {i / static_cast<float>(slices), 0.375f};

    vertices.push_back(vertex);
    normals.push_back(normal);
    textureCoords.push_back(texCoord);

    normal = {sin((i + 1) * angleStep), 0.0f, cos((i + 1) * angleStep)};
    vertex = {radius * sin((i + 1) * angleStep), -height / 2.0f,
              radius * cos((i + 1) * angleStep)};
    texCoord = {(i + 1) / static_cast<float>(slices), 0.375f};

    vertices.push_back(vertex);
    normals.push_back(normal);
    textureCoords.push_back(texCoord);

    vertex = {radius * sin((i + 1) * angleStep), height / 2.0f,
              radius * cos((i + 1) * angleStep)};
    texCoord = {(i + 1) / static_cast<float>(slices), 1.0f};

    vertices.push_back(vertex);
    normals.push_back(normal);
    textureCoords.push_back(texCoord);

    vertex = {radius * sin(i * angleStep), -height / 2.0f,
              radius * cos(i * angleStep)};
    texCoord = {i / static_cast<float>(slices), 0.375f};

    vertices.push_back(vertex);
    normals.push_back(normal);
    textureCoords.push_back(texCoord);

    // Bottom circle
    normal = {0.0f, -1.0f, 0.0f};
    vertex = {0.0f, -height / 2.0f, 0.0f};
    texCoord = {0.8125f, 0.1875f};

    vertices.push_back(vertex);
    normals.push_back(normal);
    textureCoords.push_back(texCoord);

    vertex = {radius * sin((i + 1) * angleStep), -height / 2.0f,
              radius * cos((i + 1) * angleStep)};
    texCoord = {0.8125f + 0.1875f * sin((i + 1) * angleStep),
                0.1875f + 0.1875f * cos((i + 1) * angleStep)};

    vertices.push_back(vertex);
    normals.push_back(normal);
    textureCoords.push_back(texCoord);

    vertex = {radius * sin(i * angleStep), -height / 2.0f,
              radius * cos(i * angleStep)};
    texCoord = {0.8125f + 0.1875f * sin(i * angleStep),
                0.1875f + 0.1875f * cos(i * angleStep)};

    vertices.push_back(vertex);
    normals.push_back(normal);
    textureCoords.push_back(texCoord);
  }

  return std::make_pair(std::make_pair(vertices, normals), textureCoords);
}

bool generateCylinderTriangles(float radius, float height, int slices,
                               const char* filepath, bool advanced) {
  auto cylinderData = calculateCylinder(radius, height, slices);

  const auto& vertices = cylinderData.first.first;
  const auto& normals = cylinderData.first.second;
  const auto& textureCoords = cylinderData.second;

  if (advanced) {
    save3DAdvancedfile(vertices, normals, textureCoords, filepath);
  } else {
    saveToFile(vertices, filepath);
  }

  return true;
}
