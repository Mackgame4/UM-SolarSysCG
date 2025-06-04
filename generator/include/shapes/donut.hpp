#ifndef DONUT_HPP
#define DONUT_HPP

#include <string>
#include <vector>

#include "utils.hpp"

bool generateDonutTriangles(float majorRadius, float minorRadius, int sides,
                            int rings, const char* filepath, bool advanced);

#endif  // DONUT_HPP