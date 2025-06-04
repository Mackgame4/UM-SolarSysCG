#include "../include/utils.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#define DIR "models/"

std::map<std::string, std::vector<Point>> hash_models;

namespace TerminalColors {
const std::string RESET = "\033[0m";
// Text colors
const std::string BLACK = "\033[30m";
const std::string RED = "\033[31m";
const std::string GREEN = "\033[32m";
const std::string YELLOW = "\033[33m";
const std::string BLUE = "\033[34m";
const std::string MAGENTA = "\033[35m";
const std::string CYAN = "\033[36m";
const std::string WHITE = "\033[37m";
// Background colors
const std::string BG_BLACK = "\033[40m";
const std::string BG_RED = "\033[41m";
const std::string BG_GREEN = "\033[42m";
const std::string BG_YELLOW = "\033[43m";
const std::string BG_BLUE = "\033[44m";
const std::string BG_MAGENTA = "\033[45m";
const std::string BG_CYAN = "\033[46m";
const std::string BG_WHITE = "\033[47m";
}  // namespace TerminalColors

void log(std::string type, std::string message = "") {
  if (type == "info") {
    std::cout << TerminalColors::BG_BLUE << "[Info]" << TerminalColors::RESET
              << " " << message << std::endl;
  } else if (type == "error") {
    std::cerr << TerminalColors::BG_RED << "[Error]" << TerminalColors::RESET
              << " " << message << std::endl;
  } else if (type == "warning") {
    std::cerr << TerminalColors::BG_YELLOW << "[Warning]"
              << TerminalColors::RESET << " " << message << std::endl;
  } else if (type == "success") {
    std::cout << TerminalColors::BG_GREEN << "[Success]"
              << TerminalColors::RESET << " " << message << std::endl;
  } else if (type == "debug") {
    std::cout << TerminalColors::BG_CYAN << "[Debug]" << TerminalColors::RESET
              << " " << message << std::endl;
  } else {
    std::cerr << TerminalColors::BG_MAGENTA << "[Log]" << TerminalColors::RESET
              << " " << message << std::endl;
  }
}

std::vector<Point> parseFile(std::string filePath) {
  std::vector<Point> extractedPoints;

  size_t extensionIndex = filePath.find_last_of(".");
  if (extensionIndex == std::string::npos) {
    log("error", "Failed to determine file extension for: " + filePath +
                     ". Please check the file name.");
    return extractedPoints;
  }

  std::string fileExtension = filePath.substr(extensionIndex + 1);
  if (fileExtension == "obj") {
    log("info", "Processing .obj file: " + filePath);
    extractedPoints = parseOBJfile(filePath);
  } else if (fileExtension == "3d") {
    log("info", "Processing .3d file: " + filePath);
    extractedPoints = parse3Dfile(filePath);
  } else {
    log("error", "Unsupported file format: " + fileExtension +
                     ". Supported formats: .obj, .3d");
  }

  return extractedPoints;
}

void saveToFile(const std::vector<Point>& points, const char* filePath) {
  std::string fullPath(DIR);
  fullPath.append(filePath);

  std::ofstream outputFile(fullPath);

  if (outputFile.is_open()) {
    for (const auto& point : points) {
      outputFile << point.x << " " << point.y << " " << point.z << "\n";
    }
    outputFile.close();
    log("success", "File saved successfully at: " + fullPath);
  } else {
    log("error", "nable to open file for writing: " + fullPath +
                     ". Check directory permissions.");
  }
}

std::vector<Point> parse3Dfile(std::string filename) {
  std::vector<Point> points;
  std::ifstream file(filename);

  Point point;
  while (file >> point.x >> point.y >> point.z) {
    points.push_back(point);
  }

  file.close();

  return points;
}

std::vector<Point> parseOBJfile(std::string filePath) {
  std::vector<Point> vertexPoints;
  std::vector<Point> orderedFacePoints;
  std::ifstream inputFile(filePath);

  if (!inputFile) {
    std::cerr << "[Error] Failed to open .obj file: " << filePath
              << ". Please verify the file path." << std::endl;
    return orderedFacePoints;
  }

  std::string currentLine;
  while (std::getline(inputFile, currentLine)) {
    std::istringstream lineStream(currentLine);
    std::string prefix;
    lineStream >> prefix;

    if (prefix == "v") {
      Point newVertex;
      lineStream >> newVertex.x >> newVertex.y >> newVertex.z;
      vertexPoints.push_back(newVertex);
    } else if (prefix == "f") {
      std::string faceVertex;
      std::vector<int> faceVertexIndices;
      while (lineStream >> faceVertex) {
        std::string vertexIndex = faceVertex.substr(0, faceVertex.find("/"));
        faceVertexIndices.push_back(std::stoi(vertexIndex) - 1);
      }
      for (int index : faceVertexIndices) {
        if (index < 0 || index >= vertexPoints.size()) {
          std::cerr << "[Error] Invalid vertex index " << index + 1
                    << " in face definition. Check the .obj file." << std::endl;
          continue;
        }
        orderedFacePoints.push_back(vertexPoints[index]);
      }
    }
  }
  inputFile.close();
  return orderedFacePoints;
}

Point Point::multiply(float value) {
  return Point(x * value, y * value, z * value);
}