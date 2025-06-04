#include <GL/glew.h>
extern "C" {
#include <GL/gl.h>
#ifdef __APPLE_CC__
#include <GLUT/glut.h>
#else
#include <GL/freeglut.h>
#endif
}

#define STB_IMAGE_IMPLEMENTATION
#define _USE_MATH_DEFINES
#include <math.h>

#include <unordered_map>

#include "Model.hpp"

// Global counter for model IDs
unsigned int model_counter = 0;

/**
 * Extracts position coordinates from vertices into a flat vector
 */
std::vector<float> extractPositions(const std::vector<Vertex>& vertices) {
  std::vector<float> result;
  result.reserve(vertices.size() * 3);

  for (const Vertex& vertex : vertices) {
    result.push_back(vertex.position.x);
    result.push_back(vertex.position.y);
    result.push_back(vertex.position.z);
  }
  return result;
}

/**
 * Extracts normal vectors from vertices into a flat vector
 */
std::vector<float> extractNormals(const std::vector<Vertex>& vertices) {
  std::vector<float> result;
  result.reserve(vertices.size() * 3);

  for (const Vertex& vertex : vertices) {
    result.push_back(vertex.normal.x);
    result.push_back(vertex.normal.y);
    result.push_back(vertex.normal.z);
  }
  return result;
}

/**
 * Extracts texture coordinates from vertices into a flat vector
 */
std::vector<float> extractTexCoords(const std::vector<Vertex>& vertices) {
  std::vector<float> result;
  result.reserve(vertices.size() * 2);

  for (const Vertex& vertex : vertices) {
    result.push_back(vertex.texture.x);
    result.push_back(vertex.texture.y);
  }
  return result;
}

/**
 * Creates a vertex buffer with unique vertices
 */
std::vector<Vertex> createVertexBuffer(const std::vector<Vertex>& vertices) {
  std::vector<Vertex> uniqueVertices;
  std::unordered_map<Vertex, int, VertexHash> vertexIndices;

  for (const Vertex& vertex : vertices) {
    if (vertexIndices.find(vertex) == vertexIndices.end()) {
      vertexIndices[vertex] = uniqueVertices.size();
      uniqueVertices.push_back(vertex);
    }
  }
  return uniqueVertices;
}

/**
 * Creates an index buffer for the given vertices using the VBO as reference
 */
std::vector<unsigned int> createIndexBuffer(
    const std::vector<Vertex>& vertices,
    const std::vector<Vertex>& uniqueVertices) {
  std::vector<unsigned int> indices;
  indices.reserve(vertices.size());

  std::unordered_map<Vertex, int, VertexHash> vertexIndices;
  for (size_t i = 0; i < uniqueVertices.size(); ++i) {
    vertexIndices[uniqueVertices[i]] = i;
  }

  for (const Vertex& vertex : vertices) {
    indices.push_back(vertexIndices[vertex]);
  }
  return indices;
}

// Default constructor
Model::Model() {
  this->filename = "";
  this->id = -1;
  this->initialized = false;
  model_counter++;
}

// Constructor with pre-computed buffers
Model::Model(std::string filename, std::vector<Vertex> vbo,
             std::vector<unsigned int> ibo, int id,
             std::vector<Vertex> points) {
  this->filename = filename;
  this->vbo = vbo;
  this->ibo = ibo;
  this->id = id;
  this->initialized = false;
  this->_points = points;
}

// Constructor that computes buffers from raw vertices
Model::Model(std::string filename, std::vector<Vertex> points) {
  this->filename = filename;
  this->id = model_counter;
  this->vbo = createVertexBuffer(points);
  this->ibo = createIndexBuffer(points, this->vbo);
  this->initialized = false;
  this->_points = points;
  model_counter++;
}

/**
 * Initialize the model if not already initialized
 */
void Model::initModel() {
  if (!this->initialized) {
    this->initialized = true;
    setupModel();

    if (!this->texture_filepath.empty()) {
      bool textureLoaded = loadTexture();
      if (!textureLoaded) {
        std::cerr << "Error: Failed to load texture file: "
                  << this->texture_filepath << std::endl;
      }
    }
  }
}

/**
 * Load texture from file and configure OpenGL texture parameters
 */
bool Model::loadTexture() {
  // Load image data
  int width, height, numChannels;
  unsigned char* imageData = stbi_load(this->texture_filepath.data(), &width,
                                       &height, &numChannels, STBI_rgb_alpha);

  // Debug information
  std::cout << "Loading texture: " << this->texture_filepath << std::endl;

  if (!imageData) {
    std::cerr << "Failed to load texture: " << this->texture_filepath
              << std::endl;
    return false;
  }

  // Generate and bind texture
  glGenTextures(1, &this->_texture_id);
  glBindTexture(GL_TEXTURE_2D, this->_texture_id);

  // Set texture parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  // Use mipmapping for better quality at different distances
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // Ensure proper alignment when uploading
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  // Upload texture data to GPU
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, imageData);
  glGenerateMipmap(GL_TEXTURE_2D);

  // Unbind the texture
  glBindTexture(GL_TEXTURE_2D, 0);

  // Free image data after uploading it
  stbi_image_free(imageData);

  return true;
}

/**
 * Setup vertex, normal, texture, and index buffers for the model
 */
void Model::setupModel() {
  std::vector<float> positions = extractPositions(this->vbo);
  std::vector<float> normals = extractNormals(this->vbo);
  std::vector<float> texCoords = extractTexCoords(this->vbo);

  // Generate and configure vertex position buffer
  glGenBuffers(1, &this->_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, this->_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * positions.size(),
               positions.data(), GL_STATIC_DRAW);

  // Generate and configure vertex normal buffer
  glGenBuffers(1, &this->_normals);
  glBindBuffer(GL_ARRAY_BUFFER, this->_normals);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * normals.size(), normals.data(),
               GL_STATIC_DRAW);

  // Generate and configure texture coordinate buffer
  glGenBuffers(1, &this->_textures);
  glBindBuffer(GL_ARRAY_BUFFER, this->_textures);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * texCoords.size(),
               texCoords.data(), GL_STATIC_DRAW);

  // Generate and configure index buffer
  glGenBuffers(1, &this->_ibo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->_ibo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * this->ibo.size(),
               this->ibo.data(), GL_STATIC_DRAW);
}

/**
 * Draw the model using OpenGL
 */
void Model::drawModel() {
  initModel();

  // Bind texture
  glBindTexture(GL_TEXTURE_2D, this->_texture_id);

  // Configure vertex positions
  glBindBuffer(GL_ARRAY_BUFFER, this->_vbo);
  glVertexPointer(3, GL_FLOAT, 0, 0);

  // Configure vertex normals
  glBindBuffer(GL_ARRAY_BUFFER, this->_normals);
  glNormalPointer(GL_FLOAT, 0, 0);

  // Configure texture coordinates
  glBindBuffer(GL_ARRAY_BUFFER, this->_textures);
  glTexCoordPointer(2, GL_FLOAT, 0, 0);

  // Set default color and draw elements
  glColor3f(1.0, 1.0, 1.0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->_ibo);
  glDrawElements(GL_TRIANGLES, this->ibo.size(), GL_UNSIGNED_INT, 0);

  // Unbind texture
  glBindTexture(GL_TEXTURE_2D, 0);
}

/**
 * Visualize vertex normals for debugging purposes
 */
void Model::drawNormals() {
  glDisable(GL_LIGHTING);
  glColor3f(1.0, 0.0, 0.0);

  for (const Vertex& vertex : this->vbo) {
    glBegin(GL_LINES);
    // Start point at vertex position
    glVertex3f(vertex.position.x, vertex.position.y, vertex.position.z);
    // End point in normal direction
    glVertex3f(vertex.position.x + vertex.normal.x,
               vertex.position.y + vertex.normal.y,
               vertex.position.z + vertex.normal.z);
    glEnd();
  }

  glEnable(GL_LIGHTING);
}

/**
 * Get raw vertex data
 */
std::vector<Vertex> Model::getPoints() { return this->_points; }