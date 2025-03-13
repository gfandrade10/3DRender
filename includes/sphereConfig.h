#ifndef SPHERECONFIG_H
#define SPHERECONFIG_H

#include "GeometryRenderer.h"
#include "GeometryUtils.h"

// Function to create a sphere configuration
inline GeometryConfig createSphereConfig() 
{
    // Generate sphere geometry data
    auto sphere = GeometryUtils::GenerateSphere(1.0f, 128, 128);

    GeometryConfig config;
    config.vertices = sphere.vertices,
    config.indices = sphere.indices,
    
    // Vertex shader: accepts texture coordinates and passes them to the fragment shader
    config.vertexShader = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec3 aNormal;
        layout (location = 2) in vec2 aTexCoord;

        uniform mat4 uProjection;
        uniform mat4 uView;
        uniform mat4 uModel;

        out vec3 FragPos;
        out vec3 Normal;
        out vec2 TexCoord;

        void main() {
            FragPos = vec3(uModel * vec4(aPos, 1.0));
            Normal = mat3(transpose(inverse(uModel))) * aNormal;
            TexCoord = aTexCoord;
            gl_Position = uProjection * uView * vec4(FragPos, 1.0);
        }
    )",

    // Fragment shader: includes a sampler2D uniform "diffuseTexture" and adds an ambient term
    config.fragmentShader = R"(
        #version 330 core
        in vec3 FragPos;
        in vec3 Normal;
        in vec2 TexCoord;
        out vec4 FragColor;

        uniform sampler2D diffuseTexture;
        uniform vec3 lightDir;
        uniform vec3 lightColor;
        uniform vec3 objectColor;
        uniform vec3 viewPos; // Add view position uniform

        void main() {
            vec3 norm = normalize(Normal);
            vec3 lightDirNorm = normalize(-lightDir);
            vec3 viewDir = normalize(viewPos - FragPos);
            vec3 reflectDir = reflect(-lightDirNorm, norm);

            // Diffuse
            float diff = max(dot(norm, lightDirNorm), 0.0);
            vec3 diffuse = diff * lightColor;

            // Specular
            float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32); // Adjust the exponent for shininess
            vec3 specular = spec * lightColor;

            // Ambient
            vec3 ambient = 0.2 * lightColor; // Reduced ambient

            vec3 lighting = ambient + diffuse + specular; // Add specular

            vec4 texColor = texture(diffuseTexture, TexCoord);
            FragColor = texColor * vec4(objectColor * lighting, 1.0);
        }
    )",

    //initial screen size
    config.initialWidth = 1920,
    config.initialHeight = 1080,

    // Set the draw mode (GL_TRIANGLES in this case)
    config.drawMode = GL_TRIANGLES;

    return config;
}

// Declare the sphere configuration as an inline constant to ensure proper linkage
inline const GeometryConfig sphereConfig = createSphereConfig();

#endif
