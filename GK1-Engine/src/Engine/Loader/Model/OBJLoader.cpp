#include "Engine/Loader/Model/OBJLoader.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <filesystem>

#include <Engine/Loader/LoaderFactory.h>
#include <Engine/Loader/Material/MaterialLoader.h>

std::shared_ptr<Model> OBJLoader::Load(const std::string &filePath) {
	auto model = std::make_shared<Model>();

	auto objData = LoadOBJ(filePath);
	if (!objData.success) {
		std::cerr << "Failed to load model: " << objData.error << std::endl;
		return nullptr;
	}

	// Create mesh components for each mesh in the data
	for (const auto &meshData : objData.meshes) {
		Mesh mesh;
		mesh.geometry = std::make_shared<Geometry>(meshData.vertices, meshData.indices);
		mesh.material = meshData.material ? meshData.material : std::make_shared<Material>();
		mesh.SetName(meshData.name);

		model->AddMesh(mesh);
	}

	return model;
}

OBJLoader::OBJData OBJLoader::LoadOBJ(const std::string &path) {
	OBJData result;
	result.success = false;

	std::ifstream file(path);
	if (!file.is_open()) {
		result.error = "Failed to open file: " + path;
		return result;
	}

	std::vector<glm::vec3> temp_positions;
	std::vector<glm::vec2> temp_uvs;
	std::vector<glm::vec3> temp_normals;

	// Current group/object data
	std::string currentGroupName = "default";
	std::string currentMaterial;
	std::unordered_map<std::string, TempMeshData> meshGroups;
	meshGroups[currentGroupName] = TempMeshData();

	std::string line;
	while (std::getline(file, line)) {
		std::istringstream iss(line);
		std::string type;
		iss >> type;

		if (type == "v") {
			temp_positions.push_back(ParseVec3(line));
		}
		else if (type == "vt") {
			temp_uvs.push_back(ParseVec2(line));
		}
		else if (type == "vn") {
			temp_normals.push_back(ParseVec3(line));
		}
		else if (type == "f") {
			ParseFace(line,
				meshGroups[currentGroupName].vertexIndices,
				meshGroups[currentGroupName].uvIndices,
				meshGroups[currentGroupName].normalIndices);
		}
		else if (type == "g" || type == "o") {
			// Start new group/object
			std::string name;
			iss >> name;
			if (!name.empty()) {
				currentGroupName = name;
				if (meshGroups.find(currentGroupName) == meshGroups.end()) {
					meshGroups[currentGroupName] = TempMeshData();
					meshGroups[currentGroupName].material = currentMaterial;
				}
			}
		}
		else if (type == "mtllib") {
			std::string mtlFile;
			iss >> mtlFile;
			std::filesystem::path objPath(path);
			std::filesystem::path mtlPath = objPath.parent_path() / mtlFile;
			std::string ext = mtlPath.extension().string();

			auto mtlFactory = LoaderFactory<MaterialLoader>::CreateLoader(ext);
			auto mtlData = mtlFactory->Load(mtlPath.string());
			if (!mtlData.empty()) {
				m_materials.insert(m_materials.end(), mtlData.begin(), mtlData.end());
			}
			else {
				std::cerr << "Failed to load materials from MTL file: " << mtlPath << std::endl;
			}
		}
		else if (type == "usemtl") {
			iss >> currentMaterial;

			// If material changes within a group, create a new group with unique name
			if (!meshGroups[currentGroupName].vertexIndices.empty() &&
				meshGroups[currentGroupName].material != currentMaterial) {
				std::string newGroupName = currentGroupName + "_" + currentMaterial;
				currentGroupName = newGroupName;
				meshGroups[currentGroupName] = TempMeshData();
			}
			meshGroups[currentGroupName].material = currentMaterial;
		}
	}

	// Process all mesh groups
	for (const auto &[groupName, tempMesh] : meshGroups) {
		if (tempMesh.vertexIndices.empty()) continue;

		MeshData meshData = ProcessMeshData(temp_positions, temp_uvs, temp_normals, tempMesh, groupName);

		// Assign material
		if (!tempMesh.material.empty()) {
			auto it = std::find_if(m_materials.begin(), m_materials.end(), 
				[&tempMesh](std::shared_ptr<Material> material)
				{
					return material->GetName() == tempMesh.material;
				});
			if (it != m_materials.end()) {
				meshData.material = *it;
			}
		}

		result.meshes.push_back(std::move(meshData));
	}

	result.success = true;
	return result;
}

OBJLoader::MeshData OBJLoader::ProcessMeshData(
	const std::vector<glm::vec3> &positions,
	const std::vector<glm::vec2> &uvs,
	const std::vector<glm::vec3> &normals,
	const TempMeshData &tempMesh,
	const std::string &name) {

	MeshData meshData;
	meshData.name = name;

	// Map to store unique vertices
	std::unordered_map<std::string, uint32_t> uniqueVertices;

	std::unordered_map<uint32_t, uint32_t> vertexCount;

	// Process all face vertices and build indices
	for (size_t i = 0; i < tempMesh.vertexIndices.size(); i += 3)
	{
		uint32_t indices[3];

		// Process each vertex in the triangle
		for (int j = 0; j < 3; ++j)
		{
			Geometry::Vertex vertex;

			// Get the indices
			uint32_t posIndex = tempMesh.vertexIndices[i + j];
			uint32_t uvIndex = tempMesh.uvIndices[i + j];

			// Get the position and texCoords
			vertex.position = positions[posIndex];
			vertex.texCoords = (uvIndex < uvs.size()) ? uvs[uvIndex] : glm::vec2(0.0f);
			vertex.normal = glm::vec3(0.0f);

			// Create a unique key for the vertex
			std::string key = std::to_string(posIndex);

			// Check if the vertex already exists
			if (uniqueVertices.count(key) == 0)
			{
				uniqueVertices[key] = static_cast<uint32_t>(meshData.vertices.size());
				meshData.vertices.push_back(vertex);
			}

			indices[j] = uniqueVertices[key];
			meshData.indices.push_back(indices[j]);
		}

		if (tempMesh.normalIndices.empty())
		{
			// Calculate the face normal
			Geometry::Vertex &v0 = meshData.vertices[indices[0]];
			Geometry::Vertex &v1 = meshData.vertices[indices[1]];
			Geometry::Vertex &v2 = meshData.vertices[indices[2]];
			glm::vec3 delta1 = v1.position - v0.position;
			glm::vec3 delta2 = v2.position - v0.position;
			glm::vec3 faceNormal = glm::normalize(glm::cross(delta1, delta2));

			// Accumulate normals for each vertex
			v0.normal += faceNormal;
			vertexCount[indices[0]]++;
			v1.normal += faceNormal;
			vertexCount[indices[1]]++;
			v2.normal += faceNormal;
			vertexCount[indices[2]]++;
		}
		else
		{
			// Get the normal indices
			uint32_t normalIndex = tempMesh.normalIndices[i];
			Geometry::Vertex &v0 = meshData.vertices[indices[0]];
			Geometry::Vertex &v1 = meshData.vertices[indices[1]];
			Geometry::Vertex &v2 = meshData.vertices[indices[2]];

			// Accumulate normals for each vertex
			v0.normal += normals[normalIndex];
			vertexCount[indices[0]]++;
			v1.normal += normals[normalIndex];
			vertexCount[indices[1]]++;
			v2.normal += normals[normalIndex];
			vertexCount[indices[2]]++;
		}
	}

	// Normalize the accumulated normals
	for (int i = 0; i < meshData.vertices.size(); ++i)
	{
		meshData.vertices[i].normal = glm::normalize(meshData.vertices[i].normal / static_cast<float>(vertexCount[i]));
	}

	// Calculate tangents and bitangents
	CalculateTangents(meshData.vertices, meshData.indices);

	return meshData;
}

void OBJLoader::CalculateTangents(std::vector<Geometry::Vertex> &vertices, const std::vector<uint32_t> &indices)
{
	for (auto &vertex : vertices)
	{
		vertex.tangent = glm::vec3(0.0f);
		vertex.bitangent = glm::vec3(0.0f);
	}

	std::unordered_map<uint32_t, uint32_t> tangentCount;
	std::unordered_map<uint32_t, uint32_t> bitangentCount;

	for (size_t i = 0; i < indices.size(); i += 3)
	{
		// Get the vertices
		Geometry::Vertex &v0 = vertices[indices[i]];
		Geometry::Vertex &v1 = vertices[indices[i + 1]];
		Geometry::Vertex &v2 = vertices[indices[i + 2]];

		// Calculate the edges
		glm::vec3 edge1 = v1.position - v0.position;
		glm::vec3 edge2 = v2.position - v0.position;

		// Calculate the UV deltas
		glm::vec2 deltaUV1 = v1.texCoords - v0.texCoords;
		glm::vec2 deltaUV2 = v2.texCoords - v0.texCoords;

		// Calculate the tangent and bitangent
		float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
		if (std::isinf(f)) f = 1.0f;
		glm::vec3 tangent(f * (deltaUV2.y * edge1 - deltaUV1.y * edge2));
		glm::vec3 bitangent(f * (deltaUV1.x * edge2 - deltaUV2.x * edge1));

		// Accumulate the tangent and bitangent
		for (int j = 0; j < 3; ++j)
		{
			vertices[indices[i + j]].tangent += tangent;
			tangentCount[indices[i + j]]++;

			vertices[indices[i + j]].bitangent += bitangent;
			bitangentCount[indices[i + j]]++;
		}
	}

	// Normalize the tangents and bitangents
	for (int i = 0; i < vertices.size(); ++i)
	{
		vertices[i].tangent = glm::normalize(vertices[i].tangent / static_cast<float>(tangentCount[i]));
		vertices[i].bitangent = glm::normalize(vertices[i].bitangent / static_cast<float>(bitangentCount[i]));
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