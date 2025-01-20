#pragma once
#pragma once
#include "Engine/Objects/Mesh.h"
#include "Engine/Resource/Material.h"
#include "Engine/Resource/Texture.h"

class Terrain : public GraphicsObject {
public:
	struct TerrainConfig {
		uint32_t width{ 256 };
		uint32_t depth{ 256 };
		float scale{ 1.0f };
		float heightScale{ 50.0f };
		float uvScale{ 1.0f };
	};

	Terrain();
	explicit Terrain(const TerrainConfig &config);
	~Terrain() override;

	static std::shared_ptr<Terrain> CreateFromHeightmap(const std::shared_ptr<Texture> &heightmap);
	void SetHeightData(const std::vector<float> &heightData, uint32_t width, uint32_t depth);

	void SetMaterial(std::shared_ptr<Material> material) { m_mesh->SetMaterial(material); }
	std::shared_ptr<Material> GetMaterial() const { return m_mesh->GetMaterial(); }

	std::shared_ptr<Mesh> GetMesh() const { return m_mesh; }

	// Set/get scale, height scale, and UV scale then recreate the geometry
	void SetWorldScale(float scale);	
	float GetWorldScale() const { return m_config.scale; }
	void SetHeightScale(float heightScale);
	float GetHeightScale() const { return m_config.heightScale; }
	void SetUVScale(float uvScale);
	float GetUVScale() const { return m_config.uvScale; }


	float GetHeightAt(float x, float z) const;
	glm::vec3 GetNormalAt(float x, float z) const;

	void Draw() override;

private:
	void CreateGeometry();
	float SampleHeight(int x, int z) const;
	glm::vec3 CalculateNormal(int x, int z) const;

	TerrainConfig m_config;
	std::shared_ptr<Mesh> m_mesh;
	std::vector<float> m_heightData;
};