#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>

struct Camera::Impl {
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

Camera::Camera(const float aspectRatio) {
	pImpl = std::make_unique<Impl>();
	// Default camera pose and parameters.
	pImpl->position = glm::vec3(0.0f, 1.0f, 5.0f);
	pImpl->target = glm::vec3(0.0f, 0.0f, 0.0f);
	pImpl->fovy = 30.0f;
	pImpl->nearPlane = 0.1f;
	pImpl->farPlane = 1000.0f;
	UpdateView(pImpl->position, pImpl->target, glm::vec3(0.0f, 1.0f, 0.0f));
	UpdateProjection();
}

Camera::~Camera() { pImpl.reset(); }

glm::mat4x4& Camera::GetViewMatrix() { return pImpl->viewMatrix; }

glm::mat4x4& Camera::GetProjMatrix() { return pImpl->projMatrix; }

glm::vec3& Camera::GetPosition() { return pImpl->position; }

void Camera::UpdateAspectRatio(const float aspectRatio) {
	pImpl->aspectRatio = aspectRatio;
	UpdateProjection();
}

void Camera::UpdateFovy(const float fovyInDegree) {
	pImpl->fovy = fovyInDegree;
	UpdateProjection();
}

void Camera::UpdateNearPlane(const float zNear) {
	pImpl->nearPlane = zNear;
	UpdateProjection();
}

void Camera::UpdateFarPlane(const float zFar) {
	pImpl->farPlane = zFar;
	UpdateProjection();
}

void Camera::UpdateView(const glm::vec3 newPos, const glm::vec3 newTarget, const glm::vec3 up) {
	pImpl->position = newPos;
	pImpl->target = newTarget;
	pImpl->viewMatrix = glm::lookAt(pImpl->position, pImpl->target, up);
}

void Camera::UpdateProjection() {
	pImpl->projMatrix = glm::perspective(
		glm::radians(pImpl->fovy),
		pImpl->aspectRatio,
		pImpl->nearPlane,
		pImpl->farPlane
	);
}