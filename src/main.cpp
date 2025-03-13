#include <memory>
#include <GLFW/glfw3.h>
#include "UIFramework.h"

int main()
{
    auto ui = std::make_unique<UIFramework>();
    ui->Init(1920, 1080, "UI App", nullptr, nullptr);
    ui->Run();
}
