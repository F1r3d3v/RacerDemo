#include "engine/resource/Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <iostream>

Texture::Texture() : m_textureID(0), m_width(0), m_height(0), m_channels(0),
m_type(TextureType::Diffuse), m_format(GL_RGB), m_internalFormat(GL_RGB)
{
}

Texture::~Texture()
{
	glDeleteTextures(1, &m_textureID);
}

std::shared_ptr<Texture> Texture::LoadFromFile(const std::string &path, Texture::TextureType type, bool generateMipMaps)
{
	auto texture = std::make_shared<Texture>();
	texture->m_type = type;

	stbi_set_flip_vertically_on_load(true);
	unsigned char *data = stbi_load(path.c_str(), &texture->m_width, &texture->m_height,
									&texture->m_channels, 0);

	if (!data)
	{
		std::cerr << "Failed to load texture: " << path << std::endl;
		return nullptr;
	}

	texture->m_format = texture->m_channels == 4 ? GL_RGBA : GL_RGB;
	texture->m_internalFormat = texture->m_format;

	glGenTextures(1, &texture->m_textureID);
	glBindTexture(GL_TEXTURE_2D, texture->m_textureID);

	glTexImage2D(GL_TEXTURE_2D, 0, texture->m_internalFormat, texture->m_width,
				 texture->m_height, 0, texture->m_format, GL_UNSIGNED_BYTE, data);

	if (generateMipMaps)
	{
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, generateMipMaps ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	stbi_image_free(data);
	return texture;
}

std::shared_ptr<Texture> Texture::LoadFromData(const unsigned char *data, int width, int height,
											   int channels, Texture::TextureType type, bool generateMipMaps)
{
	auto texture = std::make_shared<Texture>();

	texture->m_width = width;
	texture->m_height = height;
	texture->m_channels = channels;
	texture->m_format = channels == 4 ? GL_RGBA : GL_RGB;
	texture->m_internalFormat = texture->m_format;

	glGenTextures(1, &texture->m_textureID);
	glBindTexture(GL_TEXTURE_2D, texture->m_textureID);

	glTexImage2D(GL_TEXTURE_2D, 0, texture->m_internalFormat, width, height, 0,
				 texture->m_format, GL_UNSIGNED_BYTE, data);

	if (generateMipMaps)
	{
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, generateMipMaps ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return texture;
}

std::shared_ptr<Texture> Texture::CreateCubemap(const std::vector<std::string> &faces)
{
	auto texture = std::make_shared<Texture>();
	texture->m_type = TextureType::Cubemap;

	glGenTextures(1, &texture->m_textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture->m_textureID);

	for (unsigned int i = 0; i < faces.size(); i++)
	{
		int width, height, channels;
		unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &channels, 0);

		if (!data)
		{
			std::cerr << "Failed to load cubemap texture: " << faces[i] << std::endl;
			continue;
		}

		GLenum format = channels == 4 ? GL_RGBA : GL_RGB;
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height,
					 0, format, GL_UNSIGNED_BYTE, data);
		stbi_image_free(data);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return texture;
}

void Texture::Bind(unsigned int slot) const
{
	glActiveTexture(GL_TEXTURE0 + slot);
	if (m_type == TextureType::Cubemap)
	{
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureID);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D, m_textureID);
	}
}

void Texture::Unbind() const
{
	if (m_type == TextureType::Cubemap)
	{
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}