#include "engine/resource/Material.h"

Material::Material()
{

}

Material::Material(std::shared_ptr<Shader> shader)
{

}

Material::~Material()
{

}

void Material::SetShader(std::shared_ptr<Shader> shader)
{

}

void Material::SetTexture(Texture::TextureType type, std::shared_ptr<Texture> texture)
{

}

void Material::SetProperties(const Properties &props)
{

}

void Material::Bind() const
{

}

void Material::Unbind() const
{

}

std::shared_ptr<Shader> Material::GetTexture(Texture::TextureType type) const
{
	return nullptr;
}

