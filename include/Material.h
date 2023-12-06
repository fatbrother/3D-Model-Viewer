#pragma once

#include <string>
#include <glm/glm.hpp>

#include "ShaderProg.h"
#include "ImageTexture.h"

// Material Declarations.
class Material
{
public:
	// Material Public Methods.
	Material() { name = "Default"; };
	~Material() {};

	void SetName(const std::string mtlName) { name = mtlName; }
	std::string GetName() const { return name; }

protected:	
	// Material Protected Data.
	std::string name;
};

// ------------------------------------------------------------------------------------------------

// PhongMaterial Declarations.
class PhongMaterial : public Material
{
public:
	// PhongMaterial Public Methods.
	PhongMaterial() {
		Ka = glm::vec3(0.0f, 0.0f, 0.0f);
		Kd = glm::vec3(0.0f, 0.0f, 0.0f);
		Ks = glm::vec3(0.0f, 0.0f, 0.0f);
		Ns = 0.0f;
	};
	~PhongMaterial() {};

	void SetKa(const glm::vec3 ka) { Ka = ka; }
	void SetKd(const glm::vec3 kd) { Kd = kd; }
	void SetKs(const glm::vec3 ks) { Ks = ks; }
	void SetNs(const float n) { Ns = n; }

	const glm::vec3 GetKa() const { return Ka; }
	const glm::vec3 GetKd() const { return Kd; }
	const glm::vec3 GetKs() const { return Ks; }
	const float GetNs() const { return Ns; }

private:
	// PhongMaterial Private Data.
	glm::vec3 Ka;
	glm::vec3 Kd;
	glm::vec3 Ks;
	float Ns;
};

// ------------------------------------------------------------------------------------------------

// SkyboxMaterial Declarations.
class SkyboxMaterial : public Material
{
public:
	// SkyboxMaterial Public Methods.
	SkyboxMaterial() {
		mapKd = nullptr;
	};
	~SkyboxMaterial() {};
	void SetMapKd(std::shared_ptr<ImageTexture> tex) { mapKd = tex; }
	std::shared_ptr<ImageTexture> GetMapKd() const { return mapKd; }

private:
	// SkyboxMaterial Private Data.
	std::shared_ptr<ImageTexture> mapKd;
};