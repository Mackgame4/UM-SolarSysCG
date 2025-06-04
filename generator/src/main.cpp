#include <iostream>
#include <string>

#include "shapes/cone.hpp"
#include "shapes/cube.hpp"
#include "shapes/cylinder.hpp"
#include "shapes/donut.hpp"
#include "shapes/patches.hpp"
#include "shapes/plane.hpp"
#include "shapes/sphere.hpp"

void generateShape(int argc, char* argv[]) {
  if (argc < 5) {
    std::cerr << "Insufficient arguments\n";
    return;
  }

  bool isAdvanced = true;
  int numberOfArguments = argc;
  char* outputFileName;

  // Check if the last argument specifies a simple or advanced generation
  if (argv[argc - 1][1] == 's') {
    outputFileName = argv[argc - 2];
    isAdvanced = false;
    numberOfArguments -= 1;
  } else {
    outputFileName = argv[argc - 1];
  }

  std::string shapeName = argv[1];

  // Sphere generation
  if (shapeName == "sphere" && numberOfArguments == 6) {
    std::cout << "Generating Sphere\n";
    float radius = std::stof(argv[2]);
    int slices = std::stoi(argv[3]);
    int stacks = std::stoi(argv[4]);

    generateSphereTriangles(radius, slices, stacks, outputFileName, isAdvanced);
  }
  // Cube generation
  else if (shapeName == "box" && numberOfArguments == 5) {
    std::cout << "Generating Cube\n";
    float sideLength = std::stof(argv[2]);
    int divisions = std::stoi(argv[3]);

    generateCubeTriangles(sideLength, divisions, outputFileName, isAdvanced);
  }
  // Plane generation
  else if (shapeName == "plane" && numberOfArguments == 5) {
    std::cout << "Generating Plane\n";
    float sideLength = std::stof(argv[2]);
    int divisions = std::stoi(argv[3]);

    generatePlaneTriangles(sideLength, divisions, outputFileName, isAdvanced);
  }
  // Cone generation
  else if (shapeName == "cone" && numberOfArguments == 7) {
    std::cout << "Generating Cone\n";
    float radius = std::stof(argv[2]);
    float height = std::stof(argv[3]);
    int slices = std::stoi(argv[4]);
    int stacks = std::stoi(argv[5]);

    generateConeTriangles(radius, height, slices, stacks, outputFileName,
                          isAdvanced);
  }
  // Donut (Torus) generation
  else if (shapeName == "donut" && numberOfArguments == 7) {
    std::cout << "Generating Donut\n";
    float innerRadius = std::stof(argv[2]);
    float outerRadius = std::stof(argv[3]);
    int slices = std::stoi(argv[4]);
    int stacks = std::stoi(argv[5]);

    generateDonutTriangles(innerRadius, outerRadius, slices, stacks,
                           outputFileName, isAdvanced);
  }
  // Cylinder generation
  else if (shapeName == "cylinder" && numberOfArguments == 6) {
    std::cout << "Generating Cylinder\n";
    float radius = std::stof(argv[2]);
    float height = std::stof(argv[3]);
    int slices = std::stoi(argv[4]);

    generateCylinderTriangles(radius, height, slices, outputFileName,
                              isAdvanced);
  }
  // Patch generation
  else if (shapeName == "patch" && numberOfArguments == 5) {
    std::cout << "Generating Patch\n";
    char* bezierPatchFile = argv[2];
    int tessellation = std::stoi(argv[3]);
    std::cout << "Tessellation level: " << tessellation << std::endl;
    std::cout << "Bezier patch file: " << bezierPatchFile << std::endl;

    generatePatchTriangles(bezierPatchFile, tessellation, outputFileName,
                           isAdvanced);
  }
  // Invalid arguments
  else {
    std::cerr << "Invalid arguments\n";
  }
}

int main(int argc, char* argv[]) {
  generateShape(argc, argv);
  return 0;
}
