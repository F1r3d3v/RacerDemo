#include "Engine/Resource/Material.h"

static constexpr char kDefaultVertexShader[] = R"(
#version 330 core	
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	FragPos = vec3(model * vec4(aPos, 1.0));
	Normal = mat3(transpose(inverse(model))) * aNormal;
	TexCoords = aTexCoords;
	gl_Position = projection * view * vec4(FragPos, 1.0);
}
)";

static constexpr char kDefaultFragmentShader[] = R"(
#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

uniform Material material;

void main()
{
	FragColor = vec4(material.diffuse, 1.0);
}
)";

Material::Material() : m_properties{}
{
	SetShader(Shader::LoadFromString(kDefaultVertexShader, kDefaultFragmentShader));
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

void Material::SetTextures(Texture::TextureType type, std::vector<std::shared_ptr<Texture>> textures)
{
	m_textures[type] = textures;
}

void Material::AddTexture(Texture::TextureType type, std::shared_ptr<Texture> texture)
{
	m_textures[type].push_back(texture);
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
	std::unordered_map<Texture::TextureType, int> textureCount;
	for (const auto &[type, textures] : m_textures)
	{
		if (textures.empty()) continue;

		for (const auto &texture : textures)
		{
			texture->Bind(textureSlot);

			// Set the appropriate uniform based on texture type
			switch (type)
			{
			case Texture::TextureType::Ambient:
				m_shader->SetInt("material.ambientMap" + std::to_string(textureCount[type]), textureSlot);
				break;
			case Texture::TextureType::Diffuse:
				m_shader->SetInt("material.diffuseMap" + std::to_string(textureCount[type]), textureSlot);
				break;
			case Texture::TextureType::Specular:
				m_shader->SetInt("material.specularMap" + std::to_string(textureCount[type]), textureSlot);
				break;
			case Texture::TextureType::Shininess:
				m_shader->SetInt("material.shininessMap" + std::to_string(textureCount[type]), textureSlot);
				break;
			case Texture::TextureType::Normal:
				m_shader->SetInt("material.normalMap" + std::to_string(textureCount[type]), textureSlot);
				break;
			case Texture::TextureType::Height:
				m_shader->SetInt("material.heightMap" + std::to_string(textureCount[type]), textureSlot);
				break;
			default:
				break;
			}

			textureSlot++;
			textureCount[type]++;
		}
	}
}

void Material::Unbind() const
{
	// Unbind all textures
	for (const auto &[type, textures] : m_textures)
	{
		if (!textures.empty())
		{
			for (const auto &texture : textures)
			{
				texture->Unbind();
			}
		}
	}

	// Unbind shader
	if (m_shader)
	{
		glUseProgram(0);
	}
}

std::vector<std::shared_ptr<Texture>> Material::GetTextures(Texture::TextureType type) const
{
	auto it = m_textures.find(type);
	if (it != m_textures.end())
	{
		return it->second;
	}
	return {};
}
