#ifndef CONE_HPP
#define CONE_HPP

#include <cmath>
#include <fstream>
#include <iostream>
#include <vector>

#include "utils.hpp"

bool generateConeTriangles(float radius, float height, int slices, int stacks,
                           const char* filepath, bool advanced);

#endif  // CONE_HPP