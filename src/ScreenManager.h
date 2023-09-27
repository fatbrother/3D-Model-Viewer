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
            s_instance = new ScreenManager();
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
     * @brief Static wrapper for member functions.
     * 
     * @tparam T 
     * @tparam Args 
     * @param func 
     * @param args 
     * 
     * @note Usage: [](Args... args){ StaticWrapper(&T::MemberFunction, args...); }
     */
    template <typename T, typename... Args>
    static void StaticWrapper(void (T::*func)(Args...), Args... args)
    {
        (s_instance->*func)(args...);
    }

private:
    static ScreenManager* s_instance;
    static std::mutex s_mutex;

    int m_width = 600;
    int m_height = 600;
    std::vector<std::string> m_objNames;
    std::unique_ptr<opengl_homework::TriangleMesh> m_mesh = nullptr;
};

}
