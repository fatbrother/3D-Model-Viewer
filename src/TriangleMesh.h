#pragma once

// OpenGL and FreeGlut headers.
#include <GL/glew.h>
#include <GL/freeglut.h>

// GLM.
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

// C++ STL headers.
#include <bits/stdc++.h>

namespace opengl_homework {

// VertexPTN Declarations.
struct VertexPTN
{
	VertexPTN() {
		position = glm::vec3(0.0f, 0.0f, 0.0f);
		normal = glm::vec3(0.0f, 1.0f, 0.0f);
		texcoord = glm::vec2(0.0f, 0.0f);
	}
	VertexPTN(glm::vec3 p, glm::vec3 n, glm::vec2 uv) {
		position = p;
		normal = n;
		texcoord = uv;
	}
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texcoord;
};

/**
 * @brief TriangleMesh class.
*/
class TriangleMesh
{
public:
	// TriangleMesh Public Methods.
	TriangleMesh();
	~TriangleMesh();
	
	/**
	 * @brief Load a model from obj file.
	 * 
	 * @param filePath Path to the obj file.
	 * @param normalized Normalize the model to fit in a unit cube.
	 * 
	 * @return true if the model is loaded successfully.
	*/
	bool LoadFromFile(const std::filesystem::path& filePath, const bool normalized = true);
	
	/**
	 * @brief Create buffers for rendering.
	*/
	void CreateBuffers();

	/**
	 * @brief Render the mesh.
	*/
	void Render() const;

	/**
	 * @brief Apply transformation to the mesh on CPU.
	 * 
	 * @param mvpMatrix Model-view-projection matrix.
	*/
	void ApplyTransformCPU(const glm::mat4x4& mvpMatrix);

	int GetNumVertices() const { return numVertices; }
	int GetNumTriangles() const { return numTriangles; }
	int GetNumIndices() const { return (int)vertexIndices.size(); }
	glm::vec3 GetObjCenter() const { return objCenter; }

private:
	// TriangleMesh Private Methods.
	void PrintMeshInfo() const;

	// TriangleMesh Private Data.
	GLuint vboId;
	GLuint iboId;
	std::vector<VertexPTN> vertices;
	std::vector<unsigned int> vertexIndices;

	int numVertices;
	int numTriangles;
	glm::vec3 objCenter;
};

}