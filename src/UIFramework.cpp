#include <iostream>
#include <memory>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "UIFramework.h"
#include "GeometryRenderer.h"
#include "sphereConfig.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

UIFramework::~UIFramework()
{
    if (mWindow)
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        
        glfwDestroyWindow(mWindow);
        glfwTerminate();
    }
}

void UIFramework::Init(int width, int height, const char* title, GLFWmonitor* monitor, GLFWwindow* share)
{
    glfwInit();
    glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
    mWindow = glfwCreateWindow(width, height, title, monitor, share);
    glfwMakeContextCurrent(mWindow);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(1); // Enable vsync

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(mWindow, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    ImGuiIO& io = ImGui::GetIO();
    ImFont* font = io.Fonts->AddFontFromFileTTF("C:/Windows/Fonts/Calibri.ttf", 16.0f);
    if (!font) 
        std::cerr << "Failed to load font!" << std::endl;
    else
        io.Fonts->Build();
}

void UIFramework::Run() 
{
    // Create and initialize the renderer using your sphere configuration.
    std::unique_ptr<GeometryRenderer> renderer(new GeometryRenderer());
    renderer->Initialize(sphereConfig);
    
    // Set transformation matrices.
    {
        glm::mat4 model = glm::mat4(1.0f); // No translation
        //
        renderer->SetTransform(model);
        glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f),
                                     glm::vec3(0.0f, 0.0f, 0.0f),
                                     glm::vec3(0.0f, 1.0f, 0.0f));
        renderer->SetView(view);
    }

    // Load texture using stb_image. (This runs once before the loop.)
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    int width, height, nrChannels;
    unsigned char* data = stbi_load("earth.jpg", &width, &height, &nrChannels, 0);
    
    if (!data) 
    {
        std::cerr << "Failed to load texture! Using fallback." << std::endl;
        // Fallback: 1x1 bright pink texture.
        unsigned char pink[] = {255, 0, 255, 255};
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, pink);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    } 
    else 
    {
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        stbi_image_free(data);
    }
    // Set texture wrap mode (applies to both cases)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Bind the texture to the shader's diffuseTexture uniform, if present.
    if (renderer->GetShaderProgram() != 0) 
    {
        glUseProgram(renderer->GetShaderProgram());
        GLint texLoc = glGetUniformLocation(renderer->GetShaderProgram(), "diffuseTexture");
        if (texLoc != -1)
            glUniform1i(texLoc, 0);  // Use texture unit 0.
        else
            std::cerr << "Uniform 'diffuseTexture' not found in shader!" << std::endl;
    }

    // Main loop
    while (!glfwWindowShouldClose(mWindow))
    {
        glfwPollEvents();
        
        // Start a new ImGui frame.
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        const ImVec2 MainLoopPos = ImGui::GetIO().DisplaySize;
        ImGui::SetNextWindowPos(ImVec2(10.0f, 10.0f));
        ImGui::SetNextWindowSize(ImVec2(MainLoopPos.x - 20.0f, MainLoopPos.y - 20.0f));
        ImGui::Begin("3D Scene", nullptr,
            ImGuiWindowFlags_NoCollapse |       // Prevent collapsing
            ImGuiWindowFlags_NoResize |         // Prevent resizing manually
            ImGuiWindowFlags_NoMove |           // Prevent moving
            ImGuiWindowFlags_NoTitleBar |       // Remove the title bar
            ImGuiWindowFlags_NoScrollbar |      // Remove scrollbars
            ImGuiWindowFlags_NoScrollWithMouse  // Prevent accidental scrolling
        );

        // Get the available size of the ImGui window (our viewport for the scene).
        ImVec2 viewportSize = ImGui::GetContentRegionAvail();
        if (viewportSize.x <= 0.0f || viewportSize.y <= 0.0f) 
        {
            viewportSize = ImGui::GetWindowSize();  // Fallback to full window size
        }
        float aspect = (viewportSize.y > 0.0f) ? (viewportSize.x / viewportSize.y) : 1.0f;
        
        glm::mat4 projection = glm::perspective(glm::radians(60.0f), aspect, 0.1f, 100.0f);

        renderer->SetProjection(projection);
        renderer->BeginRenderToTexture((int)viewportSize.x, (int)viewportSize.y);
        renderer->Render();  // Render your sphere (or other 3D geometry) into the FBO.
        renderer->EndRenderToTexture();

        // Get the top-left position of the content region.
        ImVec2 pos = ImGui::GetCursorScreenPos();

        // Add the rendered texture as an image inside the ImGui window.
        ImGui::GetWindowDrawList()->AddImage(
            renderer->GetRenderTexture(), // our render texture
            pos,
            ImVec2(pos.x + viewportSize.x, pos.y + viewportSize.y),
            ImVec2(0, 1), ImVec2(1, 0)  // adjust UVs if necessary (flipping Y)
        );

        ImVec2 winSize = ImGui::GetWindowSize();
        ImGui::SetCursorPos(ImVec2(winSize.x - 80.0f, winSize.y - 57.0f));
        if(ImGui::Button("EXIT", ImVec2(60.0f, 37.0f)))
        {
            exit(0);
        }

        ImGui::End();
        ImGui::Render();

        int display_w, display_h;
        glfwGetFramebufferSize(mWindow, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
        glfwSwapBuffers(mWindow);
    }
}
