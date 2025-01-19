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
	for (const auto &meshComponent : m_meshes)
	{
		if (meshComponent->material)
		{
			meshComponent->material->Bind();
		}
		if (meshComponent->mesh)
		{
			meshComponent->mesh->Draw();
		}
		if (meshComponent->material)
		{
			meshComponent->material->Unbind();
		}
	}
}

void Model::AddMeshComponent(const MeshComponent &component)
{
	m_meshes.push_back(std::make_shared<MeshComponent>(component));
}

std::shared_ptr<Model::MeshComponent> Model::GetMeshComponent(const std::string &name) const
{
	auto it = std::find_if(m_meshes.begin(), m_meshes.end(),
		[&name](std::shared_ptr<MeshComponent> entry)
		{
			return entry->name == name;
		}
	);

	if (it != m_meshes.end())
	{
		return *it;
	}

	return nullptr;
}
