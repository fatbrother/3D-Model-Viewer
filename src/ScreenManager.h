#pragma once

#include <bits/stdc++.h>

#include "TriangleMesh.h"

namespace opengl_homework
{
    
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

    // Callback functions.
    static void RenderSceneCBWrapper() { s_instance->RenderSceneCB(); }
    static void ReshapeCBWrapper(int w, int h) { s_instance->ReshapeCB(w, h); }
    static void ProcessSpecialKeysCBWrapper(int key, int x, int y) { s_instance->ProcessSpecialKeysCB(key, x, y); }
    static void ProcessKeysCBWrapper(unsigned char key, int x, int y) { s_instance->ProcessKeysCB(key, x, y); }
    static void MenuCBWrapper(int value) { s_instance->MenuCB(value); }

private:
    static ScreenManager* s_instance;
    static std::mutex s_mutex;

    int m_width = 600;
    int m_height = 600;
    std::vector<std::string> m_objNames;
    std::unique_ptr<opengl_homework::TriangleMesh> m_mesh = nullptr;
};

}
