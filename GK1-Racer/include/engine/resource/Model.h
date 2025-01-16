#pragma once
#include "engine/resource/Resource.h"
#include "engine/resource/Mesh.h"
#include "engine/resource/Material.h"
#include <memory>
#include <vector>

class Model : public Resource {
public:

	struct MeshComponent {
		std::shared_ptr<Mesh> mesh;
		std::shared_ptr<Material> material;
	};

	Model();
	~Model() override;

	static std::shared_ptr<Model> LoadFromFile(const std::string &path);
	void Draw() const;

	void AddMeshComponent(const MeshComponent &entry);
	const std::vector<MeshComponent> &GetMeshes() const { return m_meshes; }

private:
	std::vector<MeshComponent> m_meshes;
};