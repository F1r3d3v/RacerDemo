#include "Engine/Objects/Model.h"
#include "Engine/Loader/LoaderFactory.h"
#include "Engine/Loader/Model/ModelLoader.h"
#include <iostream>
#include <filesystem>

Model::Model()
{
}

Model::~Model()
{
}

std::shared_ptr<Model> Model::LoadFromFile(const std::string &path)
{
	// Get file extension
	std::string extension = std::filesystem::path(path).extension().string();

	// Create the mesh loader
	auto factory = LoaderFactory<ModelLoader>::CreateLoader(extension);
	if (!factory)
	{
		std::cerr << "Failed to find loader for extension: " << extension << std::endl;
		return nullptr;
	}

	auto model = factory->Load(path);
	if (!model)
	{
		std::cerr << "Failed to load model: " << path << std::endl;
		return nullptr;
	}

	return model;
}

void Model::Draw()
{
	for (const auto &mesh : m_meshes)
	{
		auto shader = mesh->GetMaterial()->GetShader();
		if (shader)
		{
			shader->Use();
			shader->SetMat4("model", GetWorldMatrix() * mesh->GetModelMatrix());
		}
		mesh->Draw();
	}
}

void Model::AddMesh(const Mesh &mesh)
{
	m_meshes.push_back(std::make_shared<Mesh>(mesh));
}

std::shared_ptr<Mesh> Model::GetMesh(const std::string &name) const
{
	auto it = std::find_if(m_meshes.begin(), m_meshes.end(),
		[&name](std::shared_ptr<Mesh> entry)
		{
			return entry->GetName() == name;
		}
	);

	if (it != m_meshes.end())
	{
		return *it;
	}

	return nullptr;
}

glm::vec3 Model::GetMinBounds() const
{
	glm::vec3 minBounds = glm::vec3(FLT_MAX);
	for (const auto &mesh : m_meshes)
	{
		glm::vec3 min = mesh->GetMinBounds();
		if (min.x < minBounds.x)
			minBounds.x = min.x;
		if (min.y < minBounds.y)
			minBounds.y = min.y;
		if (min.z < minBounds.z)
			minBounds.z = min.z;
	}
	return minBounds;
}

glm::vec3 Model::GetMaxBounds() const
{
	glm::vec3 maxBounds = glm::vec3(-FLT_MAX);
	for (const auto &mesh : m_meshes)
	{
		glm::vec3 max = mesh->GetMaxBounds();
		if (max.x > maxBounds.x)
			maxBounds.x = max.x;
		if (max.y > maxBounds.y)
			maxBounds.y = max.y;
		if (max.z > maxBounds.z)
			maxBounds.z = max.z;
	}
	return maxBounds;
}
