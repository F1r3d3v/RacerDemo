#include "engine/resource/Model.h"
#include "engine/loader/LoaderFactory.h"
#include "engine/loader/model/ModelLoader.h"
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

	// Create the loader
	auto factory = LoaderFactory<ModelLoader>::CreateLoader(extension);
	if (!factory)
	{
		std::cerr << "Failed to find loader for extension: " << extension << std::endl;
		return nullptr;
	}

	return factory->Load(path);
}

void Model::Draw() const
{
	for (const auto &meshComponent : m_meshes)
	{
		if (meshComponent.material)
		{
			meshComponent.material->Bind();
		}
		if (meshComponent.mesh)
		{
			meshComponent.mesh->Draw();
		}
		if (meshComponent.material)
		{
			meshComponent.material->Unbind();
		}
	}
}

void Model::AddMeshComponent(const MeshComponent &component)
{
	m_meshes.push_back(component);
}