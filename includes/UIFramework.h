#ifndef UIFRAMEWORK_H
#define UIFRAMEWORK_H

#include <GLFW/glfw3.h>

class UIFramework
{
    public:
    UIFramework() = default;
    UIFramework(UIFramework&&) = delete;
    UIFramework(const UIFramework&) = delete;
    ~UIFramework();
    
    UIFramework& operator=(const UIFramework&) = delete;
    void Init(int width, int height, const char* title, GLFWmonitor* monitor, GLFWwindow* share);
    void Run();

    private:
    GLFWwindow* mWindow = nullptr;
};

#endif
