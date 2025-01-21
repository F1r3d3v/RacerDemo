#pragma once
#include "Engine/Renderer.h"
#include "Engine/Objects/SceneNode.h"
#include "Engine/Objects/Camera.h"
#include "Engine/Objects/Light/LightManager.h"

#include <memory>
#include <glad/gl.h>

class Scene {
public:
	Scene();
	~Scene() = default;

	void SetCamera(std::shared_ptr<Camera> camera);
	std::shared_ptr<Camera> GetCamera() const;
	std::shared_ptr<SceneNode> GetRoot() const;
	std::shared_ptr<SceneNode> AddObject(std::shared_ptr<GraphicsObject> obj, SceneNode *parent = nullptr);
	void AddLight(std::shared_ptr<Light> light, SceneNode *parent = nullptr);

	void Draw(Renderer *renderer);

private:
	LightManager m_lightManager;
	std::shared_ptr<SceneNode> m_root;
	std::shared_ptr<Camera> m_camera;
};