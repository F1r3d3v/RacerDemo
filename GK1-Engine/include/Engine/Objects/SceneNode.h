#pragma once
#include "Engine/Objects/GraphicsObject.h"
#include "Engine/Objects/Camera.h"
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

class SceneNode {
public:
	SceneNode() = default;
	SceneNode(std::shared_ptr<GraphicsObject> obj);
	virtual ~SceneNode() = default;

	void AddChild(std::shared_ptr<SceneNode> child);

	void RemoveChild(const std::shared_ptr<SceneNode> &child);

	std::shared_ptr<GraphicsObject> GetObjects() const { return m_obj; }
	SceneNode *GetParent() const { return m_parent; }
	const std::vector<std::shared_ptr<SceneNode>> &GetChildren() const { return m_children; }

	glm::mat4 GetWorldMatrix() const;

	void Draw();

protected:
	SceneNode *m_parent;
	std::shared_ptr<GraphicsObject> m_obj;
	std::vector<std::shared_ptr<SceneNode>> m_children;
};