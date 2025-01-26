#pragma once
#include "Engine/Renderer.h"
#include "Engine/Objects/SceneNode.h"
#include "Engine/Objects/Camera.h"
#include "Engine/Objects/Light/LightManager.h"
#include "Engine/Objects/Skybox.h"

#include <memory>
#include <glad/gl.h>

class Scene {
public:
	Scene();
	~Scene() = default;

	void SetCamera(std::shared_ptr<Camera> camera);
	std::shared_ptr<Camera> GetCamera() const;
	void SetSkybox(std::shared_ptr<Skybox> skybox);
	std::shared_ptr<Skybox> GetSkybox() const;
	std::shared_ptr<SceneNode> GetRoot() const;
	std::shared_ptr<SceneNode> AddObject(std::shared_ptr<GraphicsObject> obj, SceneNode *parent = nullptr);
	void AddLight(std::shared_ptr<Light> light, SceneNode *parent = nullptr);
	void RemoveLight(std::shared_ptr<Light> light);
	void SetFog(const glm::vec3 &color, float density);
	void GetFog(glm::vec3 &color, float &density) const;
	void EnableFog(bool enable);
	bool IsFogEnabled() const;
	LightManager *GetLightManager();

	void Draw(Renderer *renderer);

private:
	void UpdateMatricesUBO(Renderer *renderer) const;
	void UpdateFogUBO(Renderer *renderer) const;

	std::unique_ptr<LightManager> m_lightManager;

	struct Fog {
		glm::vec4 color{0.6f, 0.6f, 0.6f, 0.0f}; // density in w
		int enabled{false};
	} m_fog{};

	std::shared_ptr<SceneNode> m_root;
	std::shared_ptr<Camera> m_camera;
	std::shared_ptr<Skybox> m_skybox;
};