#include "Engine/Loader/Material/MTLLoader.h"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <iostream>
#include <glm/glm.hpp>

std::vector<std::shared_ptr<Material>> MTLLoader::Load(const std::string &filePath)
{
	auto mtlData = LoadMTL(filePath);
	if (!mtlData.success)
	{
		std::cerr << "Failed to load MTL file: " << mtlData.error << std::endl;
		return {};
	}

	if (mtlData.materials.empty())
	{
		std::cerr << "No materials found in MTL file: " << filePath << std::endl;
		return {};
	}

	auto materials = std::vector<std::shared_ptr<Material>>();

	for (const auto &[name, material] : mtlData.materials)
	{
		materials.push_back(material);
	}

	return materials;
}

MTLLoader::MTLData MTLLoader::LoadMTL(const std::string &path) {
	MTLData result;
	result.success = false;

	std::ifstream file(path);
	if (!file.is_open()) {
		result.error = "Failed to open MTL file: " + path;
		return result;
	}

	std::shared_ptr<Material> currentMaterial = nullptr;
	Material::Properties currentMaterialProps;
	std::string currentMaterialName;

	std::string line;
	while (std::getline(file, line)) {
		if (line.empty() || line[0] == '#') continue;

		std::istringstream iss(line);
		std::string token;
		iss >> token;

		// Start a new material
		if (token == "newmtl")
		{
			if (currentMaterial != nullptr)
				currentMaterial->SetProperties(currentMaterialProps);

			iss >> currentMaterialName;
			currentMaterial = std::make_shared<Material>();
			currentMaterialProps = currentMaterial->GetProperties();
			currentMaterial->SetName(currentMaterialName);
			result.materials[currentMaterialName] = currentMaterial;
		}
		else if (!currentMaterial) 
		{
			continue; // Skip if no material is being parsed
		}
		else if (token == "Ka")
		{
			currentMaterialProps.ambient = ParseVec3(line);
		}
		else if (token == "Kd")
		{
			currentMaterialProps.diffuse = ParseVec3(line);
		}
		else if (token == "Ks")
		{
			currentMaterialProps.specular = ParseVec3(line);
		}
		else if (token == "Ns")
		{
			currentMaterialProps.shininess = ParseFloat(line);
		}
		else if (token == "map_Ka")
		{
			std::string texPath = ParseTexture(line, path);
			if (!texPath.empty()) 
			{
				auto texture = Texture::LoadFromFile(texPath);
				if (texture)
				{
					currentMaterial->AddTexture(Texture::TextureType::Ambient, texture);
				}
			}
		}
		else if (token == "map_Kd")
		{
			std::string texPath = ParseTexture(line, path);
			if (!texPath.empty())
			{
				auto texture = Texture::LoadFromFile(texPath);
				if (texture)
				{
					currentMaterial->AddTexture(Texture::TextureType::Diffuse, texture);
				}
			}
		}
		else if (token == "map_Ks")
		{
			std::string texPath = ParseTexture(line, path);
			if (!texPath.empty())
			{
				auto texture = Texture::LoadFromFile(texPath);
				if (texture)
				{
					currentMaterial->AddTexture(Texture::TextureType::Specular, texture);
				}
			}
		}
		else if (token == "map_Kn" || token == "norm")
		{
			std::string texPath = ParseTexture(line, path);
			if (!texPath.empty())
			{
				auto texture = Texture::LoadFromFile(texPath);
				if (texture)
				{
					currentMaterial->AddTexture(Texture::TextureType::Normal, texture);
				}
			}
		}
	}

	if (currentMaterial != nullptr)
		currentMaterial->SetProperties(currentMaterialProps);

	result.success = true;
	return result;
}

glm::vec3 MTLLoader::ParseVec3(const std::string &line) {
	glm::vec3 result;
	std::istringstream iss(line);
	std::string token;
	iss >> token >> result.x >> result.y >> result.z;
	return result;
}

float MTLLoader::ParseFloat(const std::string &line) {
	float result;
	std::istringstream iss(line);
	std::string token;
	iss >> token >> result;
	return result;
}

std::string MTLLoader::ParseTexture(const std::string &line, const std::string &mtlPath) {
	std::istringstream iss(line);
	std::string token, texPath;
	iss >> token >> texPath;

	// Convert texture path to absolute path relative to MTL file location
	std::filesystem::path mtlDir = std::filesystem::path(mtlPath).parent_path();
	return (mtlDir / texPath).string();
}