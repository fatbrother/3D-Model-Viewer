#pragma once

#include <glm/glm.hpp>

// Camera Declarations.
class Camera {
public:
	// Camera Public Methods.
	Camera(const float aspectRatio);
	~Camera();

	glm::mat4x4& GetViewMatrix() { return viewMatrix; }
	glm::mat4x4& GetProjMatrix() { return projMatrix; }
	glm::vec3& GetPosition() { return position; }

	void UpdateView(const glm::vec3 newPos, const glm::vec3 newTarget, const glm::vec3 up);
	void UpdateAspectRatio(const float aspectRatio);
	void UpdateFovy(const float fovyInDegree);
	void UpdateNearPlane(const float zNear);
	void UpdateFarPlane(const float zFar);
	void UpdateProjection();

private:
	// Camera Private Data.
	glm::vec3 position;
	glm::vec3 target;

	float fovy;	// in degree.
	float aspectRatio;
	float nearPlane;
	float farPlane;

	glm::mat4x4 viewMatrix;
	glm::mat4x4 projMatrix;
};
