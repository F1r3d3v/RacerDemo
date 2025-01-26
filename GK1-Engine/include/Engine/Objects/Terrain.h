#pragma once
#include "Engine/Objects/GraphicsObject.h"
#include "Engine/Resource/Texture.h"
#include "Engine/Resource/Shader.h"
#include "Engine/Resource/Material.h"
#include "Engine/Geometry.h"
#include <memory>

class Terrain : public GraphicsObject {
public:
	Terrain(uint32_t gridSize = 64);
	~Terrain() override;

	void Draw() override;

	void SetHeightmap(std::shared_ptr<Texture> heightmap) { m_heightmap = heightmap; m_dirty = true; }
	void SetTessellationLevel(float level) { m_tessLevel = level; }
	void SetHeightScale(float scale) { m_heightScale = scale; }
	void SetWorldScale(float scale) { m_WorldScale = scale; m_dirty = true; }
	void SetMaterial(std::shared_ptr<Material> material) { m_material = material; }
	std::shared_ptr<Material> GetMaterial() const { return m_material; }

	void GenerateCollisionMesh(uint32_t gridSize, std::vector<glm::vec3> &vertices, std::vector<uint32_t> &indices);

private:
	void SetupGeometry();
	void Cleanup();

	std::shared_ptr<Texture> m_heightmap;
	std::shared_ptr<Material> m_material;

	GLuint m_vao;
	GLuint m_vbo;
	GLuint m_ebo;

	uint32_t m_gridSize;
	float m_tessLevel;
	float m_heightScale;
	float m_WorldScale;
	bool m_dirty;
	std::vector<Geometry::Vertex> m_vertices;
	std::vector<uint32_t> m_indices;
};