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
#include <map>

// Project headers.
#include "Light.h"
#include "Material.h"

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

// SubMesh Declarations.
struct TriangleMesh::SubMesh
{
	SubMesh() {
		material = nullptr;
		iboId = 0;
	}
	std::shared_ptr<PhongMaterial> material;
	GLuint iboId;
	std::vector<unsigned int> vertexIndices;
};

// TriangleMesh Private Declarations.
struct TriangleMesh::Impl {
	GLuint vboId;
	std::vector<VertexPTN> vertices;
	std::vector<SubMesh> subMeshes;
	std::map<std::string, std::shared_ptr<PhongMaterial>> materials;

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
	// return pImpl->vertexIndices.size();
	return 0;
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
	pImpl->subMeshes.clear();
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
		if (type == "mtllib") {
			std::string mtlFileName;
			iss >> mtlFileName;
			LoadMtllib(objFilePath.parent_path() / mtlFileName);
		}
		else if (type == "v") {
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
				pImpl->subMeshes.back().vertexIndices.push_back(pImpl->numVertices);
				pImpl->subMeshes.back().vertexIndices.push_back(pImpl->numVertices + i - 1);
				pImpl->subMeshes.back().vertexIndices.push_back(pImpl->numVertices + i);
			}
			pImpl->numVertices += numVertices;
			pImpl->numTriangles += numVertices - 2;
		}
		else if (type == "usemtl") {
			std::string mtlName;
			iss >> mtlName;
			pImpl->subMeshes.emplace_back();
			pImpl->subMeshes.back().material = pImpl->materials[mtlName];
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

bool TriangleMesh::LoadMtllib(const std::filesystem::path& mtlPath) {
	std::ifstream fin(mtlPath);
	if (!fin) {
		std::cerr << "Error: cannot open file " << mtlPath << std::endl;
		return false;
	}

	std::string line = "";
	std::string curMtlName = "";
	while (std::getline(fin, line)) {
		std::istringstream iss(line);
		std::string type;
		iss >> type;
		if (type == "newmtl") {
			std::string mtlName;
			iss >> mtlName;
			curMtlName = mtlName;
			pImpl->materials[curMtlName] = std::make_unique<PhongMaterial>();
			pImpl->materials[curMtlName]->SetName(curMtlName);
		}
		else if (type == "Ka") {
			float r, g, b;
			iss >> r >> g >> b;
			pImpl->materials[curMtlName]->SetKa(glm::vec3(r, g, b));
		}
		else if (type == "Kd") {
			float r, g, b;
			iss >> r >> g >> b;
			pImpl->materials[curMtlName]->SetKd(glm::vec3(r, g, b));
		}
		else if (type == "Ks") {
			float r, g, b;
			iss >> r >> g >> b;
			pImpl->materials[curMtlName]->SetKs(glm::vec3(r, g, b));
		}
		else if (type == "Ns") {
			float n;
			iss >> n;
			pImpl->materials[curMtlName]->SetNs(n);
		}
	}

	fin.close();

	return true;
}

// Desc: Create vertex buffer and index buffer.
void TriangleMesh::CreateBuffers() {
	glGenBuffers(1, &(pImpl->vboId));
	glBindBuffer(GL_ARRAY_BUFFER, pImpl->vboId);
	glBufferData(GL_ARRAY_BUFFER, pImpl->vertices.size() * sizeof(VertexPTN), pImpl->vertices.data(), GL_STATIC_DRAW);

	for (auto& subMesh : pImpl->subMeshes) {
		glGenBuffers(1, &(subMesh.iboId));
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, subMesh.iboId);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, subMesh.vertexIndices.size() * sizeof(unsigned int), subMesh.vertexIndices.data(), GL_STATIC_DRAW);
	}
}

// Desc: Release vertex buffer and index buffer.
void TriangleMesh::ReleaseBuffers() {
	glDeleteBuffers(1, &(pImpl->vboId));
	for (auto& subMesh : pImpl->subMeshes) {
		glDeleteBuffers(1, &(subMesh.iboId));
	}
}

// Desc: Render the mesh.
void TriangleMesh::Render(
	const std::unique_ptr<PhongShadingDemoShaderProg>& shader,
	const glm::mat4& MVP,
	const glm::mat4& V,
	const glm::mat4& W,
	const glm::mat4& NM,
	const glm::vec3& ambientLight,
	const std::shared_ptr<DirectionalLight>& dirLight,
	const std::shared_ptr<PointLight>& pointLight, 
	const std::shared_ptr<SpotLight>& spotLight
	) const {

	for (auto& subMesh : pImpl->subMeshes) {
		shader->Bind();
		glUniformMatrix4fv(shader->GetLocM(), 1, GL_FALSE, glm::value_ptr(W));
		glUniformMatrix4fv(shader->GetLocV(), 1, GL_FALSE, glm::value_ptr(V));
		glUniformMatrix4fv(shader->GetLocNM(), 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(W))));
		glUniformMatrix4fv(shader->GetLocMVP(), 1, GL_FALSE, glm::value_ptr(MVP));
		// Material properties.
		glUniform3fv(shader->GetLocKa(), 1, glm::value_ptr(subMesh.material->GetKa()));
		glUniform3fv(shader->GetLocKd(), 1, glm::value_ptr(subMesh.material->GetKd()));
		glUniform3fv(shader->GetLocKs(), 1, glm::value_ptr(subMesh.material->GetKs()));
		glUniform1f(shader->GetLocNs(), subMesh.material->GetNs());
		// Light data.
		if (dirLight != nullptr) {
			glUniform3fv(shader->GetLocDirLightDir(), 1, glm::value_ptr(dirLight->GetDirection()));
			glUniform3fv(shader->GetLocDirLightRadiance(), 1, glm::value_ptr(dirLight->GetRadiance()));
		}
		if (pointLight != nullptr) {
			glUniform3fv(shader->GetLocPointLightPos(), 1, glm::value_ptr(pointLight->GetPosition()));
			glUniform3fv(shader->GetLocPointLightIntensity(), 1, glm::value_ptr(pointLight->GetIntensity()));
		}
		if (spotLight != nullptr) {
			glUniform3fv(shader->GetLocSpotLightPos(), 1, glm::value_ptr(spotLight->GetPosition()));
			glUniform3fv(shader->GetLocSpotLightDir(), 1, glm::value_ptr(spotLight->GetDirection()));
			glUniform3fv(shader->GetLocSpotLightIntensity(), 1, glm::value_ptr(spotLight->GetIntensity()));
			glUniform1f(shader->GetLocSpotLightCutoff(), spotLight->GetCutoffDeg());
			glUniform1f(shader->GetLocSpotLightTotalWidth(), spotLight->GetTotalWidthDeg());
		}

		glUniform3fv(shader->GetLocAmbientLight(), 1, glm::value_ptr(ambientLight));

		RenderSubMesh(subMesh);
		shader->Unbind();
	}
}

// Desc: Render the submesh.
void TriangleMesh::RenderSubMesh(const TriangleMesh::SubMesh& subMesh) const {
	glBindBuffer(GL_ARRAY_BUFFER, pImpl->vboId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, subMesh.iboId);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPTN), (void*)offsetof(VertexPTN, position));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPTN), (void*)offsetof(VertexPTN, normal));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexPTN), (void*)offsetof(VertexPTN, texcoord));

	glDrawElements(GL_TRIANGLES, subMesh.vertexIndices.size(), GL_UNSIGNED_INT, 0);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
}

// Desc: Print mesh information.
void TriangleMesh::PrintMeshInfo() const {
	std::cout << "[*] Mesh Info: " << pImpl->name << std::endl;
	std::cout << "# Vertices: " << pImpl->numVertices << std::endl;
	std::cout << "# Triangles: " << pImpl->numTriangles << std::endl;
	std::cout << "# Submeshes: " << pImpl->subMeshes.size() << std::endl;
	std::cout << "Center: (" << pImpl->objCenter.x << " , "
		<< pImpl->objCenter.y << " , " << pImpl->objCenter.z << ")" << std::endl;
	std::cout << "Extent: (" << pImpl->objExtent.x << " , "
		<< pImpl->objExtent.y << " , " << pImpl->objExtent.z << ")" << std::endl;
}

} // namespace opengl_homework