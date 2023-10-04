#pragma once

// My headers.
#include "TriangleMesh.h"

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
    static std::shared_ptr<ScreenManager> GetInstance() {
        static std::shared_ptr<ScreenManager> instance(new ScreenManager);
        return instance;      
    }

    ~ScreenManager() { ReleaseResources(); }


    /**
     * @brief Start the rendering loop.
     */
    void Start(int, char**);

private:
    ScreenManager();

    void SetupRenderState();
    void SetupScene(int);
    void ReleaseResources();
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
     * @note Usage: glutRegisterFunc(StaticWrapper(&ScreenManager::MemberFunc));
    */
    template<typename... Args>
    static auto Member2Callback(void(ScreenManager::* func)(Args...)) -> void(*)(Args...) {
        static void(ScreenManager:: * s_func)(Args...) = func;
        return [](Args... args) { (GetInstance().get()->*s_func)(args...); };
    }

private:
    int m_width = 600;
    int m_height = 600;
    std::vector<std::string> m_objNames;

    using MeshPtr = std::shared_ptr<opengl_homework::TriangleMesh>;
    std::vector<MeshPtr> m_meshes;
    MeshPtr m_currentMesh;
};

}
