#pragma once
#include "engine/resource/Mesh.h"
#include "engine/loader/model/ModelLoader.h"
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <memory>

class OBJLoader : public virtual ModelLoader {
public:

	struct OBJData {
		std::vector<Mesh::Vertex> vertices;
		std::vector<uint32_t> indices;
		bool success;
		std::string error;
	};

	std::shared_ptr<Model> Load(const std::string &filePath) override;

private:
	OBJData LoadOBJ(const std::string &path);
	void CalculateTangents(std::vector<Mesh::Vertex> &vertices, const std::vector<uint32_t> &indices);
	glm::vec3 ParseVec3(const std::string &line);
	glm::vec2 ParseVec2(const std::string &line);
	void ParseFace(const std::string &line,
				   std::vector<uint32_t> &vertexIndices,
				   std::vector<uint32_t> &uvIndices,
				   std::vector<uint32_t> &normalIndices);
};