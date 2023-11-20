#include <ScreenManager.h>

// OpenGL and FreeGlut headers.
#include <GL/glew.h>
#include <GL/freeglut.h>

// GLM headers.
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

// C++ STL headers.
#include <iostream>
#include <thread>
#include <vector>
#include <mutex>

// My headers.
#include "TriangleMesh.h"
#include "ShaderProg.h"
#include "Light.h"
#include "Camera.h"

namespace opengl_homework {

using MeshPtr = std::shared_ptr<opengl_homework::TriangleMesh>;

std::shared_ptr<ScreenManager> ScreenManager::GetInstance() {
    static std::shared_ptr<ScreenManager> instance(new ScreenManager());
    return instance;
}

template<typename... Args>
auto ScreenManager::Member2Callback(void(ScreenManager::* func)(Args...)) {
    static void(ScreenManager:: * s_func)(Args...) = func;
    return [](Args... args) { (GetInstance().get()->*s_func)(args...); };
}

class SceneObject
{
public:
    SceneObject() {
        mesh = nullptr;
        worldMatrix = glm::mat4x4(1.0f);
    }

    void Update(const glm::mat4& transform) {
        worldMatrix = transform * worldMatrix;
    }

    MeshPtr mesh;
    glm::mat4x4 worldMatrix;
};

// SceneLight (for visualization of a point light).
// T is derived from PointLight
template<typename T,
    typename = std::enable_if<std::is_base_of<PointLight, T>::value>>
    struct SceneLight
{
    SceneLight() {
        light = nullptr;
        worldMatrix = glm::mat4x4(1.0f);
        visColor = glm::vec3(1.0f, 1.0f, 1.0f);
    }
    std::shared_ptr<T> light;
    glm::mat4x4 worldMatrix;
    glm::vec3 visColor;
};

// ------------------------------------------------------------------------
// Private member implementations. ----------------------------------------
// ------------------------------------------------------------------------
struct ScreenManager::Impl {
    Impl() :
        width(600),
        height(600),
        camera(std::make_unique<Camera>((float)width / (float)height)) {
        sceneObj = std::make_unique<SceneObject>();
        pointLightObj = std::make_unique<SceneLight<PointLight>>();
        spotLightObj = std::make_unique<SceneLight<SpotLight>>();
    };

    int width;
    int height;
    std::vector<std::string> objNames;
    std::vector<MeshPtr> meshes;
    RenderStyle renderStyle = RenderStyle::FILL_COLOR;
    std::unique_ptr<FillColorShaderProg> fillColorShader;
    std::unique_ptr<PhongShadingDemoShaderProg> phongShader;
    std::unique_ptr<SceneObject> sceneObj;
    std::unique_ptr<Camera> camera;
    std::shared_ptr<DirectionalLight> dirLight;
    std::shared_ptr<SceneLight<PointLight>> pointLightObj;
    std::shared_ptr<SceneLight<SpotLight>> spotLightObj;
    glm::vec3 ambientLight;
    float lightMoveSpeed = 0.2f;
};

// ------------------------------------------------------------------------
// Public member functions. -----------------------------------------------
// ------------------------------------------------------------------------

void ScreenManager::Start(int argc, char** argv) {
    // Setting window properties.
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(pImpl->width, pImpl->height);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("HW2: Lighting and Shading");

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
    SetupLights();
    SetupCamera();
    SetupShaderLib();
    SetupScene(0);

    // Register callback functions.
    glutCreateMenu(Member2Callback(&ScreenManager::MenuCB));
    glutDisplayFunc(Member2Callback(&ScreenManager::RenderSceneCB));
    glutIdleFunc(Member2Callback(&ScreenManager::RenderSceneCB));
    glutReshapeFunc(Member2Callback(&ScreenManager::ReshapeCB));
    glutSpecialFunc(Member2Callback(&ScreenManager::ProcessSpecialKeysCB));
    glutKeyboardFunc(Member2Callback(&ScreenManager::ProcessKeysCB));

    SetupMenu();

    // Start rendering loop.
    glutMainLoop();
}

// ------------------------------------------------------------------------
// Private member functions. ----------------------------------------------
// ------------------------------------------------------------------------

ScreenManager::ScreenManager() {
    pImpl = std::make_unique<Impl>();

    // Load all obj files in the models directory.
    for (const auto& entry : std::filesystem::directory_iterator("models")) {
        if (entry.is_directory()) {
            pImpl->objNames.push_back(entry.path().filename().string());
        }
    }

    // find the minimum bytes size of the obj files and swap it to the first position
    int min = INT_MAX;
    int minIndex = 0;
    for (int i = 0; i < pImpl->objNames.size(); i++) {
        auto size = std::filesystem::file_size("models/" + pImpl->objNames[i] + "/" + pImpl->objNames[i] + ".obj");
        if (size < min) {
            min = size;
            minIndex = i;
        }
    }
    std::swap(pImpl->objNames[0], pImpl->objNames[minIndex]);
    // Load all obj files.
    auto objBasePath = std::filesystem::path("models");
    pImpl->meshes.resize(pImpl->objNames.size());

    // Load first model in the main thread.
    auto firstFilePath = objBasePath / pImpl->objNames[0] / (pImpl->objNames[0] + ".obj");
    pImpl->meshes[0] = std::make_shared<TriangleMesh>(firstFilePath, true);

    auto threadFunc = [](
        int i,
        const std::filesystem::path& objBasePath,
        const std::vector<std::string>& objNames,
        std::vector<MeshPtr>& meshes) {
            auto objFilePath = objBasePath / objNames[i] / (objNames[i] + ".obj");
            meshes[i] = std::make_shared<TriangleMesh>(objFilePath, true);
        };
    for (int i = 1; i < pImpl->objNames.size(); i++) {
        std::thread thread(threadFunc, i, objBasePath, pImpl->objNames, std::ref(pImpl->meshes));
        thread.detach();
    }
}

// Callback function for glutDisplayFunc.
void ScreenManager::RenderSceneCB() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    static auto lastTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastTime).count() / 1000.0f;
    lastTime = currentTime;

    // Rotate the model.
    auto rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
    auto rotationAngle = 0.1f * deltaTime;
    glm::mat4x4 R = glm::rotate(glm::mat4x4(1.0f), rotationAngle, rotationAxis);
    pImpl->sceneObj->Update(R);

    glm::mat4x4 V = pImpl->camera->GetViewMatrix();
    glm::mat4x4 normalMatrix = glm::transpose(glm::inverse(pImpl->sceneObj->worldMatrix));
    glm::mat4x4 MVP = pImpl->camera->GetProjMatrix() * V * pImpl->sceneObj->worldMatrix;

    pImpl->sceneObj->mesh->Render(
        pImpl->phongShader,
        MVP,
        V,
        pImpl->sceneObj->worldMatrix,
        normalMatrix,
        pImpl->ambientLight,
        pImpl->dirLight,
        pImpl->pointLightObj->light,
        pImpl->spotLightObj->light);

    // Visualize the light with fill color. ------------------------------------------------------
    // Bind shader and set parameters.
    auto pointLight = pImpl->pointLightObj->light;
    if (pointLight != nullptr) {
        glm::mat4x4 T = glm::translate(glm::mat4x4(1.0f), (pointLight->GetPosition()));
        pImpl->pointLightObj->worldMatrix = T;
        glm::mat4x4 MVP = pImpl->camera->GetProjMatrix() * pImpl->camera->GetViewMatrix() * pImpl->pointLightObj->worldMatrix;

        pImpl->fillColorShader->Bind();

        glUniformMatrix4fv(pImpl->fillColorShader->GetLocMVP(), 1, GL_FALSE, glm::value_ptr(MVP));
        glUniform3fv(pImpl->fillColorShader->GetLocFillColor(), 1, glm::value_ptr(pImpl->pointLightObj->visColor));

        // Render the point light.
        pImpl->pointLightObj->light->Draw();

        pImpl->fillColorShader->Unbind();
    }
    auto spotLight = pImpl->spotLightObj->light;
    if (spotLight != nullptr) {
        glm::mat4x4 T = glm::translate(glm::mat4x4(1.0f), (spotLight->GetPosition()));
        pImpl->spotLightObj->worldMatrix = T;
        glm::mat4x4 MVP = pImpl->camera->GetProjMatrix() * pImpl->camera->GetViewMatrix() * pImpl->spotLightObj->worldMatrix;

        pImpl->fillColorShader->Bind();

        glUniformMatrix4fv(pImpl->fillColorShader->GetLocMVP(), 1, GL_FALSE, glm::value_ptr(MVP));
        glUniform3fv(pImpl->fillColorShader->GetLocFillColor(), 1, glm::value_ptr(pImpl->spotLightObj->visColor));

        // Render the point light.
        pImpl->spotLightObj->light->Draw();

        pImpl->fillColorShader->Unbind();
    }

    glutSwapBuffers();
}

// Callback function for glutReshapeFunc.
void ScreenManager::ReshapeCB(int w, int h) {
    // Adjust camera and projection here.

    // Update viewport.
    pImpl->width = w;
    pImpl->height = h;
    glViewport(0, 0, pImpl->width, pImpl->height);
    // Adjust camera and projection.
    pImpl->camera->UpdateAspectRatio((float)pImpl->width / (float)pImpl->height);
    pImpl->camera->UpdateProjection();
}

void ScreenManager::ProcessSpecialKeysCB(int key, int x, int y) {
    // Light control.
    switch (key) {
    case GLUT_KEY_LEFT:
        if (pImpl->pointLightObj->light != nullptr)
            pImpl->pointLightObj->light->MoveLeft(pImpl->lightMoveSpeed);
        break;
    case GLUT_KEY_RIGHT:
        if (pImpl->pointLightObj->light != nullptr)
            pImpl->pointLightObj->light->MoveRight(pImpl->lightMoveSpeed);
        break;
    case GLUT_KEY_UP:
        if (pImpl->pointLightObj->light != nullptr)
            pImpl->pointLightObj->light->MoveUp(pImpl->lightMoveSpeed);
        break;
    case GLUT_KEY_DOWN:
        if (pImpl->pointLightObj->light != nullptr)
            pImpl->pointLightObj->light->MoveDown(pImpl->lightMoveSpeed);
        break;
    default:
        break;
    }
}

// Callback function for glutKeyboardFunc.
void ScreenManager::ProcessKeysCB(unsigned char key, int x, int y) {
    // Handle other keyboard inputs those are not defined as special keys.
    if (key == 27) {
        exit(0);
    }

    // Spot light control.
    auto spotLight = pImpl->spotLightObj->light;
    if (spotLight != nullptr) {
        if (key == 'a')
            spotLight->MoveLeft(pImpl->lightMoveSpeed);
        if (key == 'd')
            spotLight->MoveRight(pImpl->lightMoveSpeed);
        if (key == 'w')
            spotLight->MoveUp(pImpl->lightMoveSpeed);
        if (key == 's')
            spotLight->MoveDown(pImpl->lightMoveSpeed);
    }
}

void ScreenManager::SetupRenderState() {
    glEnable(GL_DEPTH_TEST);

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
void ScreenManager::SetupScene(int objIndex) {
    glm::mat4x4 S = glm::scale(glm::mat4x4(1.0f), glm::vec3(1.5f, 1.5f, 1.5f));
    pImpl->sceneObj->worldMatrix = S;
    if (pImpl->sceneObj->mesh != nullptr) {
        pImpl->sceneObj->mesh->ReleaseBuffers();
    }
    pImpl->sceneObj->mesh = pImpl->meshes[objIndex];
    pImpl->sceneObj->mesh->CreateBuffers();
}

void ScreenManager::SetupLights() {
    glm::vec3 dirLightDirection = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 dirLightRadiance = glm::vec3(0.6f, 0.6f, 0.6f);
    glm::vec3 pointLightPosition = glm::vec3(0.8f, 0.0f, 0.8f);
    glm::vec3 pointLightIntensity = glm::vec3(0.5f, 0.1f, 0.1f);
    glm::vec3 spotLightPosition = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 spotLightDirection = glm::vec3(0.0f, -1.0f, 0.0f);
    glm::vec3 spotLightIntensity = glm::vec3(0.5f, 0.5f, 0.1f);
    float spotLightCutoffStartInDegree = 30.0f;
    float spotLightTotalWidthInDegree = 45.0f;
    glm::vec3 ambientLight = glm::vec3(0.2f, 0.2f, 0.2f);
    pImpl->dirLight = std::make_unique<DirectionalLight>(dirLightDirection, dirLightRadiance);
    pImpl->pointLightObj->light = std::make_shared<PointLight>(pointLightPosition, pointLightIntensity);
    pImpl->pointLightObj->visColor = glm::normalize((pImpl->pointLightObj->light->GetIntensity()));
    pImpl->spotLightObj->light = std::make_shared<SpotLight>(
        spotLightPosition,
        spotLightIntensity,
        spotLightDirection,
        spotLightCutoffStartInDegree,
        spotLightTotalWidthInDegree);
    pImpl->spotLightObj->visColor = glm::normalize((pImpl->spotLightObj->light->GetIntensity()));
    pImpl->ambientLight = ambientLight;
}

void ScreenManager::SetupCamera() {
    // Create a camera and update view and proj matrices.
    float fovy = 30.0f;
    float zNear = 0.1f;
    float zFar = 1000.0f;
    glm::vec3 cameraPos = glm::vec3(0.0f, 1.0f, 5.0f);
    glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
    pImpl->camera->UpdateView(cameraPos, cameraTarget, cameraUp);
    pImpl->camera->UpdateAspectRatio((float)pImpl->width / (float)pImpl->height);
    pImpl->camera->UpdateFovy(fovy);
    pImpl->camera->UpdateNearPlane(zNear);
    pImpl->camera->UpdateFarPlane(zFar);
    pImpl->camera->UpdateProjection();
}

void ScreenManager::SetupShaderLib() {
    pImpl->fillColorShader = std::make_unique<FillColorShaderProg>();
    pImpl->phongShader = std::make_unique<PhongShadingDemoShaderProg>();

    if (!pImpl->fillColorShader->LoadFromFiles("shaders/fixed_color.vs", "shaders/fixed_color.fs", "")) {
        std::cerr << "Failed to load fixed_color shader." << std::endl;
        exit(EXIT_FAILURE);
    }
    if (!pImpl->phongShader->LoadFromFiles("shaders/phong_shading_demo.vs", "shaders/phong_shading_demo.fs", "shaders/face_culling.gs")) {
        std::cerr << "Failed to load gouraud shader." << std::endl;
        exit(EXIT_FAILURE);
    }
}

void ScreenManager::SetupMenu() {
    // Create the menu and attach it to the right button.
    for (int i = 0; i < pImpl->objNames.size(); i++)
        glutAddMenuEntry(pImpl->objNames[i].c_str(), i + 1);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void ScreenManager::MenuCB(int value) {
    SetupScene(value - 1);
}

} // namespace opengl_homework