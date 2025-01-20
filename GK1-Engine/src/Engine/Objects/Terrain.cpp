#include "Engine/Objects/Terrain.h"
#include <algorithm>

Terrain::Terrain()
	: m_config{}
{
}

Terrain::Terrain(const TerrainConfig &config)
	: m_config(config)
{
	CreateGeometry();
}

Terrain::~Terrain() = default;

std::shared_ptr<Terrain> Terrain::CreateFromHeightmap(const std::shared_ptr<Texture> &heightmap) {
	if (!heightmap || heightmap->GetType() != Texture::TextureType::Height) {
		return nullptr;
	}

	auto terrain = std::make_shared<Terrain>();

	int width = heightmap->GetWidth();
	int height = heightmap->GetHeight();
	uint8_t *data = heightmap->GetData();

	std::vector<float> heightData(width * height);
	for (int i = 0; i < width * height; i++) {
		heightData[i] = static_cast<float>(data[i]) / 255.0f;
	}

	terrain->SetHeightData(heightData, width, height);
	return terrain;
}

void Terrain::SetHeightData(const std::vector<float> &heightData, uint32_t width, uint32_t depth) {
	m_heightData = heightData;
	m_config.width = width;
	m_config.depth = depth;
	CreateGeometry();
}

void Terrain::CreateGeometry() {
	std::vector<Geometry::Vertex> vertices;
	std::vector<uint32_t> indices;

	vertices.reserve(m_config.width * m_config.depth);
	indices.reserve((m_config.width - 1) * (m_config.depth - 1) * 6);

	// Generate vertices
	for (uint32_t z = 0; z < m_config.depth; ++z) {
		for (uint32_t x = 0; x < m_config.width; ++x) {
			Geometry::Vertex vertex;

			// Calculate actual world position
			float xPos = (static_cast<float>(x) - m_config.width * 0.5f) * m_config.scale;
			float zPos = (static_cast<float>(z) - m_config.depth * 0.5f) * m_config.scale;

			// Real displacement using height data
			float height = SampleHeight(x, z);
			float yPos = height * m_config.heightScale;

			vertex.position = glm::vec3(xPos, yPos, zPos);

			// Calculate normal using central differences
			vertex.normal = CalculateNormal(x, z);

			// Calculate texture coordinates
			vertex.texCoords = glm::vec2(
				static_cast<float>(x) / (m_config.width - 1) * m_config.uvScale,
				static_cast<float>(z) / (m_config.depth - 1) * m_config.uvScale
			);

			// Calculate tangent space basis vectors
			glm::vec3 tangent(1.0f, 0.0f, 0.0f);
			vertex.tangent = glm::normalize(tangent - vertex.normal * glm::dot(tangent, vertex.normal));
			vertex.bitangent = glm::cross(vertex.normal, vertex.tangent);

			vertices.push_back(vertex);
		}
	}

	// Generate indices for triangles
	for (uint32_t z = 0; z < m_config.depth - 1; ++z) {
		for (uint32_t x = 0; x < m_config.width - 1; ++x) {
			uint32_t topLeft = z * m_config.width + x;
			uint32_t topRight = topLeft + 1;
			uint32_t bottomLeft = (z + 1) * m_config.width + x;
			uint32_t bottomRight = bottomLeft + 1;

			// First triangle
			indices.push_back(topLeft);
			indices.push_back(bottomLeft);
			indices.push_back(topRight);

			// Second triangle
			indices.push_back(topRight);
			indices.push_back(bottomLeft);
			indices.push_back(bottomRight);
		}
	}

	// Create or update the mesh
	if (!m_mesh) {
		m_mesh = std::make_shared<Mesh>();
	}

	auto geometry = std::make_shared<Geometry>(vertices, indices);
	m_mesh->SetGeometry(geometry);
}

float Terrain::SampleHeight(int x, int z) const {
	x = std::clamp(x, 0, static_cast<int>(m_config.width) - 1);
	z = std::clamp(z, 0, static_cast<int>(m_config.depth) - 1);
	return m_heightData[z * m_config.width + x];
}

glm::vec3 Terrain::CalculateNormal(int x, int z) const {
	float hL = SampleHeight(x - 1, z);
	float hR = SampleHeight(x + 1, z);
	float hD = SampleHeight(x, z - 1);
	float hU = SampleHeight(x, z + 1);

	// Calculate the normal using the height differences
	glm::vec3 normal(
		(hL - hR) * m_config.heightScale,
		2.0f * m_config.scale,
		(hD - hU) * m_config.heightScale
	);

	return glm::normalize(normal);
}

void Terrain::SetWorldScale(float scale)
{
	m_config.scale = scale;
	CreateGeometry();
}

void Terrain::SetHeightScale(float heightScale)
{
	m_config.heightScale = heightScale;
	CreateGeometry();
}

void Terrain::SetUVScale(float uvScale)
{
	m_config.uvScale = uvScale;
	CreateGeometry();
}

float Terrain::GetHeightAt(float x, float z) const {
	float nx = (x / m_config.scale + m_config.width * 0.5f);
	float nz = (z / m_config.scale + m_config.depth * 0.5f);

	int ix = static_cast<int>(nx);
	int iz = static_cast<int>(nz);

	float fx = nx - ix;
	float fz = nz - iz;

	float h00 = SampleHeight(ix, iz);
	float h10 = SampleHeight(ix + 1, iz);
	float h01 = SampleHeight(ix, iz + 1);
	float h11 = SampleHeight(ix + 1, iz + 1);

	float h0 = h00 * (1.0f - fx) + h10 * fx;
	float h1 = h01 * (1.0f - fx) + h11 * fx;

	return (h0 * (1.0f - fz) + h1 * fz) * m_config.heightScale;
}

glm::vec3 Terrain::GetNormalAt(float x, float z) const
{
	float nx = (x / m_config.scale + m_config.width * 0.5f);
	float nz = (z / m_config.scale + m_config.depth * 0.5f);

	int ix = static_cast<int>(nx);
	int iz = static_cast<int>(nz);

	float fx = nx - ix;
	float fz = nz - iz;

	glm::vec3 n00 = CalculateNormal(ix, iz);
	glm::vec3 n10 = CalculateNormal(ix + 1, iz);
	glm::vec3 n01 = CalculateNormal(ix, iz + 1);
	glm::vec3 n11 = CalculateNormal(ix + 1, iz + 1);
	glm::vec3 n0 = n00 * (1.0f - fx) + n10 * fx;
	glm::vec3 n1 = n01 * (1.0f - fx) + n11 * fx;
	return glm::normalize(n0 * (1.0f - fz) + n1 * fz);
}

void Terrain::Draw() {
	if (m_mesh) {
		auto shader = m_mesh->GetMaterial()->GetShader();
		if (shader) {
			shader->Use();
			shader->SetMat4("model", GetWorldMatrix());
		}
		m_mesh->Draw();
	}
}