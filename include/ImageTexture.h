#pragma once

// C++ STL headers.
#include <string>
#include <filesystem>

// OpenCV headers.
#include <opencv2/opencv.hpp>

// OpenGL headers.
#include <GL/glew.h>

// Texture Declarations.
class ImageTexture
{
public:
	// Texture Public Methods.
	ImageTexture(const std::filesystem::path& texImagePath);
	~ImageTexture();

	void Bind(GLenum textureUnit);
	void Preview();
	std::filesystem::path GetTexFilePath() const { return texFilePath; }

private:
	// Texture Private Data.
	std::filesystem::path texFilePath;
	GLuint textureObj;
	int imageWidth;
	int imageHeight;
	int numChannels;
	cv::Mat texImage;
};
