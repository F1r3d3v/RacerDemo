#pragma once
#include "engine/resource/Mesh.h"
#include "engine/loader/model/ModelLoader.h"
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

class OBJLoader : public ModelLoader {
public:
	struct MeshData {
		std::string name;
		std::vector<Mesh::Vertex> vertices;
		std::vector<uint32_t> indices;
		std::shared_ptr<Material> material;
	};

	struct OBJData {
		std::vector<MeshData> meshes;
		bool success;
		std::string error;
	};

	std::shared_ptr<Model> Load(const std::string &filePath) override;

private:
	struct TempMeshData {
		std::vector<uint32_t> vertexIndices;
		std::vector<uint32_t> uvIndices;
		std::vector<uint32_t> normalIndices;
		std::string material;
	};

	std::vector<std::shared_ptr<Material>> m_materials;

	OBJData LoadOBJ(const std::string &path);
	void CalculateTangents(std::vector<Mesh::Vertex> &vertices, const std::vector<uint32_t> &indices);
	glm::vec3 ParseVec3(const std::string &line);
	glm::vec2 ParseVec2(const std::string &line);
	void ParseFace(const std::string &line,
		std::vector<uint32_t> &vertexIndices,
		std::vector<uint32_t> &uvIndices,
		std::vector<uint32_t> &normalIndices);
	MeshData ProcessMeshData(const std::vector<glm::vec3> &positions,
		const std::vector<glm::vec2> &uvs,
		const std::vector<glm::vec3> &normals,
		const TempMeshData &tempMesh,
		const std::string &name);
};