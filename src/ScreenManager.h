#pragma once

// My headers.
#include "TriangleMesh.h"

namespace opengl_homework
{

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
        std::lock_guard<std::mutex> lock(s_mutex);
        if (s_instance == nullptr) {
            s_instance = std::shared_ptr<ScreenManager>(new ScreenManager());
        }
        return std::shared_ptr<ScreenManager>(s_instance);
    }

    ~ScreenManager() { ReleaseResources(); }

   
    /**
     * @brief Start the rendering loop.
     */  
    void Start(int, char **);

private:
    ScreenManager();

    void SetupRenderState();
    void SetupScene(const std::string&);
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
    static auto Member2Callback(void(ScreenManager::*func)(Args...)) {
        static void(ScreenManager::*s_func)(Args...) = func;
        return [](Args... args) { (GetInstance().get()->*s_func)(args...); };
    }

private:
    static std::shared_ptr<ScreenManager> s_instance;
    static std::mutex s_mutex;

    int m_width = 600;
    int m_height = 600;
    std::vector<std::string> m_objNames;
    std::unique_ptr<opengl_homework::TriangleMesh> m_mesh = nullptr;
};

}
