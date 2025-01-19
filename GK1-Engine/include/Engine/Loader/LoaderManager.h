#pragma once
#include "Engine/Loader/LoaderFactory.h"

#include "Engine/Loader/Model/ModelLoader.h"
#include "Engine/Loader/Model/OBJLoader.h"

#include "Engine/Loader/Material/MaterialLoader.h"
#include "Engine/Loader/Material/MTLLoader.h"

class LoaderManager {
public:

	static void Initialize()
	{
		// Initialize loaders
		LoaderFactory<ModelLoader>::RegisterLoader(".obj", []() { return std::make_shared<OBJLoader>(); });
		LoaderFactory<MaterialLoader>::RegisterLoader(".mtl", []() { return std::make_shared<MTLLoader>(); });
	}
};