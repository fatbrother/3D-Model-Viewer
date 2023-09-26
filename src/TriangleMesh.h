#ifndef TRIANGLEMESH_H
#define TRIANGLEMESH_H

// OpenGL and FreeGlut headers.
#include <GL/glew.h>
#include <GL/freeglut.h>

// GLM.
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

// C++ STL headers.
#include <bits/stdc++.h>

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

// TriangleMesh Declarations.
class TriangleMesh
{
public:
	// TriangleMesh Public Methods.
	TriangleMesh();
	~TriangleMesh();
	
	// Load the model from an *.OBJ file.
	bool LoadFromFile(const std::filesystem::path& filePath, const bool normalized = true);
	
	// Create vertex and index buffers.
	void CreateBuffers();

	// Apply transform on CPU.
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


#endif
