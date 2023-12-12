#pragma once

#include <glm/glm.hpp>
#include <memory>

// Camera Declarations.
class Camera {
public:
	// Camera Public Methods.
	Camera(const float aspectRatio);
	~Camera();

	glm::mat4x4& GetViewMatrix();
	glm::mat4x4& GetProjMatrix();
	glm::vec3& GetPosition();

	void UpdateView(const glm::vec3 newPos, const glm::vec3 newTarget, const glm::vec3 up);
	void UpdateAspectRatio(const float aspectRatio);
	void UpdateFovy(const float fovyInDegree);
	void UpdateNearPlane(const float zNear);
	void UpdateFarPlane(const float zFar);
	void UpdateProjection();

private:
	/**
	 * @brief Camera Private Data.
	*/
	struct Impl;
	std::unique_ptr<Impl> pImpl;
};
