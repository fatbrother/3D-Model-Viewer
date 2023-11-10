#include "TriangleMesh.h"

// OpenGL and FreeGlut headers.
#include <GL/glew.h>
#include <GL/freeglut.h>

// GLM headers.
#include <glm/gtc/type_ptr.hpp>

// C++ STL headers.
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

namespace opengl_homework {

// VertexPTN Declarations.
struct TriangleMesh::VertexPTN {
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

// TriangleMesh Private Declarations.
struct TriangleMesh::Impl {
	GLuint vboId;
	GLuint iboId;
	std::vector<VertexPTN> vertices;
	std::vector<unsigned int> vertexIndices;

	std::string name;
	int numVertices;
	int numTriangles;
	glm::vec3 objCenter;
	glm::vec3 objExtent;
};

// Desc: Get the number of vertices.
int TriangleMesh::GetNumVertices() const {
	return pImpl->numVertices;
}

// Desc: Get the number of triangles.
int TriangleMesh::GetNumTriangles() const {
	return pImpl->numTriangles;
}

// Desc: Get the number of indices.
int TriangleMesh::GetNumIndices() const {
	return pImpl->vertexIndices.size();
}

// Desc: Get the center of the model.
glm::vec3 TriangleMesh::GetObjCenter() const {
	return pImpl->objCenter;
}

// Desc: Constructor of a triangle mesh.
TriangleMesh::TriangleMesh(const std::filesystem::path& objFilePath, const bool normalized = true) {
	pImpl = std::make_unique<Impl>();
	pImpl->name = objFilePath.stem().string();
	pImpl->numVertices = 0;
	pImpl->numTriangles = 0;
	pImpl->objCenter = glm::vec3(0.0f, 0.0f, 0.0f);
	LoadFromFile(objFilePath, normalized);
}

// Desc: Destructor of a triangle mesh.
TriangleMesh::~TriangleMesh() {
	pImpl->vertices.clear();
	pImpl->vertexIndices.clear();
	ReleaseBuffers();
}

// Desc: Load the geometry data of the model from file and normalize it.
bool TriangleMesh::LoadFromFile(const std::filesystem::path& objFilePath, const bool normalized) {
	std::ifstream fin(objFilePath);
	if (!fin) {
		std::cerr << "Error: cannot open file " << objFilePath << std::endl;
		return false;
	}

	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> texcoords;

	std::string line = "";
	while (std::getline(fin, line)) {
		std::istringstream iss(line);
		std::string type;
		iss >> type;
		if (type == "v") {
			float x, y, z;
			iss >> x >> y >> z;
			positions.emplace_back(x, y, z);
		}
		else if (type == "vn") {
			float x, y, z;
			iss >> x >> y >> z;
			normals.emplace_back(x, y, z);
		}
		else if (type == "vt") {
			float u, v;
			iss >> u >> v;
			texcoords.emplace_back(u, v);
		}
		else if (type == "f") {
			int numVertices = 0;
			std::string token;
			while (iss >> token) {
				std::istringstream viss(token);
				std::string posIndexStr, texcoordIndexStr, normalIndexStr;
				std::getline(viss, posIndexStr, '/');
				std::getline(viss, texcoordIndexStr, '/');
				std::getline(viss, normalIndexStr, '/');
				int posIndex = std::stoi(posIndexStr) - 1;
				int texcoordIndex = std::stoi(texcoordIndexStr) - 1;
				int normalIndex = std::stoi(normalIndexStr) - 1;
				pImpl->vertices.emplace_back(positions[posIndex], normals[normalIndex], texcoords[texcoordIndex]);
				++numVertices;
			}

			// Triangulate the polygon.
			for (int i = 2; i < numVertices; ++i) {
				pImpl->vertexIndices.push_back(pImpl->numVertices);
				pImpl->vertexIndices.push_back(pImpl->numVertices + i - 1);
				pImpl->vertexIndices.push_back(pImpl->numVertices + i);
			}
			pImpl->numVertices += numVertices;
			pImpl->numTriangles += numVertices - 2;
		}
	}

	fin.close();

	if (normalized) {
		// Normalize the model.
		glm::vec3 minPos = glm::vec3(1e9, 1e9, 1e9);
		glm::vec3 maxPos = glm::vec3(-1e9, -1e9, -1e9);
		for (int i = 0; i < pImpl->numVertices; ++i) {
			minPos = glm::min(minPos, pImpl->vertices[i].position);
			maxPos = glm::max(maxPos, pImpl->vertices[i].position);
		}
		pImpl->objCenter = minPos + (maxPos - minPos) * 0.5f;
		float maxLen = std::max(maxPos.x - minPos.x, std::max(maxPos.y - minPos.y, maxPos.z - minPos.z));
		for (int i = 0; i < pImpl->numVertices; ++i) {
			pImpl->vertices[i].position = (pImpl->vertices[i].position - pImpl->objCenter) / maxLen;
		}
		pImpl->objExtent = (maxPos - minPos) / maxLen;
	}

	PrintMeshInfo();
	return true;
}

// Desc: Create vertex buffer and index buffer.
void TriangleMesh::CreateBuffers() {
	glGenBuffers(1, &(pImpl->vboId));
	glBindBuffer(GL_ARRAY_BUFFER, pImpl->vboId);
	glBufferData(GL_ARRAY_BUFFER, pImpl->vertices.size() * sizeof(VertexPTN), pImpl->vertices.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &(pImpl->iboId));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pImpl->iboId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, pImpl->vertexIndices.size() * sizeof(unsigned int), pImpl->vertexIndices.data(), GL_STATIC_DRAW);
}

// Desc: Release vertex buffer and index buffer.
void TriangleMesh::ReleaseBuffers() {
	glDeleteBuffers(1, &(pImpl->vboId));
	glDeleteBuffers(1, &(pImpl->iboId));
}

// Desc: Render the mesh.
void TriangleMesh::Render() const {
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, pImpl->vboId);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPTN), (void*)offsetof(VertexPTN, position));
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, pImpl->iboId);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPTN), (void*)offsetof(VertexPTN, normal));
	// glEnableVertexAttribArray(2);
	// glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexPTN), (void*)offsetof(VertexPTN, texcoord));

	glDrawElements(GL_TRIANGLES, pImpl->vertexIndices.size(), GL_UNSIGNED_INT, 0);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	// glDisableVertexAttribArray(2);
}

// Desc: Print mesh information.
void TriangleMesh::PrintMeshInfo() const {
	std::cout << "[*] Mesh Info: " << pImpl->name << std::endl;
	std::cout << "# Vertices: " << pImpl->numVertices << std::endl;
	std::cout << "# Triangles: " << pImpl->numTriangles << std::endl;
	std::cout << "Center: (" << pImpl->objCenter.x << " , "
		<< pImpl->objCenter.y << " , " << pImpl->objCenter.z << ")" << std::endl;
	std::cout << "Extent: (" << pImpl->objExtent.x << " , "
		<< pImpl->objExtent.y << " , " << pImpl->objExtent.z << ")" << std::endl;
}

} // namespace opengl_homework