#ifndef GEOMETRYRENDERER_H
#define GEOMETRYRENDERER_H

#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct GeometryConfig 
{
    struct Vertex
    {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texCoord;
    };

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    const char* vertexShader;
    const char* fragmentShader;
    unsigned int initialWidth;
    unsigned int initialHeight;
    GLenum drawMode = GL_TRIANGLES;
};

class GeometryRenderer 
{
    public:
    GeometryRenderer() = default;
    
    void Initialize(const GeometryConfig& config);
    void SetProjection(const glm::mat4& projection);
    void SetView(const glm::mat4& view);
    void SetTransform(const glm::mat4& model);
    void BeginRenderToTexture(int width, int height);
    void EndRenderToTexture();
    void Render();

    GLuint GetShaderProgram() const;
    GLuint GetRenderTexture() const;

    private:
    GLuint m_vao = 0;
    GLuint m_vbo = 0;
    GLuint m_ebo = 0;
    GLuint m_shader = 0;
    GLuint m_FBO = 0;
    GLuint m_RenderTexture = 0;
    GLuint m_RBO = 0;

    GLenum m_drawMode;

    size_t m_elementCount = 0;
    
    glm::mat4 m_projection;
    glm::mat4 m_view;
    glm::mat4 m_model;
};

#endif
