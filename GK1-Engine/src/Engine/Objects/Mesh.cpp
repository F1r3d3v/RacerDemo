#include "Engine/Objects/Mesh.h"

Mesh::Mesh() : geometry(nullptr), material(nullptr)
{
}

Mesh::Mesh(std::shared_ptr<Geometry> geometry, std::shared_ptr<Material> material)
	: geometry(geometry), material(material)
{
}

Mesh::~Mesh()
{
}

void Mesh::Draw()
{
	if (!geometry || !material) return;

	material->Bind();
	glBindVertexArray(geometry->GetVAO());
	glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(geometry->GetIndexCount()), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	material->Unbind();
}

glm::vec3 Mesh::GetMinBounds() const
{
    if (!geometry) return glm::vec3(0.0f);

    const auto& vertices = geometry->GetVertices();
    glm::vec3 minBounds = vertices[0].position;

    for (const auto& vertex : vertices)
    {
        minBounds = glm::min(minBounds, vertex.position);
    }

    return minBounds;
}

glm::vec3 Mesh::GetMaxBounds() const
{
    if (!geometry) return glm::vec3(0.0f);

    const auto& vertices = geometry->GetVertices();
    glm::vec3 maxBounds = vertices[0].position;

    for (const auto& vertex : vertices)
    {
        maxBounds = glm::max(maxBounds, vertex.position);
    }

    return maxBounds;
}