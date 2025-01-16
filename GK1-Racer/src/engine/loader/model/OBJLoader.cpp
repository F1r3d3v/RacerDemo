#include "engine/loader/model/OBJLoader.h"
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <iostream>
#include <algorithm>

std::shared_ptr<Model> OBJLoader::Load(const std::string &filePath)
{
	auto model = std::make_shared<Model>();

	auto objData = LoadOBJ(filePath);
	if (!objData.success)
	{
		std::cerr << "Failed to load model: " << objData.error << std::endl;
		return nullptr;
	}

	// Create mesh component
	Model::MeshComponent component;
	component.mesh = std::make_shared<Mesh>(objData.vertices, objData.indices);
	component.material = std::make_shared<Material>();

	// Add the mesh component to the model
	model->AddMeshComponent(component);

	return model;
}

OBJLoader::OBJData OBJLoader::LoadOBJ(const std::string &path)
{
	OBJData result;
	result.success = false;

	std::ifstream file(path);
	if (!file.is_open())
	{
		result.error = "Failed to open file: " + path;
		return result;
	}

	std::vector<glm::vec3> temp_positions;
	std::vector<glm::vec2> temp_uvs;
	std::vector<glm::vec3> temp_normals;
	std::vector<uint32_t> vertexIndices, uvIndices, normalIndices;

	std::string line;
	while (std::getline(file, line))
	{
		std::istringstream iss(line);
		std::string type;
		iss >> type;

		if (type == "v")
		{
			temp_positions.push_back(ParseVec3(line));
		}
		else if (type == "vt")
		{
			temp_uvs.push_back(ParseVec2(line));
		}
		else if (type == "vn")
		{
			temp_normals.push_back(ParseVec3(line));
		}
		else if (type == "f")
		{
			ParseFace(line, vertexIndices, uvIndices, normalIndices);
		}
		else if (type != "#")
		{
			result.error = "Invalid parameter: " + type;
			result.success = false;

			return result;
		}
	}

	// Create a map to store unique vertices
	std::unordered_map<std::string, uint32_t> uniqueVertices;

	// Process all face vertices
	for (size_t i = 0; i < vertexIndices.size(); i++)
	{
		Mesh::Vertex vertex;

		// Get the indices
		uint32_t posIndex = vertexIndices[i];
		uint32_t uvIndex = uvIndices[i];
		uint32_t normalIndex = normalIndices[i];

		// Get the actual data
		vertex.position = temp_positions[posIndex];
		vertex.texCoords = uvIndex < temp_uvs.size() ? temp_uvs[uvIndex] : glm::vec2(0.0f);
		vertex.normal = normalIndex < temp_normals.size() ? temp_normals[normalIndex] : glm::vec3(0.0f, 1.0f, 0.0f);

		// Create a string key for the vertex
		std::string key = std::to_string(posIndex) + "|" +
			std::to_string(uvIndex) + "|" +
			std::to_string(normalIndex);

		// Check if we've seen this vertex before
		if (uniqueVertices.count(key) == 0)
		{
			// If not, add it to our vertices
			uniqueVertices[key] = static_cast<uint32_t>(result.vertices.size());
			result.vertices.push_back(vertex);
		}

		// Add the index
		result.indices.push_back(uniqueVertices[key]);
	}

	// Calculate tangents and bitangents
	CalculateTangents(result.vertices, result.indices);

	result.success = true;
	return result;
}

void OBJLoader::CalculateTangents(std::vector<Mesh::Vertex> &vertices, const std::vector<uint32_t> &indices)
{
	for (size_t i = 0; i < indices.size(); i += 3)
	{
		Mesh::Vertex &v0 = vertices[indices[i]];
		Mesh::Vertex &v1 = vertices[indices[i + 1]];
		Mesh::Vertex &v2 = vertices[indices[i + 2]];

		glm::vec3 edge1 = v1.position - v0.position;
		glm::vec3 edge2 = v2.position - v0.position;
		glm::vec2 deltaUV1 = v1.texCoords - v0.texCoords;
		glm::vec2 deltaUV2 = v2.texCoords - v0.texCoords;

		float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

		glm::vec3 tangent;
		tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
		tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
		tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
		tangent = glm::normalize(tangent);

		glm::vec3 bitangent;
		bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
		bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
		bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
		bitangent = glm::normalize(bitangent);

		v0.tangent = v1.tangent = v2.tangent = tangent;
		v0.bitangent = v1.bitangent = v2.bitangent = bitangent;
	}
}

glm::vec3 OBJLoader::ParseVec3(const std::string &line)
{
	glm::vec3 result;
	std::istringstream iss(line);
	std::string type;
	iss >> type >> result.x >> result.y >> result.z;
	return result;
}

glm::vec2 OBJLoader::ParseVec2(const std::string &line)
{
	glm::vec2 result;
	std::istringstream iss(line);
	std::string type;
	iss >> type >> result.x >> result.y;
	return result;
}

void OBJLoader::ParseFace(const std::string &line,
						  std::vector<uint32_t> &vertexIndices,
						  std::vector<uint32_t> &uvIndices,
						  std::vector<uint32_t> &normalIndices)
{
	std::istringstream iss(line);
	std::string type;
	iss >> type;

	std::string vertex;
	while (iss >> vertex)
	{
		std::replace(vertex.begin(), vertex.end(), '/', ' ');
		std::istringstream vertexStream(vertex);
		uint32_t v, vt, vn;
		vertexStream >> v >> vt >> vn;

		// OBJ indices are 1-based, convert to 0-based
		vertexIndices.push_back(v - 1);
		uvIndices.push_back(vt - 1);
		normalIndices.push_back(vn - 1);
	}
}