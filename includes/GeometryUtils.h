#ifndef GEOMETRYUTILS_H
#define GEOMETRYUTILS_H

#include "GeometryRenderer.h"
#include <vector>
#include <glm/glm.hpp>

namespace GeometryUtils 
{
    struct SphereGeometry 
    {
        std::vector<GeometryConfig::Vertex> vertices;
        std::vector<unsigned int> indices;
    };

    SphereGeometry GenerateSphere(float radius, int sectors, int stacks);
}

#endif
