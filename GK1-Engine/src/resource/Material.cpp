#include "resource/Material.h"


Material::Material() : m_properties{}
{
}

Material::Material(std::shared_ptr<Shader> shader) : Material()
{
	SetShader(shader);
}

Material::~Material()
{
}

void Material::SetShader(std::shared_ptr<Shader> shader)
{
	m_shader = shader;
}

void Material::SetTexture(Texture::TextureType type, std::shared_ptr<Texture> texture)
{
	m_textures[type] = texture;
}

void Material::SetProperties(const Properties &props)
{
	m_properties = props;
}

void Material::Bind() const
{
	if (!m_shader) return;

	m_shader->Use();

	// Set material properties
	m_shader->SetVec3("material.ambient", m_properties.ambient);
	m_shader->SetVec3("material.diffuse", m_properties.diffuse);
	m_shader->SetVec3("material.specular", m_properties.specular);
	m_shader->SetFloat("material.shininess", m_properties.shininess);

	// Bind textures to appropriate slots
	int textureSlot = 0;
	for (const auto &[type, texture] : m_textures)
	{
		if (!texture) continue;

		texture->Bind(textureSlot);

		// Set the appropriate uniform based on texture type
		switch (type)
		{
			case Texture::TextureType::Ambient:
				m_shader->SetInt("material.ambientMap", textureSlot);
				break;
			case Texture::TextureType::Diffuse:
				m_shader->SetInt("material.diffuseMap", textureSlot);
				break;
			case Texture::TextureType::Specular:
				m_shader->SetInt("material.specularMap", textureSlot);
				break;
			case Texture::TextureType::Shininess:
				m_shader->SetInt("material.shininessMap", textureSlot);
				break;
			case Texture::TextureType::Normal:
				m_shader->SetInt("material.normalMap", textureSlot);
				break;
			case Texture::TextureType::Height:
				m_shader->SetInt("material.heightMap", textureSlot);
				break;
			default:
				break;
		}

		textureSlot++;
	}
}

void Material::Unbind() const
{
	// Unbind all textures
	for (const auto &[type, texture] : m_textures)
	{
		if (texture)
		{
			texture->Unbind();
		}
	}

	// Unbind shader
	if (m_shader)
	{
		glUseProgram(0);
	}
}

std::shared_ptr<Texture> Material::GetTexture(Texture::TextureType type) const
{
	auto it = m_textures.find(type);
	if (it != m_textures.end())
	{
		return it->second;
	}
	return nullptr;
}
