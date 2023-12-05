#pragma once

// C++ STL headers.
#include <memory>

namespace opengl_homework {

/**
 * @brief ScreenManager class.
 *
 * This class manages the rendering loop and the user input.
 * It is implemented as a singleton.
 *
 * @note Call GetInstance() to get the singleton instance, and call Start() to start the rendering loop.
*/
class ScreenManager
{
public:
    /**
     * @brief Get the singleton instance of ScreenManager.
     *
     * @return std::shared_ptr<ScreenManager>
     */
    static std::shared_ptr<ScreenManager> GetInstance();

    /**
     * @brief Start the rendering loop.
     */
    void Start(int, char**);

private:
    // ScreenManager Private Methods.
    ScreenManager();

    int CalculateFrameRate();

    void SetupRenderState();
    void SetupScene(int);
    void SetupShaderLib();
    void SetupLights();
    void SetupCamera();
    void SetupMenu();

    void ReshapeCB(int, int);
    void ProcessSpecialKeysCB(int, int, int);
    void ProcessKeysCB(unsigned char, int, int);
    void RenderSceneCB();
    void MenuCB(int);

private:
    // ScreenManager Private Data.
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

}
