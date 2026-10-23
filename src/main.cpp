#include <iostream>
#include <FL/Fl.H>
#include "./Backend/Backend.h"
#include "./Frontend/Frontend.h"

int main()
{
    ShowWindow(GetConsoleWindow(), SW_HIDE);

    if (Fl::lock() != 0)
    {
        std::cerr << "Failed to initialize FLTK threading support" << std::endl;
        return 1;
    }

    Backend backend;
    Frontend frontend;
    frontend.attachBackend(&backend);
    backend.setUpdateUICallback([&frontend]() {
        Fl::awake([](void* data) {
            static_cast<Frontend*>(data)->update();
        }, &frontend);
    });

    std::cout << "Server is running on http://localhost:8080" << std::endl;

    return Fl::run();
}
