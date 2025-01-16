#pragma once
#include "engine/loader/LoaderFactory.h"
#include "engine/loader/model/ModelLoader.h"
#include "engine/loader/model/OBJLoader.h"

class LoaderManager {
public:

	static void Initialize()
	{
		// Initialize loaders
		LoaderFactory<ModelLoader>::RegisterLoader(".obj", []() { return std::make_shared<OBJLoader>(); });
	}
};