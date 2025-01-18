#pragma once
#include "resource/Resource.h"
#include "resource/Mesh.h"
#include "resource/Material.h"
#include "GraphicsObject.h"
#include <memory>
#include <vector>

class Model : public Resource, GraphicsObject {
public:

	struct MeshComponent {
		std::shared_ptr<Mesh> mesh;
		std::shared_ptr<Material> material;
	};

	Model();
	~Model() override;

	static std::shared_ptr<Model> LoadFromFile(const std::string &path);
	void Draw() override;

	void AddMeshComponent(const MeshComponent &entry);
	const std::vector<MeshComponent> &GetMeshes() const { return m_meshes; }

private:
	std::vector<MeshComponent> m_meshes;
};