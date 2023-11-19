#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(const float aspectRatio) {
	// Default camera pose and parameters.
	position = glm::vec3(0.0f, 1.0f, 5.0f);
	target = glm::vec3(0.0f, 0.0f, 0.0f);
	fovy = 30.0f;
	nearPlane = 0.1f;
	farPlane = 1000.0f;
	UpdateView(position, target, glm::vec3(0.0f, 1.0f, 0.0f));
	UpdateProjection();
}

Camera::~Camera() {
}

void Camera::UpdateView(const glm::vec3 newPos, const glm::vec3 newTarget, const glm::vec3 up) {
	position = newPos;
	target = newTarget;
	viewMatrix = glm::lookAt(position, target, up);
}

void Camera::UpdateAspectRatio(const float aspectRatio) {
	this->aspectRatio = aspectRatio;
}

void Camera::UpdateFovy(const float fovyInDegree) {
	fovy = fovyInDegree;
}

void Camera::UpdateNearPlane(const float zNear) {
	nearPlane = zNear;
}

void Camera::UpdateFarPlane(const float zFar) {
	farPlane = zFar;
}

void Camera::UpdateProjection() {
	projMatrix = glm::perspective(glm::radians(fovy), aspectRatio, nearPlane, farPlane);
}