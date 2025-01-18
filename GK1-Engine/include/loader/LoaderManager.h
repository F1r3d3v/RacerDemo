#pragma once
#include "loader/LoaderFactory.h"
#include "loader/model/ModelLoader.h"
#include "loader/model/OBJLoader.h"

class LoaderManager {
public:

	static void Initialize()
	{
		// Initialize loaders
		LoaderFactory<ModelLoader>::RegisterLoader(".obj", []() { return std::make_shared<OBJLoader>(); });
	}
};