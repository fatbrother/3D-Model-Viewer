#pragma once

// GLM headers.
#include <glm/glm.hpp>

// C++ STL headers.
#include <filesystem>

// Project headers.
#include "Light.h"
#include "ShaderProg.h"

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
	void Render(
		const std::unique_ptr<PhongShadingDemoShaderProg>&, 
		const glm::mat4&, 
		const glm::mat4&, 
		const glm::mat4&,
		const glm::vec3&, 
		const std::shared_ptr<DirectionalLight>&,
		const std::shared_ptr<PointLight>&) const;

	int GetNumVertices() const;
	int GetNumTriangles() const;
	int GetNumIndices() const;
	glm::vec3 GetObjCenter() const;

	void PrintMeshInfo() const;

private:

	// VertexPTN Declarations.
	struct VertexPTN;
	struct SubMesh;

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

	/**
	 * @brief Load material library.
	 *
	 * @param mtlFilePath Path to the mtl file.
	 *
	 * @return true if the material library is loaded successfully.
	*/
	bool LoadMtllib(const std::filesystem::path&);

	/**
	 * @brief Render submesh.
	 * @details This function is used to render a submesh.
	 * A submesh is a part of the model that has the same
	 * material.
	 * 
	 * @param subMesh The submesh to render.
	 * 
	 * @note This function is called by Render().
	*/
	void RenderSubMesh(const SubMesh&) const;
};

}