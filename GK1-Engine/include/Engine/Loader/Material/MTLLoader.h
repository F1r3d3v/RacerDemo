#pragma once
#include "Engine/Loader/Material/MaterialLoader.h"
#include <string>
#include <unordered_map>
#include <memory>

class MTLLoader : public MaterialLoader {
public:
	struct MTLData {
		std::unordered_map<std::string, std::shared_ptr<Material>> materials;
		bool success;
		std::string error;
	};

	std::vector<std::shared_ptr<Material>> Load(const std::string &filePath) override;

private:
	MTLData LoadMTL(const std::string &path);
	glm::vec3 ParseVec3(const std::string &line);
	float ParseFloat(const std::string &line);
	std::string ParseTexture(const std::string &line, const std::string &mtlPath);
};