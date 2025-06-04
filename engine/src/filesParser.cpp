#include "filesParser.hpp"

#include <fmt/core.h>

#include <filesystem>

#include "readFile.hpp"

Configuration parseConfig3D(std::string inputFile) {
  Configuration config = parseConfig("scenes/default.xml");
  ModelGroup& modelGrp = config.modelGroup;
  modelGrp.models.clear();
  modelGrp.subModelgroups.clear();
  Model threeDModel = readFile(inputFile.data());
  if (threeDModel.id == -1) {
    std::cerr << "Error reading model file: " << inputFile << std::endl;
    return config;
  }
  modelGrp.models.push_back(threeDModel);
  return config;
}

Configuration parseConfigObj(std::string objFile) {
  Configuration configObj = parseConfig("scenes/default.xml");
  ModelGroup& objGroup = configObj.modelGroup;
  objGroup.models.clear();
  objGroup.subModelgroups.clear();
  Model objectModel = readFile(objFile.data());
  if (objectModel.id == -1) {
    std::cerr << "Error reading model file: " << objFile << std::endl;
    return configObj;
  }
  objGroup.models.push_back(objectModel);
  return configObj;
}

Configuration parseConfig(std::string configFile) {
  std::ifstream configStream(configFile);

  std::cout << "Current directory: " << std::filesystem::current_path()
            << std::endl;
  std::cout << "File path: " << configFile << std::endl;

  if (!configStream.is_open()) {
    std::cerr << "Error opening the configuration file!" << std::endl;
    exit(1);
  }

  printf("Processing configuration file: %s\n", configFile.c_str());

  std::string xmlData((std::istreambuf_iterator<char>(configStream)),
                      std::istreambuf_iterator<char>());
  configStream.close();

  rapidxml::xml_document<> xmlDoc;
  xmlDoc.parse<0>(&xmlData[0]);

  rapidxml::xml_node<>* rootNode = xmlDoc.first_node("world");

  char* winWidth =
      rootNode->first_node("window")->first_attribute("width")->value();
  char* winHeight =
      rootNode->first_node("window")->first_attribute("height")->value();

  Window displaySettings = Window(std::stoi(winWidth), std::stoi(winHeight));

  rapidxml::xml_node<>* camNode = rootNode->first_node("camera");
  rapidxml::xml_node<>* posNode = camNode->first_node("position");
  glm::vec3 camPosition =
      glm::vec3(std::stof(posNode->first_attribute("x")->value()),
                std::stof(posNode->first_attribute("y")->value()),
                std::stof(posNode->first_attribute("z")->value()));

  rapidxml::xml_node<>* viewNode = camNode->first_node("lookAt");
  glm::vec3 viewDirection =
      glm::vec3(std::stof(viewNode->first_attribute("x")->value()),
                std::stof(viewNode->first_attribute("y")->value()),
                std::stof(viewNode->first_attribute("z")->value()));

  rapidxml::xml_node<>* upVecNode = camNode->first_node("up");
  glm::vec3 upVector =
      glm::vec3(std::stof(upVecNode->first_attribute("x")->value()),
                std::stof(upVecNode->first_attribute("y")->value()),
                std::stof(upVecNode->first_attribute("z")->value()));

  rapidxml::xml_node<>* projNode = camNode->first_node("projection");
  int fieldOfView = std::stoi(projNode->first_attribute("fov")->value());
  float nearClip = std::stof(projNode->first_attribute("near")->value());
  float farClip = std::stof(projNode->first_attribute("far")->value());

  Camera camSettings = Camera(camPosition, viewDirection, upVector, fieldOfView,
                              nearClip, farClip);

  std::vector<Light> lightSources;
  rapidxml::xml_node<>* lightsNode = rootNode->first_node("lights");
  if (lightsNode) {
    for (rapidxml::xml_node<>* lightNode = lightsNode->first_node("light");
         lightNode && lightSources.size() < 8;
         lightNode = lightNode->next_sibling("light")) {
      switch (lightNode->first_attribute("type")->value()[0]) {
        case 'p': {
          float posX = std::stof(lightNode->first_attribute("posx")->value());
          float posY = std::stof(lightNode->first_attribute("posy")->value());
          float posZ = std::stof(lightNode->first_attribute("posz")->value());
          Light pointLight = createPointLight(glm::vec4(posX, posY, posZ, 1));
          lightSources.push_back(pointLight);
        } break;
        case 'd': {
          float dirX = std::stof(lightNode->first_attribute("dirx")->value());
          float dirY = std::stof(lightNode->first_attribute("diry")->value());
          float dirZ = std::stof(lightNode->first_attribute("dirz")->value());
          Light dirLight = createDirectionLight(glm::vec4(dirX, dirY, dirZ, 1));
          lightSources.push_back(dirLight);
        } break;
        case 's': {
          float spotX = std::stof(lightNode->first_attribute("posx")->value());
          float spotY = std::stof(lightNode->first_attribute("posy")->value());
          float spotZ = std::stof(lightNode->first_attribute("posz")->value());
          float spotDirX =
              std::stof(lightNode->first_attribute("dirx")->value());
          float spotDirY =
              std::stof(lightNode->first_attribute("diry")->value());
          float spotDirZ =
              std::stof(lightNode->first_attribute("dirz")->value());
          float cutOff =
              std::stof(lightNode->first_attribute("cutoff")->value());
          Light spotLight = createSpotLight(
              glm::vec4(spotX, spotY, spotZ, 1),
              glm::vec4(spotDirX, spotDirY, spotDirZ, 1), cutOff);
          lightSources.push_back(spotLight);
        } break;
        default:
          break;
      }
    }
  }

  rapidxml::xml_node<>* mainGroup = rootNode->first_node("group");

  ModelGroup sceneGroup = parseGroup(mainGroup);

  return Configuration(displaySettings, camSettings, sceneGroup, lightSources);
}

ModelGroup parseGroup(rapidxml::xml_node<>* groupElement) {
  ModelGroup sceneGroup;

  rapidxml::xml_node<>* transformElement =
      groupElement->first_node("transform");
  if (transformElement) {
    parseTransform(transformElement, sceneGroup);
  }

  rapidxml::xml_node<>* modelsElement = groupElement->first_node("models");
  if (modelsElement) {
    parseModels(modelsElement, sceneGroup);
  }

  rapidxml::xml_node<>* subGroups = groupElement->first_node("group");
  while (subGroups) {
    ModelGroup childGroup = parseGroup(subGroups);
    sceneGroup.subModelgroups.push_back(childGroup);
    subGroups = subGroups->next_sibling("group");
  }

  return sceneGroup;
}

void parseTransform(rapidxml::xml_node<>* transformElement,
                    ModelGroup& targetGroup) {
  bool isStatic = false;
  int staticIndex = -1;

  for (rapidxml::xml_node<>* transformNode = transformElement->first_node();
       transformNode; transformNode = transformNode->next_sibling()) {
    std::string transformType = transformNode->name();
    if (transformType == "scale") {
      float scaleX = std::stof(transformNode->first_attribute("x")->value());
      float scaleY = std::stof(transformNode->first_attribute("y")->value());
      float scaleZ = std::stof(transformNode->first_attribute("z")->value());
      glm::mat4 scaleMatrix = Scalematrix(scaleX, scaleY, scaleZ);
      if (isStatic) {
        targetGroup.static_transformations[staticIndex] =
            targetGroup.static_transformations[staticIndex] * scaleMatrix;
      } else {
        targetGroup.order.push_back(STATIC);
        targetGroup.static_transformations.push_back(scaleMatrix);
        isStatic = true;
        staticIndex++;
      }

    } else if (transformType == "rotate") {
      if (transformNode->first_attribute("time")) {
        float rotationTime =
            std::stof(transformNode->first_attribute("time")->value());
        float rotX = std::stof(transformNode->first_attribute("x")->value());
        float rotY = std::stof(transformNode->first_attribute("y")->value());
        float rotZ = std::stof(transformNode->first_attribute("z")->value());
        TimeRotations timedRotation = TimeRotations(
            float(rotationTime), float(rotX), float(rotY), float(rotZ));
        targetGroup.rotations.push_back(timedRotation);
        targetGroup.order.push_back(TIMEROTATION);
        isStatic = false;

      } else {
        float rotationAngle =
            std::stof(transformNode->first_attribute("angle")->value());
        float axisX = std::stof(transformNode->first_attribute("x")->value());
        float axisY = std::stof(transformNode->first_attribute("y")->value());
        float axisZ = std::stof(transformNode->first_attribute("z")->value());
        glm::mat4 rotationMatrix =
            Rotationmatrix(rotationAngle, axisX, axisY, axisZ);
        if (isStatic) {
          targetGroup.static_transformations[staticIndex] =
              targetGroup.static_transformations[staticIndex] * rotationMatrix;
        } else {
          targetGroup.order.push_back(STATIC);
          targetGroup.static_transformations.push_back(rotationMatrix);
          isStatic = true;
          staticIndex++;
        }
      }

    } else if (transformType == "translate") {
      if (transformNode->first_attribute("time")) {
        float translationTime =
            std::stof(transformNode->first_attribute("time")->value());

        bool shouldAlign = true;
        if (transformNode->first_attribute("align")) {
          shouldAlign =
              std::string(transformNode->first_attribute("align")->value()) ==
              "true";
        }
        std::vector<Point> translationPath;
        while (transformNode->first_node("point")) {
          rapidxml::xml_node<>* pathPoint = transformNode->first_node("point");
          float pointX = std::stof(pathPoint->first_attribute("x")->value());
          float pointY = std::stof(pathPoint->first_attribute("y")->value());
          float pointZ = std::stof(pathPoint->first_attribute("z")->value());
          translationPath.push_back(Point(pointX, pointY, pointZ));
          transformNode->remove_node(pathPoint);
        }
        TimeTranslations pathTranslation = TimeTranslations(
            float(translationTime), shouldAlign, translationPath);
        targetGroup.translates.push_back(pathTranslation);
        targetGroup.order.push_back(TIMETRANSLATE);
        isStatic = false;

      } else {
        float transX = std::stof(transformNode->first_attribute("x")->value());
        float transY = std::stof(transformNode->first_attribute("y")->value());
        float transZ = std::stof(transformNode->first_attribute("z")->value());
        glm::mat4 translationMatrix = Translatematrix(transX, transY, transZ);
        if (isStatic) {
          targetGroup.static_transformations[staticIndex] =
              targetGroup.static_transformations[staticIndex] *
              translationMatrix;
        } else {
          targetGroup.order.push_back(STATIC);
          targetGroup.static_transformations.push_back(translationMatrix);
          isStatic = true;
          staticIndex++;
        }
      }
    }
  }
}

void parseModels(rapidxml::xml_node<>* modelsElement, ModelGroup& targetGroup) {
  rapidxml::xml_node<>* modelElement = modelsElement->first_node("model");
  while (modelElement) {
    const std::string& modelFile =
        modelElement->first_attribute("file")->value();
    Model sceneModel = readFile(modelFile.data());
    if (sceneModel.id == -1) {
      std::cerr << "Error reading model file: " << modelFile << std::endl;
      return;
    }
    rapidxml::xml_node<>* textureElement = modelElement->first_node("texture");

    if (textureElement) {
      sceneModel.texture_filepath =
          textureElement->first_attribute("file")->value();
    } else {
      sceneModel.texture_filepath = "";
    }

    rapidxml::xml_node<>* colorElement = modelElement->first_node("color");

    glm::vec4 diffuseColor = glm::vec4(0.8f, 0.8f, 0.8f, 1.0f);
    glm::vec4 ambientColor = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
    glm::vec4 specularColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    glm::vec4 emissiveColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    float shininessValue = 0.0f;

    if (colorElement) {
      rapidxml::xml_node<>* diffuseNode = colorElement->first_node("diffuse");
      if (diffuseNode) {
        diffuseColor = glm::vec4(
            std::stof(diffuseNode->first_attribute("R")->value()) / 255.0f,
            std::stof(diffuseNode->first_attribute("G")->value()) / 255.0f,
            std::stof(diffuseNode->first_attribute("B")->value()) / 255.0f,
            1.0f);
      }

      rapidxml::xml_node<>* ambientNode = colorElement->first_node("ambient");
      if (ambientNode) {
        ambientColor = glm::vec4(
            std::stof(ambientNode->first_attribute("R")->value()) / 255.0f,
            std::stof(ambientNode->first_attribute("G")->value()) / 255.0f,
            std::stof(ambientNode->first_attribute("B")->value()) / 255.0f,
            1.0f);
      }

      rapidxml::xml_node<>* specularNode = colorElement->first_node("specular");
      if (specularNode) {
        specularColor = glm::vec4(
            std::stof(specularNode->first_attribute("R")->value()) / 255.0f,
            std::stof(specularNode->first_attribute("G")->value()) / 255.0f,
            std::stof(specularNode->first_attribute("B")->value()) / 255.0f,
            1.0f);
      }

      rapidxml::xml_node<>* emissiveNode = colorElement->first_node("emissive");
      if (emissiveNode) {
        emissiveColor = glm::vec4(
            std::stof(emissiveNode->first_attribute("R")->value()) / 255.0f,
            std::stof(emissiveNode->first_attribute("G")->value()) / 255.0f,
            std::stof(emissiveNode->first_attribute("B")->value()) / 255.0f,
            1.0f);
      }

      rapidxml::xml_node<>* shininessNode =
          colorElement->first_node("shininess");
      if (shininessNode) {
        shininessValue =
            std::stof(shininessNode->first_attribute("value")->value());
      }
    }

    Material modelMaterial =
        createMaterial(ambientColor, diffuseColor, specularColor, emissiveColor,
                       shininessValue);

    sceneModel.material = modelMaterial;

    targetGroup.models.push_back(sceneModel);
    modelElement = modelElement->next_sibling("model");
  }
}