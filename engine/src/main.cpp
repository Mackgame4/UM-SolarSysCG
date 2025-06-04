#include <GL/glew.h>
extern "C" {
#include <GL/gl.h>
#ifdef __APPLE_CC__
#include <GLUT/glut.h>
#else
#include <GL/freeglut.h>
#endif
}

#define _USE_MATH_DEFINES
#include <math.h>

#include <chrono>
#include <unordered_map>

#include "Configuration.hpp"
#include "cameraController.hpp"
#include "catmullCurves.hpp"
#include "filesParser.hpp"
#include "menuGUI.hpp"
#include "process_input.hpp"

// Global scene configuration variables
std::string sceneFile;
bool basicMode = false;

// Display settings
bool showAxes = true;
bool wireframeMode = false;
bool showNormals = false;
bool backfaceCulling = false;
bool enableLighting = false;
float animationSpeed = 1.0f;
bool showModelDetails = false;
bool showUI = false;

// Statistics tracking
int modelCountTotal = 0;
int modelCountVisible = 0;

// Performance tracking
int timeStart;
float frameCount;

// Scene data
Configuration sceneConfig;
Camera mainCamera;

/**
 * Structure to hold mesh statistics for display in UI
 */
struct MeshStats {
  int vertexCount;    // Number of unique vertices
  int triangleCount;  // Number of triangles in the mesh
};

// Map of model filenames to their statistics
std::unordered_map<std::string, MeshStats> modelStatistics;

/**
 * Handles window resize events and updates the projection matrix
 *
 * @param width New window width
 * @param height New window height
 */
void windowResize(int width, int height) {
  // Calculate aspect ratio for proper perspective
  float aspect = static_cast<float>(width) / static_cast<float>(height);

  // Update viewport to match new window dimensions
  glViewport(0, 0, width, height);

  // Update projection matrix
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(sceneConfig.camera.fov, aspect, sceneConfig.camera.nearPlane,
                 sceneConfig.camera.farPlane);

  // Return to modelview mode
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  // Update ImGui for the new dimensions
  ImGui_ImplGLUT_ReshapeFunc(width, height);
}

/**
 * Renders the world coordinate system axes
 */
void renderCoordinateSystem() {
  if (showAxes) {
    // Temporarily disable lighting for axes
    glDisable(GL_LIGHTING);

    glBegin(GL_LINES);
    // X axis (red)
    glColor3f(50.0f, 0.0f, 0.0f);
    glVertex3f(-500.0f, 0.0f, 0.0f);
    glVertex3f(500.0f, 0.0f, 0.0f);

    // Y axis (green)
    glColor3f(0.0f, 50.0f, 0.0f);
    glVertex3f(0.0f, -500.0f, 0.0f);
    glVertex3f(0.0f, 500.0f, 0.0f);

    // Z axis (blue)
    glColor3f(0.0f, 0.0f, 50.0f);
    glVertex3f(0.0f, 0.0f, -500.0f);
    glVertex3f(0.0f, 0.0f, 500.0f);

    // Reset color
    glColor3f(1.0f, 1.0f, 1.0f);
    glEnd();

    // Re-enable lighting if there are lights configured
    if (sceneConfig.lights.size() != 0) {
      glEnable(GL_LIGHTING);
    }
  }
}

/**
 * Initializes the scene from a file
 *
 * @param filePath Path to the scene description file
 */
void initializeScene(char* filePath) {
  sceneFile.assign(filePath);

  // Parse scene based on file extension
  if (sceneFile.substr(sceneFile.size() - 4) == ".xml") {
    sceneConfig = parseConfig(sceneFile);
  } else if (sceneFile.substr(sceneFile.size() - 3) == ".3d") {
    sceneConfig = parseConfig3D(sceneFile);
  } else if (sceneFile.substr(sceneFile.size() - 4) == ".obj") {
    sceneConfig = parseConfigObj(sceneFile);
  } else {
    std::cout << "Unsupported file format: " << sceneFile << std::endl;
    std::cout << "Supported formats: .xml, .3d, .obj" << std::endl;
    exit(1);
  }

  // Initialize camera from scene configuration
  mainCamera = sceneConfig.camera;
}

/**
 * Recursively prepares all models in the model group hierarchy
 *
 * @param modelCollection Model group to process
 */
void prepareModels(ModelGroup& modelCollection) {
  // Process all models in the current group
  for (Model& mesh : modelCollection.models) {
    modelCountTotal++;
    mesh.initModel();

    // Store statistics for UI display
    MeshStats stats = {
        static_cast<int>(mesh.vbo.size()),     // Vertex count
        static_cast<int>(mesh.ibo.size() / 3)  // Triangle count (indices / 3)
    };
    modelStatistics[mesh.filename] = stats;
  }

  // Recursively process all subgroups
  for (ModelGroup& subgroup : modelCollection.subModelgroups) {
    prepareModels(subgroup);
  }
}

/**
 * Reloads the scene from the current scene file
 */
void reloadScene() {
  // Reset statistics
  modelCountTotal = 0;
  modelStatistics.clear();

  // Re-initialize scene and models
  initializeScene(const_cast<char*>(sceneFile.c_str()));
  prepareModels(sceneConfig.modelGroup);
  enableLighting = setupLights(sceneConfig.lights);
}

/**
 * Restores the camera to its initial configuration
 */
void restoreCamera() {
  mainCamera = sceneConfig.camera;
  animationSpeed = 1.0f;
}

/**
 * Renders the ImGui-based user interface
 */
void displayUI() {
  // Initialize new frame
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGLUT_NewFrame();
  ImGui::NewFrame();

  ImGuiIO& guiState = ImGui::GetIO();

  // Main information panel
  {
    ImGui::Begin("Information Panel", NULL, ImGuiWindowFlags_AlwaysAutoResize);

    // Performance information
    ImGui::Text("FPS: %.1f (%.3f ms/frame)", guiState.Framerate,
                1000.f / guiState.Framerate);

    // Camera information
    ImGui::Text("Camera Position: (%.3f, %.3f, %.3f)", mainCamera.position.x,
                mainCamera.position.y, mainCamera.position.z);

    // View frustum information
    ImGui::Text("FOV: %d Aspect Ratio: %.1f Near: %.3f Far: %.3f",
                sceneConfig.camera.fov,
                static_cast<float>(glutGet(GLUT_WINDOW_WIDTH)) /
                    static_cast<float>(glutGet(GLUT_WINDOW_HEIGHT)),
                sceneConfig.camera.nearPlane, sceneConfig.camera.farPlane);

    // Scene information
    ImGui::Text("Scene File: %s", sceneFile.c_str());
    ImGui::Text("Models: %d (Total %d)", modelCountVisible, modelCountTotal);

    // Toggle model statistics panel
    ImGui::Checkbox("Model Statistics", &showModelDetails);
    ImGui::End();
  }

  // Display settings panel
  {
    ImGui::Begin("Display Settings", NULL, ImGuiWindowFlags_AlwaysAutoResize);

    // Rendering options
    ImGui::Checkbox("Show Axes", &showAxes);
    ImGui::SameLine();
    ImGui::Checkbox("Wireframe Mode", &wireframeMode);
    ImGui::SameLine();
    ImGui::Checkbox("Show Normals", &showNormals);
    ImGui::SameLine();
    ImGui::Checkbox("Enable Lighting", &enableLighting);

    // Animation controls
    ImGui::SliderFloat("Animation Speed", &animationSpeed, 0.0f, 2.0f);

    // Reset and reload buttons
    ImGui::Button("Reset View", ImVec2(80, 20));
    if (ImGui::IsItemClicked()) {
      restoreCamera();
    }
    ImGui::SameLine();
    ImGui::Button("Reload Scene", ImVec2(100, 20));
    if (ImGui::IsItemClicked()) {
      reloadScene();
    }
    ImGui::End();
  }

  // Optional model details panel
  if (showModelDetails) {
    ImGui::Begin("Model Details", NULL, ImGuiWindowFlags_AlwaysAutoResize);
    for (const auto& model : modelStatistics) {
      ImGui::Text("Model: %s", model.first.c_str());
      ImGui::Text("Vertices: %d Triangles: %d", model.second.vertexCount,
                  model.second.triangleCount);
      ImGui::Separator();
    }
    ImGui::End();
  }

  // Render ImGui
  ImGui::Render();
  glViewport(0, 0, (GLsizei)guiState.DisplaySize.x,
             (GLsizei)guiState.DisplaySize.y);
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

/**
 * Sets polygon rendering mode based on current settings
 */
void setRenderMode() {
  if (wireframeMode) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  } else {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }
}

/**
 * Calculates time elapsed since last frame
 *
 * @return Time elapsed in seconds
 */
inline float calculateFrameTime() {
  static auto previousTime = std::chrono::high_resolution_clock::now();
  const auto currentTime = std::chrono::high_resolution_clock::now();

  // Calculate elapsed time in milliseconds
  const auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(
                             currentTime - previousTime)
                             .count();

  previousTime = currentTime;

  // Return time in seconds
  return elapsedMs / 1000.0f;
}

/**
 * Updates scene state based on user input
 */
void updateScene() {
  // Process input events
  Input::process_input();

  // Update camera using controller
  static CameraController camControl(mainCamera);
  float deltaTime = calculateFrameTime();
  camControl.update(deltaTime, showUI);
}

/**
 * Main render function called each frame
 */
void renderFrame() {
  // Update scene logic
  updateScene();

  // Clear buffers
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();

  // Set up camera view
  glm::vec3 viewTarget = mainCamera.position + mainCamera.forward;
  gluLookAt(mainCamera.position.x, mainCamera.position.y, mainCamera.position.z,
            viewTarget.x, viewTarget.y, viewTarget.z, mainCamera.up.x,
            mainCamera.up.y, mainCamera.up.z);

  // Configure rendering mode
  setRenderMode();

  // Render coordinate axes
  renderCoordinateSystem();

  // Draw lights if enabled
  if (enableLighting) {
    drawLights(sceneConfig.lights);
  }

  // Draw all models in the scene
  modelCountVisible = 0;
  sceneConfig.modelGroup.drawGroup(enableLighting, showNormals, animationSpeed,
                                   modelCountVisible);

  // Draw UI if enabled
  if (showUI) {
    displayUI();
  }

  // Swap buffers and request next frame
  glutSwapBuffers();
  glutPostRedisplay();
}

/**
 * Parse command line arguments
 *
 * @param argCount Number of arguments
 * @param argValues Argument values
 */
void parseArguments(int argCount, char** argValues) {
  for (int i = 2; i < argCount; i++) {
    if (strcmp(argValues[i], "-s") == 0) {
      basicMode = true;
    }
  }
}

/**
 * Main program entry point
 *
 * @param argc Number of command line arguments
 * @param argv Command line arguments
 * @return Exit code
 */
int main(int argc, char** argv) {
  // Validate command line arguments
  if (argc == 1) {
    std::cout << "Invalid Parameters\n";
    std::cout << "Usage: ./build/engine/Debug/engine <scene_file> [options]\n";
    std::cout << "Options:\n";
    std::cout << "  -s    Basic mode (simplified rendering)\n";
    return 1;
  }

  // Initialize scene from file
  initializeScene(argv[1]);

  // Parse additional command line arguments
  parseArguments(argc, argv);

  // Initialize GLUT
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
  glutInitWindowPosition(100, 100);
  glutInitWindowSize(sceneConfig.window.width, sceneConfig.window.height);
  glutCreateWindow("3D Scene Renderer");

  // Initialize GLEW and OpenGL state
  glewInit();
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_NORMAL_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);

  // Initialize menu system
  setupMenu();

  // Register GLUT callbacks
  glutDisplayFunc(renderFrame);
  glutReshapeFunc(windowResize);

  // Configure input handling
  glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF);
  glutKeyboardFunc(&Input::on_key_down);
  glutKeyboardUpFunc(&Input::on_key_up);
  glutSpecialFunc(&Input::on_special_key_down);
  glutSpecialUpFunc(&Input::on_special_key_up);
  glutMouseFunc(&Input::on_mouse_button);
  glutMotionFunc(&Input::on_mouse_motion);
  glutPassiveMotionFunc(&Input::on_mouse_motion);

  // Enable OpenGL features
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glEnable(GL_TEXTURE_2D);

  // Set up scene
  enableLighting = setupLights(sceneConfig.lights);
  prepareModels(sceneConfig.modelGroup);

  // Enter main loop
  glutMainLoop();

  // Clean up resources
  shutDownMenu();

  return 0;
}