// My headers.
#include "ScreenManager.h"

int main(int argc, char** argv) {
    // Start rendering loop.
    auto screen = opengl_homework::ScreenManager::GetInstance();
    screen->Start(argc, argv);
    return 0;
}

