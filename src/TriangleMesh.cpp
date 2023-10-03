#include "TriangleMesh.h"

// C++ STL headers.
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>

using namespace opengl_homework;

// Desc: Constructor of a triangle mesh.
TriangleMesh::TriangleMesh(const std::filesystem::path& filePath, const bool normalized = true) {
	name = filePath.stem().string();
	numVertices = 0;
	numTriangles = 0;
	objCenter = glm::vec3(0.0f, 0.0f, 0.0f);
	LoadFromFile(filePath, normalized);
}

// Desc: Destructor of a triangle mesh.
TriangleMesh::~TriangleMesh() {
	vertices.clear();
	vertexIndices.clear();
	glDeleteBuffers(1, &vboId);
	glDeleteBuffers(1, &iboId);
}

// Desc: Load the geometry data of the model from file and normalize it.
bool TriangleMesh::LoadFromFile(const std::filesystem::path& filePath, const bool normalized) {
	std::ifstream fin(filePath);
	if (!fin) {
		std::cerr << "Error: cannot open file " << filePath << std::endl;
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
			std::string v1, v2, v3;
			iss >> v1 >> v2 >> v3;
			std::istringstream iss1(v1);
			std::istringstream iss2(v2);
			std::istringstream iss3(v3);
			std::string v1p, v1t, v1n;
			std::string v2p, v2t, v2n;
			std::string v3p, v3t, v3n;
			std::getline(iss1, v1p, '/');
			std::getline(iss1, v1t, '/');
			std::getline(iss1, v1n, '/');
			std::getline(iss2, v2p, '/');
			std::getline(iss2, v2t, '/');
			std::getline(iss2, v2n, '/');
			std::getline(iss3, v3p, '/');
			std::getline(iss3, v3t, '/');
			std::getline(iss3, v3n, '/');
			VertexPTN vertex1, vertex2, vertex3;
			vertex1.position = positions[std::stoi(v1p) - 1];
			vertex1.texcoord = texcoords[std::stoi(v1t) - 1];
			vertex1.normal = normals[std::stoi(v1n) - 1];
			vertex2.position = positions[std::stoi(v2p) - 1];
			vertex2.texcoord = texcoords[std::stoi(v2t) - 1];
			vertex2.normal = normals[std::stoi(v2n) - 1];
			vertex3.position = positions[std::stoi(v3p) - 1];
			vertex3.texcoord = texcoords[std::stoi(v3t) - 1];
			vertex3.normal = normals[std::stoi(v3n) - 1];
			vertices.push_back(vertex1);
			vertices.push_back(vertex2);
			vertices.push_back(vertex3);
			vertexIndices.push_back(numVertices);
			vertexIndices.push_back(numVertices + 1);
			vertexIndices.push_back(numVertices + 2);
			numVertices += 3;
			numTriangles += 1;
		}
	}

	fin.close();

	if (normalized) {
		// Normalize the model.
		glm::vec3 minPos = glm::vec3(1e9, 1e9, 1e9);
		glm::vec3 maxPos = glm::vec3(-1e9, -1e9, -1e9);
		for (int i = 0; i < numVertices; ++i) {
			minPos = glm::min(minPos, vertices[i].position);
			maxPos = glm::max(maxPos, vertices[i].position);
		}
		objCenter = minPos + (maxPos - minPos) * 0.5f;
		float maxLen = std::max(maxPos.x - minPos.x, std::max(maxPos.y - minPos.y, maxPos.z - minPos.z));
		for (int i = 0; i < numVertices; ++i) {
			vertices[i].position = (vertices[i].position - objCenter) / maxLen;
		}
	}

	PrintMeshInfo();
	return true;
}

// Desc: Create vertex buffer and index buffer.
void TriangleMesh::CreateBuffers() {
	glGenBuffers(1, &vboId);
	glBindBuffer(GL_ARRAY_BUFFER, vboId);
	glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(VertexPTN), vertices.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &iboId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertexIndices.size() * sizeof(unsigned int), vertexIndices.data(), GL_STATIC_DRAW);
}

// Desc: Render the mesh.
void TriangleMesh::Render() const {
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPTN), (void*)offsetof(VertexPTN, position));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPTN), (void*)offsetof(VertexPTN, normal));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexPTN), (void*)offsetof(VertexPTN, texcoord));

	glDrawElements(GL_TRIANGLES, vertexIndices.size(), GL_UNSIGNED_INT, 0);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
}

// Desc: Apply transformation to all vertices (DON'T NEED TO TOUCH)
void TriangleMesh::ApplyTransformCPU(const glm::mat4x4& mvpMatrix) {
	for (int i = 0; i < numVertices; ++i) {
		glm::vec4 p = mvpMatrix * glm::vec4(vertices[i].position, 1.0f);
		if (p.w != 0.0f) {
			float inv = 1.0f / p.w;
			vertices[i].position = p * inv;
		}
	}
}

// Desc: Print mesh information.
void TriangleMesh::PrintMeshInfo() const {
	std::cout << "[*] Mesh Info: " << name << std::endl;
	std::cout << "# Vertices: " << numVertices << std::endl;
	std::cout << "# Triangles: " << numTriangles << std::endl;
	std::cout << "Center: (" << objCenter.x << " , " << objCenter.y << " , " << objCenter.z << ")" << std::endl;
}
