#include "Skybox.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Skybox::Skybox(const std::filesystem::path& texImagePath, const int nSlices, const int nStacks, const float radius) {
	rotationY = 0.0f;

	// Load panorama.
	panorama = std::make_shared<ImageTexture>(texImagePath);
	// panorama->Preview();

	// Create material.
	material = std::make_unique<SkyboxMaterial>();
	material->SetMapKd(panorama);

	// Create sphere geometry.
	CreateSphere3D(nSlices, nStacks, radius, vertices, indices);

	// Create vertex buffer.
	glGenBuffers(1, &vboId);
	glBindBuffer(GL_ARRAY_BUFFER, vboId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexPT) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
	// Create index buffer.
	glGenBuffers(1, &iboId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &(indices[0]), GL_STATIC_DRAW);
}

Skybox::~Skybox() {
	vertices.clear();
	glDeleteBuffers(1, &vboId);
	indices.clear();
	glDeleteBuffers(1, &iboId);
}

void Skybox::Render(std::shared_ptr<Camera> camera, std::shared_ptr<SkyboxShaderProg> shader) {
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, vboId);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPT), 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexPT), (const GLvoid*)12);

	shader->Bind();

	// Set transform.
	// -------------------------------------------------------
	// TODO: modify code here to rotate the skybox.
	glm::mat4x4 MVP = camera->GetProjMatrix() * camera->GetViewMatrix() * glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0)) * glm::rotate(glm::mat4(1.0f), rotationY, glm::vec3(0.0f, 1.0f, 0.0f));
	// -------------------------------------------------------
	glUniformMatrix4fv(shader->GetLocMVP(), 1, GL_FALSE, glm::value_ptr(MVP));
	// Set material properties.
	if (material->GetMapKd() != nullptr) {
		material->GetMapKd()->Bind(GL_TEXTURE0);
		glUniform1i(shader->GetLocMapKd(), 0);
	}

	// Draw.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboId);
	glDrawElements(GL_TRIANGLES, (GLsizei)(indices.size()), GL_UNSIGNED_INT, 0);

	shader->Unbind();

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}

void Skybox::CreateSphere3D(const int nSlices, const int nStacks, const float radius,
	std::vector<VertexPT>& vertices, std::vector<unsigned int>& indices) {
	const int numPhi = nSlices;
	const int numTheta = nStacks;
	// Phi range: from 0 to 2PI.
	// Theta range: from PI/2 to -PI/2.
	const float phiStart = 0.0f;
	const float thetaStart = 0.5f * glm::pi<float>();
	const float phiOffset = 2.0f * glm::pi<float>() / (float)numPhi;
	const float thetaOffset = -glm::pi<float>() / (float)numTheta;
	for (int t = 0; t <= numTheta; ++t) {
		for (int p = 0; p <= numPhi; ++p) {
			float phi = phiStart + p * phiOffset;
			float theta = thetaStart + t * thetaOffset;
			// std::cout << glm::degrees<float>(phi) << " " << glm::degrees<float>(theta) << std::endl;
			glm::vec2 uv = glm::vec2((float)p / (float)numPhi, (float)t / (float)numTheta);
			// std::cout << uv.x << " " << uv.y << std::endl;
			float x = radius * glm::cos(theta) * glm::cos(phi);
			float y = radius * glm::sin(theta);
			float z = radius * glm::cos(theta) * glm::sin(phi);

			VertexPT vt = VertexPT(glm::vec3(x, y, z), uv);
			vertices.push_back(vt);
		}
	}

	// Vertex order (4 x 2 division for example): 
	//  0   1   2   3   4.
	//  5   6   7   8   9.
	// 10  11  12  13  14.
	const int pVertices = nSlices + 1;
	const int tVertices = nStacks + 1;
	for (int t = 0; t < nStacks; ++t) {
		for (int p = 0; p < nSlices; ++p) {
			// Each grid will generate 2 triangles.
			// The upper-half one.
			indices.push_back(t * pVertices + p);
			indices.push_back(t * pVertices + p + 1);
			indices.push_back((t + 1) * pVertices + p);
			// The bottom-half one.
			indices.push_back(t * pVertices + p + 1);
			indices.push_back((t + 1) * pVertices + p + 1);
			indices.push_back((t + 1) * pVertices + p);
		}
	}
}

