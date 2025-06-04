#include <cmath>
#include <fstream>
#include <iostream>
#include <save3dFile.hpp>
#include <vector>

#include "utils.hpp"

std::pair<std::pair<std::vector<Point>, std::vector<Point>>,
          std::vector<Point2D>>
coneTriangles(const float baseRadius, const float coneHeight,
              const size_t numSlices, const size_t numStacks) {
  std::vector<Point> trianglePoints;
  std::vector<Point> vertexNormals;
  std::vector<Point2D> textureCoords;

  float anglePerSlice = (float)(2 * M_PI) / numSlices;
  float heightPerStack = (float)coneHeight / numStacks;
  float radiusDecrement = (float)baseRadius / numStacks;

  // Base
  for (float angle = 0; angle < (2 * M_PI); angle += anglePerSlice) {
    float centerX = 0;
    float centerY = 0;
    float centerZ = 0;

    float outerX1 = baseRadius * std::sin(angle);
    float outerY1 = 0;
    float outerZ1 = baseRadius * std::cos(angle);

    float outerX2 = baseRadius * std::sin(angle + anglePerSlice);
    float outerY2 = 0;
    float outerZ2 = baseRadius * std::cos(angle + anglePerSlice);

    trianglePoints.push_back(Point(outerX1, outerY1, outerZ1));
    trianglePoints.push_back(Point(centerX, centerY, centerZ));
    trianglePoints.push_back(Point(outerX2, outerY2, outerZ2));

    vertexNormals.push_back(Point(0, -1, 0));
    vertexNormals.push_back(Point(0, -1, 0));
    vertexNormals.push_back(Point(0, -1, 0));

    textureCoords.push_back(
        Point2D(0.5 + 0.5 * sin(angle), 0.5 + 0.5 * cos(angle)));
    textureCoords.push_back(Point2D(0.5, 0.5));
    textureCoords.push_back(Point2D(0.5 + 0.5 * sin(angle + anglePerSlice),
                                    0.5 + 0.5 * cos(angle + anglePerSlice)));
  }

  float precomputedNormals[numSlices * 2][3];
  int isFirstStack = 1;
  int currentIndex = 0;

  for (int stackIdx = 0; stackIdx < numStacks; stackIdx++) {
    for (float angle = 0; angle < (2 * M_PI); angle += anglePerSlice) {
      float lowerY = stackIdx * heightPerStack;
      float upperY = (stackIdx + 1) * heightPerStack;

      float lowerRadius = baseRadius - (stackIdx * radiusDecrement);
      float upperRadius = baseRadius - ((stackIdx + 1) * radiusDecrement);

      float lowerX1 = lowerRadius * sin(angle);
      float lowerZ1 = lowerRadius * cos(angle);
      Point normalLower1;
      if (isFirstStack) {
        normalLower1.x = sin(angle);
        normalLower1.y = (float)lowerRadius / coneHeight;
        normalLower1.z = cos(angle);
        normalLower1 = normalLower1.normalize();
        precomputedNormals[currentIndex][0] = normalLower1.x;
        precomputedNormals[currentIndex][1] = normalLower1.y;
        precomputedNormals[currentIndex][2] = normalLower1.z;
      } else {
        normalLower1.x = precomputedNormals[currentIndex][0];
        normalLower1.y = precomputedNormals[currentIndex][1];
        normalLower1.z = precomputedNormals[currentIndex][2];
      }
      currentIndex++;

      float lowerX2 = lowerRadius * sin(angle + anglePerSlice);
      float lowerZ2 = lowerRadius * cos(angle + anglePerSlice);
      Point normalLower2;
      if (isFirstStack) {
        normalLower2.x = sin(angle + anglePerSlice);
        normalLower2.y = (float)lowerRadius / coneHeight;
        normalLower2.z = cos(angle + anglePerSlice);
        normalLower2 = normalLower2.normalize();
        precomputedNormals[currentIndex][0] = normalLower2.x;
        precomputedNormals[currentIndex][1] = normalLower2.y;
        precomputedNormals[currentIndex][2] = normalLower2.z;
      } else {
        normalLower2.x = precomputedNormals[currentIndex][0];
        normalLower2.y = precomputedNormals[currentIndex][1];
        normalLower2.z = precomputedNormals[currentIndex][2];
      }
      currentIndex++;

      if (stackIdx == numStacks - 1) {
        Point avgTopNormal;
        avgTopNormal.x = (sin(angle) + sin(angle + anglePerSlice)) / 2;
        avgTopNormal.y = 0;
        avgTopNormal.z = (cos(angle) + cos(angle + anglePerSlice)) / 2;
        avgTopNormal = avgTopNormal.normalize();
        precomputedNormals[currentIndex - 2][0] = avgTopNormal.x;
        precomputedNormals[currentIndex - 2][1] = avgTopNormal.y;
        precomputedNormals[currentIndex - 2][2] = avgTopNormal.z;
      }

      float upperX1 = upperRadius * sin(angle);
      float upperZ1 = upperRadius * cos(angle);
      Point normalUpper1 = Point(precomputedNormals[currentIndex - 2][0],
                                 precomputedNormals[currentIndex - 2][1],
                                 precomputedNormals[currentIndex - 2][2]);

      float upperX2 = upperRadius * sin(angle + anglePerSlice);
      float upperZ2 = upperRadius * cos(angle + anglePerSlice);
      Point normalUpper2 = Point(precomputedNormals[currentIndex - 1][0],
                                 precomputedNormals[currentIndex - 1][1],
                                 precomputedNormals[currentIndex - 1][2]);

      if (stackIdx == numStacks - 1) {
        Point avgTopNormal;
        avgTopNormal.x = (sin(angle) + sin(angle + anglePerSlice)) / 2.0f;
        avgTopNormal.y = 0;
        avgTopNormal.z = (cos(angle) + cos(angle + anglePerSlice)) / 2.0f;
        avgTopNormal = avgTopNormal.normalize();
        normalUpper2 = avgTopNormal;
      }

      trianglePoints.push_back(Point(upperX1, upperY, upperZ1));
      trianglePoints.push_back(Point(lowerX1, lowerY, lowerZ1));
      trianglePoints.push_back(Point(lowerX2, lowerY, lowerZ2));

      trianglePoints.push_back(Point(upperX1, upperY, upperZ1));
      trianglePoints.push_back(Point(lowerX2, lowerY, lowerZ2));
      trianglePoints.push_back(Point(upperX2, upperY, upperZ2));

      vertexNormals.push_back(normalUpper1);
      vertexNormals.push_back(normalLower1);
      vertexNormals.push_back(normalLower2);

      vertexNormals.push_back(normalUpper1);
      vertexNormals.push_back(normalLower2);
      vertexNormals.push_back(normalUpper2);

      textureCoords.push_back(Point2D(angle / (2 * M_PI), upperY / coneHeight));
      textureCoords.push_back(Point2D(angle / (2 * M_PI), lowerY / coneHeight));
      textureCoords.push_back(
          Point2D((angle + anglePerSlice) / (2 * M_PI), lowerY / coneHeight));

      textureCoords.push_back(Point2D(angle / (2 * M_PI), upperY / coneHeight));
      textureCoords.push_back(
          Point2D((angle + anglePerSlice) / (2 * M_PI), lowerY / coneHeight));
      textureCoords.push_back(
          Point2D((angle + anglePerSlice) / (2 * M_PI), upperY / coneHeight));
    }
    isFirstStack = 0;
    currentIndex = 0;
  }

  return std::make_pair(std::make_pair(trianglePoints, vertexNormals),
                        textureCoords);
}

bool generateConeTriangles(float radius, float height, int slices, int stacks,
                           const char* filepath, bool advanced) {
  std::pair<std::pair<std::vector<Point>, std::vector<Point>>,
            std::vector<Point2D>>
      cone = coneTriangles(radius, height, slices, stacks);

  if (advanced) {
    save3DAdvancedfile(cone.first.first, cone.first.second, cone.second,
                       filepath);
  } else {
    saveToFile(cone.first.first, filepath);
  }
  return true;
}
