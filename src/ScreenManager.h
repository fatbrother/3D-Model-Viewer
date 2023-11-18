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

    void SetupRenderState();
    void SetupScene(int);
    void SetupShaderLib();
    void SetupLights();
    void SetupCamera();
    void ReshapeCB(int, int);
    void ProcessSpecialKeysCB(int, int, int);
    void ProcessKeysCB(unsigned char, int, int);
    void SetupMenu();
    void RenderSceneCB();
    void MenuCB(int);

    /**
     * @brief Convert a member function to a lambda function.
     *
     * @tparam Args
     * @param func
     * @return lambda function
     *
     * @note Usage: glutRegisterFunc(Member2Callback(&ScreenManager::MemberFunc));
    */
    template<typename... Args>
    static auto Member2Callback(void(ScreenManager::* func)(Args...));
    
private:
    enum RenderStyle
    {
        FILL_COLOR,
        VIS_NORMAL,
    };

    // ScreenManager Private Data.
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

}
