#include "Engine/Objects/SceneNode.h"

SceneNode::SceneNode(std::shared_ptr<GraphicsObject> obj)
	: m_parent(nullptr), m_obj(obj)
{
}

void SceneNode::AddChild(std::shared_ptr<SceneNode> child)
{
	if (child->m_parent) {
		child->m_parent->RemoveChild(child);
	}
	child->m_parent = this;
	m_children.push_back(child);
}

void SceneNode::RemoveChild(const std::shared_ptr<SceneNode> &child)
{
	auto it = std::find(m_children.begin(), m_children.end(), child);
	if (it != m_children.end()) {
		(*it)->m_parent = nullptr;
		m_children.erase(it);
	}
}

glm::mat4 SceneNode::GetWorldMatrix() const
{
	if (m_parent && m_obj) {
		return m_parent->GetWorldMatrix() * m_obj->GetModelMatrix();
	}
	return glm::mat4(1.0f);
}

void SceneNode::Draw()
{
	if (m_obj)
	{
		m_obj->SetWorldMatrix(GetWorldMatrix());
		m_obj->Draw();
	}

	for (const auto &child : m_children) {
		child->Draw();
	}
}
