#include <cmath>
#include <fstream>
#include <iostream>
#include <save3dFile.hpp>
#include <vector>

#include "utils.hpp"

// Função que gera os pontos, normais e coordenadas de textura para uma esfera
std::pair<std::pair<std::vector<Point>, std::vector<Point>>,
          std::vector<Point2D>>
generateSpherePoints(float radius, int slices, int stacks) {
  std::vector<Point> vertices;  // Vértices da esfera
  std::vector<Point> normals;   // Normais para cada vértice
  std::vector<Point2D>
      textureCoords;  // Coordenadas de textura para cada vértice

  // Loop sobre os slices e stacks
  for (int i = 0; i < slices; ++i) {
    float angleTheta1 = static_cast<float>(i) * static_cast<float>(M_PI) /
                        static_cast<float>(slices);
    float angleTheta2 = static_cast<float>(i + 1) * static_cast<float>(M_PI) /
                        static_cast<float>(slices);

    for (int j = 0; j < stacks; ++j) {
      float anglePhi1 = static_cast<float>(j) * 2.0f *
                        static_cast<float>(M_PI) / static_cast<float>(stacks);
      float anglePhi2 = static_cast<float>(j + 1) * 2.0f *
                        static_cast<float>(M_PI) / static_cast<float>(stacks);

      // Cálculo das coordenadas dos vértices
      Point p1(radius * std::sin(angleTheta1) * std::sin(anglePhi1),
               radius * std::cos(angleTheta1),
               radius * std::sin(angleTheta1) * std::cos(anglePhi1));
      Point p2(radius * std::sin(angleTheta1) * std::sin(anglePhi2),
               radius * std::cos(angleTheta1),
               radius * std::sin(angleTheta1) * std::cos(anglePhi2));
      Point p3(radius * std::sin(angleTheta2) * std::sin(anglePhi1),
               radius * std::cos(angleTheta2),
               radius * std::sin(angleTheta2) * std::cos(anglePhi1));
      Point p4(radius * std::sin(angleTheta2) * std::sin(anglePhi2),
               radius * std::cos(angleTheta2),
               radius * std::sin(angleTheta2) * std::cos(anglePhi2));

      // Adicionar os vértices para as faces
      vertices.push_back(p1);
      vertices.push_back(p4);
      vertices.push_back(p2);
      vertices.push_back(p1);
      vertices.push_back(p3);
      vertices.push_back(p4);

      // Adicionar as normais (normalizadas)
      normals.push_back(p1.normalize());
      normals.push_back(p4.normalize());
      normals.push_back(p2.normalize());
      normals.push_back(p1.normalize());
      normals.push_back(p3.normalize());
      normals.push_back(p4.normalize());

      // Cálculo das coordenadas de textura
      float u1 = anglePhi1 / (2.0f * static_cast<float>(M_PI));
      float u2 = anglePhi2 / (2.0f * static_cast<float>(M_PI));
      float v1 = angleTheta1 / static_cast<float>(M_PI);
      float v2 = angleTheta2 / static_cast<float>(M_PI);

      // Coordenadas de textura para os vértices
      textureCoords.push_back(Point2D(u1, v1));
      textureCoords.push_back(Point2D(u2, v2));
      textureCoords.push_back(Point2D(u1, v2));
      textureCoords.push_back(Point2D(u1, v1));
      textureCoords.push_back(Point2D(u1, v2));
      textureCoords.push_back(Point2D(u2, v2));
    }
  }

  return std::make_pair(std::make_pair(vertices, normals), textureCoords);
}

// Função para gerar e salvar a esfera no formato desejado
bool generateSphereTriangles(float radius, int slices, int stacks,
                             const char* filepath, bool advanced) {
  // Gerar os pontos, normais e coordenadas de textura da esfera
  std::pair<std::pair<std::vector<Point>, std::vector<Point>>,
            std::vector<Point2D>>
      sphere = generateSpherePoints(radius, slices, stacks);

  // Salvar os dados da esfera, dependendo do tipo de arquivo desejado
  if (advanced) {
    save3DAdvancedfile(sphere.first.first, sphere.first.second, sphere.second,
                       filepath);
  } else {
    saveToFile(sphere.first.first, filepath);
  }

  return true;
}
