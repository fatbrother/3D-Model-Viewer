#pragma once

#include "ImageTexture.h"
#include "ShaderProg.h"
#include "Material.h"
#include "Camera.h"

// VertexPT Declarations.
struct VertexPT
{
	VertexPT() {
		position = glm::vec3(0.0f, 0.0f, 0.0f);
		texcoord = glm::vec2(0.0f, 0.0f);
	}
	VertexPT(glm::vec3 p, glm::vec2 uv) {
		position = p;
		texcoord = uv;
	}
	glm::vec3 position;
	glm::vec2 texcoord;
};


// Skybox Declarations.
class Skybox
{
public:
	// Skybox Public Methods.
	Skybox(const std::filesystem::path& texImagePath, const int nSlices,
			const int nStacks, const float radius);
	~Skybox();
	void Render(std::shared_ptr<Camera> camera, std::shared_ptr<SkyboxShaderProg> shader);
	
	void SetRotation(const float newRotation) { rotationY = newRotation; }
	
	std::shared_ptr<SkyboxMaterial> GetMaterial() const { return material; }
	float GetRotation() const  { return rotationY; }

private:
	// Skybox Private Methods.
	static void CreateSphere3D(const int nSlices, const int nStacks, const float radius, 
					std::vector<VertexPT>& vertices, std::vector<unsigned int>& indices);

	// Skybox Private Data.
	GLuint vboId;
	GLuint iboId;
	std::vector<VertexPT> vertices;
	std::vector<unsigned int> indices;
	
	std::shared_ptr<SkyboxMaterial> material;
	std::shared_ptr<ImageTexture> panorama;

	float rotationY;
};
