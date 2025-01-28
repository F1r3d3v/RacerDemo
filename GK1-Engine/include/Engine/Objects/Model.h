#pragma once
#include "Engine/Resource/Resource.h"
#include "Engine/Objects/Mesh.h"
#include "Engine/Resource/Material.h"
#include "Engine/Objects/GraphicsObject.h"
#include <memory>
#include <vector>

class Model : public GraphicsObject {
public:

	Model();
	~Model() override;

	static std::shared_ptr<Model> LoadFromFile(const std::string &path);
	void Draw() override;

	void AddMesh(const Mesh &entry);
	const std::vector<std::shared_ptr<Mesh>> &GetMeshes() const { return m_meshes; }
	std::shared_ptr<Mesh> GetMesh(const std::string &name) const;

	glm::vec3 GetMinBounds() const;
	glm::vec3 GetMaxBounds() const;

private:
	std::vector<std::shared_ptr<Mesh>> m_meshes;
};