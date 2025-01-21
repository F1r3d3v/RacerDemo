#pragma once
#include "Engine/Resource/Resource.h"
#include <glad/gl.h>
#include <memory>
#include <string>
#include <vector>

class Texture : public Resource {
public:

	enum class TextureType {
		Ambient,
		Diffuse,
		Specular,
		Shininess,
		Normal,
		Height,
		Cubemap
	};

	Texture();
	~Texture();

	static std::shared_ptr<Texture> LoadFromFile(const std::string &path, TextureType type = TextureType::Diffuse, bool generateMipMaps = true);
	static std::shared_ptr<Texture> LoadFromData(const uint8_t *data, int width, int height, int channels, TextureType type = TextureType::Diffuse, bool generateMipMaps = true);
	static std::shared_ptr<Texture> CreateCubemap(const std::vector<std::string> &faces);

	void Bind(unsigned int slot = 0) const;
	void Unbind() const;

	uint8_t *GetData() const
	{
		return m_data;
	}

	GLuint GetID() const
	{
		return m_textureID;
	}
	int GetWidth() const
	{
		return m_width;
	}
	int GetHeight() const
	{
		return m_height;
	}
	TextureType GetType() const
	{
		return m_type;
	}
	void SetType(TextureType type)
	{
		m_type = type;
	}

	static std::shared_ptr<Texture> GetDefaultTexture(bool normalmap = false);
private:

	GLuint m_textureID;
	uint8_t *m_data;
	int m_width;
	int m_height;
	int m_channels;
	TextureType m_type;
	GLenum m_format;
	GLenum m_internalFormat;
};