#pragma once
#include "Engine/Objects/Model.h"
#include <string>
#include <memory>

class ModelLoader {
public:
    virtual ~ModelLoader() = default;
    virtual std::shared_ptr<Model> Load(const std::string& filePath) = 0;
};