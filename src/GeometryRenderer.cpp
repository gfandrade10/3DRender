#include "GeometryRenderer.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext.hpp>
#include <vector>
#include <iostream>

void GeometryRenderer::Initialize(const GeometryConfig& config) 
{
    //setting identity as default
    m_projection = glm::mat4(1.0f);
    m_view = glm::mat4(1.0f);
    m_model = glm::mat4(1.0f);

    // Create shader program
    GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertShader, 1, &config.vertexShader, nullptr);
    glCompileShader(vertShader);

    // Check vertex shader compilation
    GLint success;
    glGetShaderiv(vertShader, GL_COMPILE_STATUS, &success);
    if (!success) 
    {
        char infoLog[512];
        glGetShaderInfoLog(vertShader, 512, NULL, infoLog);
        std::cerr << "Vertex Shader Error:\n" << infoLog << std::endl;
        return; // Exit early on error
    }

    // Compile fragment shader
    GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragShader, 1, &config.fragmentShader, nullptr);
    glCompileShader(fragShader);

    // Check fragment shader compilation
    glGetShaderiv(fragShader, GL_COMPILE_STATUS, &success);
    if (!success) 
    {
        char infoLog[512];
        glGetShaderInfoLog(fragShader, 512, NULL, infoLog);
        std::cerr << "Fragment Shader Error:\n" << infoLog << std::endl;
        return; // Exit early on error
    }

    // Link program
    m_shader = glCreateProgram();
    glAttachShader(m_shader, vertShader);
    glAttachShader(m_shader, fragShader);
    glLinkProgram(m_shader);

    // Check program linking
    glGetProgramiv(m_shader, GL_LINK_STATUS, &success);
    if (!success) 
    {
        char infoLog[512];
        glGetProgramInfoLog(m_shader, 512, NULL, infoLog);
        std::cerr << "Shader Link Error:\n" << infoLog << std::endl;
        return; // Exit early on error
    }

    // Clean up shaders
    glDeleteShader(vertShader);
    glDeleteShader(fragShader);

    // Create buffers
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ebo);

    glBindVertexArray(m_vao);
    
    // Vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, 
                config.vertices.size() * sizeof(GeometryConfig::Vertex),
                config.vertices.data(), 
                GL_STATIC_DRAW);

    // Vertex attributes
    // Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 
                         sizeof(GeometryConfig::Vertex),
                         (void*)offsetof(GeometryConfig::Vertex, position));
    // Normal
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
                         sizeof(GeometryConfig::Vertex),
                         (void*)offsetof(GeometryConfig::Vertex, normal));
    // TexCoord
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE,
                         sizeof(GeometryConfig::Vertex),
                         (void*)offsetof(GeometryConfig::Vertex, texCoord));

    // Element buffer
    if(!config.indices.empty()) 
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                    config.indices.size() * sizeof(unsigned int),
                    config.indices.data(),
                    GL_STATIC_DRAW);
        m_elementCount = config.indices.size();
    }
    else
        m_elementCount = config.vertices.size();

    m_drawMode = config.drawMode;
    glBindVertexArray(0);

    glGenFramebuffers(1, &m_FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

    // Create texture to render into
    glGenTextures(1, &m_RenderTexture);
    glBindTexture(GL_TEXTURE_2D, m_RenderTexture);
    // Allocate texture storage (initial size; you can resize on demand)
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, config.initialWidth, config.initialHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Attach texture to framebuffer as color attachment
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_RenderTexture, 0);

    // Create renderbuffer for depth and stencil if required
    glGenRenderbuffers(1, &m_RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, m_RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, config.initialWidth, config.initialHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RBO);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cerr << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

    // Unbind framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GeometryRenderer::SetProjection(const glm::mat4& projection) 
{
    m_projection = projection;
}

void GeometryRenderer::SetView(const glm::mat4& view) 
{
    m_view = view;
}

void GeometryRenderer::SetTransform(const glm::mat4& model) 
{
    m_model = model;
}

void GeometryRenderer::BeginRenderToTexture(int width, int height)
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
    glViewport(0, 0, width, height);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GeometryRenderer::EndRenderToTexture()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLuint GeometryRenderer::GetRenderTexture() const 
{ 
    return m_RenderTexture; 
}

void GeometryRenderer::Render()
{
    glUseProgram(m_shader);

    GLint projLoc = glGetUniformLocation(m_shader, "uProjection");
    GLint viewLoc = glGetUniformLocation(m_shader, "uView");
    GLint modelLoc = glGetUniformLocation(m_shader, "uModel");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(m_model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(m_view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(m_projection));

    GLint lightDirLoc = glGetUniformLocation(m_shader, "lightDir");
    GLint lightColorLoc = glGetUniformLocation(m_shader, "lightColor");
    GLint objectColorLoc = glGetUniformLocation(m_shader, "objectColor");

    glm::vec3 lightDir = glm::normalize(glm::vec3(0.0f, -1.0f, -1.0f));
    glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDir));
    glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f); // White light
    glUniform3f(objectColorLoc, 1.0f, 1.0f, 1.0f); // White object

    glm::vec3 viewPos = glm::vec3(0.0f, 0.0f, 3.0f); // Assuming your view position is (0, 0, 3)
    GLint viewPosLoc = glGetUniformLocation(m_shader, "viewPos");
    glUniform3fv(viewPosLoc, 1, glm::value_ptr(viewPos));

    // Draw
    glBindVertexArray(m_vao);
    glDrawElements(m_drawMode, m_elementCount, GL_UNSIGNED_INT, 0);
}

GLuint GeometryRenderer::GetShaderProgram() const
{
    return m_shader;
}
