#pragma once
#include "Engine/Geometry.h"
#include "Engine/Resource/Material.h"
#include <string>
#include <memory>

class Mesh : public Resource, public GraphicsObject
{
public:
	std::shared_ptr<Geometry> geometry;
	std::shared_ptr<Material> material;

	Mesh();
	Mesh(std::shared_ptr<Geometry> geometry, std::shared_ptr<Material> material);
	~Mesh();

	void Draw() override;

	std::shared_ptr<Geometry> GetGeometry() const { return geometry; }
	std::shared_ptr<Material> GetMaterial() const { return material; }
	void SetGeometry(std::shared_ptr<Geometry> geometry) { this->geometry = geometry; }
	void SetMaterial(std::shared_ptr<Material> material) { this->material = material; }

};