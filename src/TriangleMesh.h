#pragma once

// GLM headers.
#include <glm/glm.hpp>

// C++ STL headers.
#include <filesystem>

namespace opengl_homework {

/**
 * @brief TriangleMesh class.
*/
class TriangleMesh
{
public:
	// TriangleMesh Public Methods.
	TriangleMesh(const std::filesystem::path&, const bool);
	~TriangleMesh();

	/**
	 * @brief Create buffers for rendering.
	*/
	void CreateBuffers();

	/**
	 * @brief Release buffers.
	*/
	void ReleaseBuffers();

	/**
	 * @brief Render the mesh.
	*/
	void Render() const;

	int GetNumVertices() const;
	int GetNumTriangles() const;
	int GetNumIndices() const;
	glm::vec3 GetObjCenter() const;
	
	void PrintMeshInfo() const;

private:

	// VertexPTN Declarations.
	struct VertexPTN;

	/**
	 * @brief TriangleMesh Private Declarations.
	 * @details This struct is used to hide the implementation 
	 * details of TriangleMesh and remove the dependency on 
	 * libraries to speed up compilation.
	 * 
	 * @note This is a common technique to hide implementation
	*/
	struct Impl;
	std::unique_ptr<Impl> pImpl;

	/**
	 * @brief Load a model from obj file.
	 *
	 * @param objFilePath Path to the obj file.
	 * @param normalized Normalize the model to fit in a unit cube.
	 *
	 * @return true if the model is loaded successfully.
	*/
	bool LoadFromFile(const std::filesystem::path&, const bool);

};

}