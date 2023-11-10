#pragma once

#include <string>
#include <glm/glm.hpp>
#include <GL/glew.h>

// ShaderProg Declarations.
class ShaderProg
{
public:
	// ShaderProg Public Methods.
	ShaderProg();
	~ShaderProg();

	bool LoadFromFiles(const std::string vsFilePath, const std::string fsFilePath);
	void Bind() { glUseProgram(shaderProgId); };
	void Unbind() { glUseProgram(0); };

	GLint GetLocMVP() const { return locMVP; }

protected:
	// ShaderProg Protected Methods.
	virtual void GetUniformVariableLocation();

	// ShaderProg Protected Data.
	GLuint shaderProgId;

private:
	// ShaderProg Private Methods.
	GLuint AddShader(const std::string& sourceText, GLenum shaderType);
	static bool LoadShaderTextFromFile(const std::string filePath, std::string& sourceText);

	// ShaderProg Private Data.
	GLint locMVP;
};

// ------------------------------------------------------------------------------------------------

// FillColorShaderProg Declarations.
class FillColorShaderProg : public ShaderProg
{
public:
	// FillColorShaderProg Public Methods.
	FillColorShaderProg();
	~FillColorShaderProg();

	GLint GetLocFillColor() const { return locFillColor; }

protected:
	// FillColorShaderProg Protected Methods.
	void GetUniformVariableLocation();

private:
	// FillColorShaderProg Private Data.
	GLint locFillColor;
};

// ------------------------------------------------------------------------------------------------

// GouraudShadingDemoShaderProg Declarations.
class GouraudShadingDemoShaderProg : public ShaderProg
{
public:
	// GouraudShadingDemoShaderProg Public Methods.
	GouraudShadingDemoShaderProg();
	~GouraudShadingDemoShaderProg();

	GLint GetLocM() const { return locM; }
	GLint GetLocV() const { return locV; }
	GLint GetLocNM() const { return locNM; }
	GLint GetLocKa() const { return locKa; }
	GLint GetLocKd() const { return locKd; }
	GLint GetLocKs() const { return locKs; }
	GLint GetLocNs() const { return locNs; }
	GLint GetLocAmbientLight() const { return locAmbientLight; }
	GLint GetLocDirLightDir() const { return locDirLightDir; }
	GLint GetLocDirLightRadiance() const { return locDirLightRadiance; }
	GLint GetLocPointLightPos() const { return locPointLightPos; }
	GLint GetLocPointLightIntensity() const { return locPointLightIntensity; }
	
protected:
	// GouraudShadingDemoShaderProg Protected Methods.
	void GetUniformVariableLocation();

private:
	// GouraudShadingDemoShaderProg Public Data.
	// Transformation matrix.
	GLint locM;
	GLint locV;
	GLint locNM;
	// Material properties.
	GLint locKa;
	GLint locKd;
	GLint locKs;
	GLint locNs;
	// Light data.
	GLint locAmbientLight;
	GLint locDirLightDir;
	GLint locDirLightRadiance;
	GLint locPointLightPos;
	GLint locPointLightIntensity;
};
