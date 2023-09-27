#include <ScreenManager.h>

namespace opengl_homework
{

// ------------------------------------------------------------------------
// Public member functions. -----------------------------------------------
// ------------------------------------------------------------------------

void ScreenManager::Start(int argc, char **argv)
{
    // Setting window properties.
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(m_width, m_height);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("HW1: OBJ Loader");

    // Initialize GLEW.
    // Must be done after glut is initialized!
    GLenum res = glewInit();
    if (res != GLEW_OK) {
        std::cerr << "GLEW initialization error: " 
                  << glewGetErrorString(res) << std::endl;
        exit(EXIT_FAILURE);
    }

    // Initialization.
    SetupRenderState();
    SetupScene("Bunny");
    SetupMenu();

    // Register callback functions.
    glutDisplayFunc([](){ StaticWrapper(&ScreenManager::RenderSceneCB); });
    glutIdleFunc([](){ StaticWrapper(&ScreenManager::RenderSceneCB); });
    glutReshapeFunc([](int w, int h){ StaticWrapper(&ScreenManager::ReshapeCB, w, h); });
    glutSpecialFunc([](int key, int x, int y){ StaticWrapper(&ScreenManager::ProcessSpecialKeysCB, key, x, y); });
    glutKeyboardFunc([](unsigned char key, int x, int y){ StaticWrapper(&ScreenManager::ProcessKeysCB, key, x, y); });

    // Start rendering loop.
    glutMainLoop();
}

// ------------------------------------------------------------------------
// Private member functions. ----------------------------------------------
// ------------------------------------------------------------------------

ScreenManager::ScreenManager()
{
    // Load all obj files in the models directory.
    for (const auto& entry : std::filesystem::directory_iterator("models")) {
        if (entry.path().extension() == ".obj") {
            m_objNames.push_back(entry.path().stem().string());
        }
    }
}

// Callback function for glutDisplayFunc.
void ScreenManager::RenderSceneCB()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    m_mesh->Render();
    
    glutSwapBuffers();
}

// Callback function for glutReshapeFunc.
void ScreenManager::ReshapeCB(int w, int h)
{
    // Adjust camera and projection here.
    // Implemented in HW2.
}

// Callback function for glutSpecialFunc.
void ScreenManager::ProcessSpecialKeysCB(int key, int x, int y)
{
    // Handle special (functional) keyboard inputs such as F1, spacebar, page up, etc. 
    switch (key) {
    case GLUT_KEY_F1:
        // Render with point mode.
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
        break;
    case GLUT_KEY_F2:
        // Render with line mode.
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        break;
    case GLUT_KEY_F3:
        // Render with fill mode.
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        break;
    default:
        break;
    }
}

// Callback function for glutKeyboardFunc.
void ScreenManager::ProcessKeysCB(unsigned char key, int x, int y)
{
    // Handle other keyboard inputs those are not defined as special keys.
    if (key == 27) {
        // Release memory allocation if needed.
        ReleaseResources();
        exit(0);
    }
}

void ScreenManager::ReleaseResources()
{
    s_instance = nullptr;

    // Release the memory allocated for the triangle mesh.
    m_mesh.reset(nullptr);
    m_objNames.clear();
}

void ScreenManager::SetupRenderState()
{
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glm::vec4 clearColor = glm::vec4(0.44f, 0.57f, 0.75f, 1.00f);
    glClearColor(
        (GLclampf)(clearColor.r), 
        (GLclampf)(clearColor.g), 
        (GLclampf)(clearColor.b), 
        (GLclampf)(clearColor.a)
    );
}

// Load a model from obj file and apply transformation.
// You can alter the parameters for dynamically loading a model.
void ScreenManager::SetupScene(const std::string& objName)
{
    std::cout << "Loading model: " << objName << std::endl;
    auto modelPath = std::filesystem::path("models") / (objName + ".obj");

    m_mesh = std::make_unique<opengl_homework::TriangleMesh>();
    m_mesh->LoadFromFile(modelPath);
 
    // Please DO NOT TOUCH the following code.
    // ------------------------------------------------------------------------
    // Build transformation matrices.
    // World.
    glm::mat4x4 M(1.0f);
    // Camera.
    glm::vec3 cameraPos = glm::vec3(0.0f, 0.5f, 2.0f);
    glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::mat4x4 V = glm::lookAt(cameraPos, cameraTarget, cameraUp);
    // Projection.
    float fov = 40.0f;
    float aspectRatio = static_cast<float>(m_width) / static_cast<float>(m_height);
    float zNear = 0.1f;
    float zFar = 100.0f;
    glm::mat4x4 P = glm::perspective(glm::radians(fov), aspectRatio, zNear, zFar);

    // Apply CPU transformation.
    glm::mat4x4 MVP = P * V * M;
    
    m_mesh->ApplyTransformCPU(MVP);
    
    // Create and upload vertex/index buffers.
    m_mesh->CreateBuffers();
}

void ScreenManager::SetupMenu()
{
    // Create the main menu.
    glutCreateMenu([](int value){ StaticWrapper(&ScreenManager::MenuCB, value); });
    
    for (int i = 0; i < m_objNames.size(); i++) 
        glutAddMenuEntry(m_objNames[i].c_str(), i + 1);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void ScreenManager::MenuCB(int value)
{
    m_mesh.reset(nullptr);
    SetupScene(m_objNames[value - 1]);
}

ScreenManager* ScreenManager::s_instance = nullptr;
std::mutex ScreenManager::s_mutex = std::mutex();

}