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
