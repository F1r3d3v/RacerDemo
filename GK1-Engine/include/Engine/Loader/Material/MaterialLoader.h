#pragma once
#include "Engine/Resource/Material.h"
#include <string>
#include <memory>

class MaterialLoader {
public:
	virtual ~MaterialLoader() = default;
	virtual std::vector<std::shared_ptr<Material>> Load(const std::string &filePath) = 0;
};