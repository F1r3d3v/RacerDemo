#pragma once
#include "Engine/Resource/Resource.h"
#include "Engine/Resource/Mesh.h"
#include "Engine/Resource/Material.h"
#include "Engine/Objects/GraphicsObject.h"
#include <memory>
#include <vector>

class Model : public GraphicsObject {
public:

	struct MeshComponent
	{
		std::string name;
		std::shared_ptr<Mesh> mesh;
		std::shared_ptr<Material> material;
	};

	Model();
	~Model() override;

	static std::shared_ptr<Model> LoadFromFile(const std::string &path);
	void Draw() override;

	void AddMeshComponent(const MeshComponent &entry);
	const std::vector<std::shared_ptr<MeshComponent>> &GetMeshes() const { return m_meshes; }
	std::shared_ptr<MeshComponent> GetMeshComponent(const std::string &name) const;

private:
	std::vector<std::shared_ptr<MeshComponent>> m_meshes;
};