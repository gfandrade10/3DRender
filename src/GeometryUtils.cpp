#include "GeometryUtils.h"
#include "glm/ext/scalar_constants.hpp"

namespace GeometryUtils
{
    SphereGeometry GenerateSphere(float radius, int sectors, int stacks) 
    {  
        SphereGeometry geometry;

        const float PI = glm::pi<float>();
        const float TAU = 2.0f * PI;

        // Generate vertices
        for(int stack = 0; stack <= stacks; ++stack) 
        {
            float phi = PI/2 - stack * PI / stacks; // From -π/2 to π/2
            float y = radius * sin(phi);
            
            for(int sector = 0; sector <= sectors; ++sector) 
            {
                float theta = sector * TAU / sectors; // From 0 to 2π
                float x = radius * cos(phi) * cos(theta);
                float z = radius * cos(phi) * sin(theta);

                GeometryConfig::Vertex vertex;
                vertex.position = glm::vec3(x, y, z);
                vertex.normal = glm::normalize(vertex.position);
                
                // Texture coordinates
                vertex.texCoord.x = static_cast<float>(sector) / sectors;
                vertex.texCoord.y = static_cast<float>(stack) / stacks;
                
                geometry.vertices.push_back(vertex);
            }
        }

        // Generate indices
        for(int stack = 0; stack < stacks; ++stack) 
        {
            int k1 = stack * (sectors + 1);
            int k2 = k1 + sectors + 1;

            for(int sector = 0; sector < sectors; ++sector, ++k1, ++k2) 
            {
                // 2 triangles per sector
                if(stack != 0) 
                {
                    geometry.indices.insert(geometry.indices.end(), 
                    {
                        static_cast<unsigned int>(k1),
                        static_cast<unsigned int>(k2),
                        static_cast<unsigned int>(k1 + 1)
                    });
                }
                
                if(stack != stacks-1) 
                {
                    geometry.indices.insert(geometry.indices.end(), 
                    {
                        static_cast<unsigned int>(k1 + 1),
                        static_cast<unsigned int>(k2),
                        static_cast<unsigned int>(k2 + 1)
                    });
                }
            }
        }

        return geometry;
    }
}
