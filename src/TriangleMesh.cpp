#include "TriangleMesh.h"

// Desc: Constructor of a triangle mesh.
TriangleMesh::TriangleMesh()
{
	numVertices = 0;
	numTriangles = 0;
	objCenter = glm::vec3(0.0f, 0.0f, 0.0f);
	vboId = 0;
	iboId = 0;
}

// Desc: Destructor of a triangle mesh.
TriangleMesh::~TriangleMesh()
{
	vertices.clear();
	vertexIndices.clear();
	glDeleteBuffers(1, &vboId);
	glDeleteBuffers(1, &iboId);
}

// Desc: Load the geometry data of the model from file and normalize it.
bool TriangleMesh::LoadFromFile(const std::filesystem::path& filePath, const bool normalized)
{	
	// Add your code here.
	// ... 

	if (normalized) {
		// Normalize the geometry data.	
		// Add your code here.
		// ... 
	}

	PrintMeshInfo();
	return true;
}

// Desc: Create vertex buffer and index buffer.
void TriangleMesh::CreateBuffers()
{
	// Add your code here.
	// ... 
}

// Desc: Apply transformation to all vertices (DON'T NEED TO TOUCH)
void TriangleMesh::ApplyTransformCPU(const glm::mat4x4& mvpMatrix)
{
	for (int i = 0 ; i < numVertices; ++i) {
        glm::vec4 p = mvpMatrix * glm::vec4(vertices[i].position, 1.0f);
        if (p.w != 0.0f) {
            float inv = 1.0f / p.w; 
            vertices[i].position.x = p.x * inv;
            vertices[i].position.y = p.y * inv;
            vertices[i].position.z = p.z * inv;
        }
    }
}

// Desc: Print mesh information.
void TriangleMesh::PrintMeshInfo() const
{
	std::cout << "[*] Mesh Information: " << std::endl;
	std::cout << "# Vertices: " << numVertices << std::endl;
	std::cout << "# Triangles: " << numTriangles << std::endl;
	std::cout << "Center: (" << objCenter.x << " , " << objCenter.y << " , " << objCenter.z << ")" << std::endl;
}
