#pragma once
#include "engine/resource/Resource.h"
#include <glm/glm.hpp>
#include <glad/gl.h>
#include <vector>

class Mesh : public Resource {
public:

	struct Vertex {
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 texCoords;
		glm::vec3 tangent;
		glm::vec3 bitangent;
	};

	Mesh();
	explicit Mesh(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices);
	~Mesh();

	void SetData(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices);
	void Draw() const;

	GLuint GetVAO() const { return m_vao; }
	GLuint GetVBO() const { return m_vbo; }
	GLuint GetEBO() const { return m_ebo; }
	size_t GetVertexCount() const { return m_vertexCount; }
	size_t GetIndexCount() const { return m_indexCount; }

private:
	void Cleanup();
	void SetupMesh();

	std::vector<Vertex> m_vertices;
	std::vector<uint32_t> m_indices;
	GLuint m_vao;
	GLuint m_vbo;
	GLuint m_ebo;
	size_t m_vertexCount;
	size_t m_indexCount;
};