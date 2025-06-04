#ifndef FILEPARSER_HPP
#define FILEPARSER_HPP

#include <rapidxml-1.13/rapidxml.hpp>
#include <string>

#include "Configuration.hpp"
#include "catmullCurves.hpp"

Configuration parseConfig(std::string filename);

Configuration parseConfig3D(std::string filename);

Configuration parseConfigObj(std::string filename);

ModelGroup parseGroup(rapidxml::xml_node<>* groupNode);

void parseTransform(rapidxml::xml_node<>* transformNode,
                    ModelGroup& modelGroup);

void parseModels(rapidxml::xml_node<>* modelsNode, ModelGroup& modelGroup);

#endif