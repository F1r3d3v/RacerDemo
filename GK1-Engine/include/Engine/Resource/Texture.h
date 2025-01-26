#pragma once
#include "Engine/Resource/Resource.h"
#include <glad/gl.h>
#include <memory>
#include <string>
#include <vector>
#include <glm/glm.hpp>

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

    template <typename T>
    requires (std::is_unsigned<T>::value && std::is_integral<T>::value && sizeof(T) < 8)
    glm::vec4 GetPixel(int x, int y) const
    {
        size_t max_value = (1 << 8 * sizeof(T)) - 1;
        if (m_channels == 1)
        {
			double value = static_cast<double>(reinterpret_cast<T *>(m_data)[y * m_width + x]);
            return glm::vec4((float)(value / max_value));
        }
        else if (m_channels == 3)
        {
			double r = static_cast<double>(reinterpret_cast<T *>(m_data)[(y * m_width + x) * 3]);
			double g = static_cast<double>(reinterpret_cast<T *>(m_data)[(y * m_width + x) * 3 + 1]);
			double b = static_cast<double>(reinterpret_cast<T *>(m_data)[(y * m_width + x) * 3 + 2]);
            return glm::vec4((float)(r / max_value), (float)(g / max_value), b / (float)(max_value), 1.0f);
        }
        else if (m_channels == 4)
        {
			double r = static_cast<double>(reinterpret_cast<T *>(m_data)[(y * m_width + x) * 4]);
			double g = static_cast<double>(reinterpret_cast<T *>(m_data)[(y * m_width + x) * 4 + 1]);
			double b = static_cast<double>(reinterpret_cast<T *>(m_data)[(y * m_width + x) * 4 + 2]);
			double a = static_cast<double>(reinterpret_cast<T *>(m_data)[(y * m_width + x) * 4 + 3]);
            return glm::vec4((float)(r / max_value), (float)(g / max_value), (float)(b / max_value), (float)(a / max_value));
        }
        return glm::vec4(0.0f);
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