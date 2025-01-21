#include "Engine/Resource/Material.h"

static constexpr char kDefaultVertexShader[] = R"(
#version 330 core	
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;
flat out vec3 viewPos;

uniform mat4 model;
layout (std140) uniform Matrices
{
    mat4 view;
    mat4 projection;
};

void main()
{
	FragPos = vec3(model * vec4(aPos, 1.0));
	Normal = normalize(mat3(transpose(inverse(model))) * aNormal);
	TexCoords = aTexCoords;
	viewPos = -vec3(view[3]) * mat3(view);
	gl_Position = projection * view * vec4(FragPos, 1.0);
}
)";

static constexpr char kDefaultFragmentShader[] = R"(
#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
flat in vec3 viewPos;
flat in int count;

struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};
uniform Material material;

struct Light {
    vec4 position;
    vec4 direction;	  // w = 0.0 for point lights and w > 0.0 for spot lights focus exponent
    vec4 color;       // RGB + intensity in w
    vec4 attenuation; // x=constant, y=linear, z=quadratic, w=radius
};

layout (std140) uniform Lights
{
    Light pointLights[4];
    Light spotLights[4];
    ivec4 counts;     // x=numPointLights, y=numSpotLights
} lights;

vec3 CalcLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir;
    float attenuation = 1.0;
    float intensity = light.color.w;
    
    lightDir = normalize(light.position.xyz - fragPos);
        
    // Calculate attenuation
    float distance = length(light.position.xyz - fragPos);
    attenuation = 1.0 / (light.attenuation.x + 
                        light.attenuation.y * distance +
                        light.attenuation.z * distance * distance);
        
	if (light.direction.w > 0.0) {
		intensity *= pow(dot(normalize(-light.direction.xyz), lightDir), light.direction.w);
	}
    
    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    
    // Specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    
    // Combine
    vec3 ambient = light.color.rgb * material.ambient;
    vec3 diffuse = light.color.rgb * diff * material.diffuse;
    vec3 specular = light.color.rgb * spec * material.specular;
    
    return (ambient + diffuse + specular) * attenuation * intensity;
}

void main()
{
	vec3 result = vec3(0.0);

	vec3 viewDir = normalize(viewPos - FragPos);
    
	// Calculate lighting
    for(int i = 0; i < lights.counts.x; i++) {
        result += CalcLight(lights.pointLights[i], Normal, FragPos, viewDir);
    }
    for(int i = 0; i < lights.counts.y; i++) {
        result += CalcLight(lights.spotLights[i], Normal, FragPos, viewDir);
    }
    
	FragColor = vec4(result, 1.0);
}
)";

Material::Material() : m_properties{}
{
	auto shader = Shader::LoadFromString(kDefaultVertexShader, kDefaultFragmentShader);
	unsigned int uniformMatricesBlockIndex = glGetUniformBlockIndex(shader->GetID(), "Matrices");
	glUniformBlockBinding(shader->GetID(), uniformMatricesBlockIndex, 0);
	unsigned int uniformLightsBlockIndex = glGetUniformBlockIndex(shader->GetID(), "Lights");
	glUniformBlockBinding(shader->GetID(), uniformLightsBlockIndex, 1);
	SetShader(shader);
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
