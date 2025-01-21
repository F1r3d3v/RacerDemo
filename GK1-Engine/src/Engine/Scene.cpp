#include "Engine/Scene.h"
#include "Engine/Objects/Light/LightManager.h"

Scene::Scene() : m_root(std::make_shared<SceneNode>()) {}

void Scene::SetCamera(std::shared_ptr<Camera> camera) {
	m_camera = camera;
}

std::shared_ptr<Camera> Scene::GetCamera() const {
	return m_camera;
}

std::shared_ptr<SceneNode> Scene::GetRoot() const {
	return m_root;
}

void Scene::Draw(Renderer *renderer) {
	if (!m_camera) return;

	glm::mat4 ubo[2] = { m_camera->GetViewMatrix(), m_camera->GetProjectionMatrix() };
	glBindBuffer(GL_UNIFORM_BUFFER, renderer->GetMatricesUBO());
	glBufferSubData(GL_UNIFORM_BUFFER, 0, 2 * sizeof(glm::mat4), reinterpret_cast<float *>(&(*ubo)));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	m_lightManager.UpdateLights();

	m_root->Draw();
}

std::shared_ptr<SceneNode> Scene::AddObject(std::shared_ptr<GraphicsObject> obj, SceneNode *parent) {
	auto sceneObject = std::make_shared<SceneNode>(obj);
	if (parent) {
		parent->AddChild(sceneObject);
	}
	else {
		m_root->AddChild(sceneObject);
	}
	return sceneObject;
}

void Scene::AddLight(std::shared_ptr<Light> light, SceneNode *parent)
{
	m_lightManager.AddLight(light);
	AddObject(light, parent);
}
