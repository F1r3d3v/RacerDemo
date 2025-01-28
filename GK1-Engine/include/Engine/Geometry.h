#pragma once
#include "Engine/Resource/Resource.h"
#include "Engine/Objects/GraphicsObject.h"
#include <glm/glm.hpp>
#include <glad/gl.h>
#include <vector>

class Geometry  {
public:

	struct Vertex {
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 texCoords;
		glm::vec3 tangent;
		glm::vec3 bitangent;
	};

	Geometry();
	explicit Geometry(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices);
	~Geometry();

	void SetData(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices);

	GLuint GetVAO() const { return m_vao; }
	GLuint GetVBO() const { return m_vbo; }
	GLuint GetEBO() const { return m_ebo; }
	size_t GetVertexCount() const { return m_vertexCount; }
	size_t GetIndexCount() const { return m_indexCount; }
	const std::vector<Vertex> &GetVertices() const { return m_vertices; }
	const std::vector<uint32_t> &GetIndices() const { return m_indices; }

protected:
	virtual void Cleanup();
	virtual void SetupGeometry();

	std::vector<Vertex> m_vertices;
	std::vector<uint32_t> m_indices;
	GLuint m_vao;
	GLuint m_vbo;
	GLuint m_ebo;
	size_t m_vertexCount;
	size_t m_indexCount;
};