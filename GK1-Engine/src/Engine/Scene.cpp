#include "Engine/Scene.h"
#include "Engine/Objects/Light/LightManager.h"

Scene::Scene() : m_root(std::make_shared<SceneNode>()) {}

void Scene::SetCamera(std::shared_ptr<Camera> camera) {
	m_camera = camera;
}

std::shared_ptr<Camera> Scene::GetCamera() const {
	return m_camera;
}

void Scene::SetSkybox(std::shared_ptr<Skybox> skybox)
{
	m_skybox = skybox;
}

std::shared_ptr<Skybox> Scene::GetSkybox() const
{
	return m_skybox;
}

std::shared_ptr<SceneNode> Scene::GetRoot() const {
	return m_root;
}

void Scene::Draw(Renderer *renderer) {
	if (!m_camera) return;

	// Update states
	UpdateMatricesUBO(renderer);
	m_lightManager.UpdateLights();
	UpdateFogUBO(renderer);

	// Draw scene

	m_root->Draw();
	if (m_skybox)
	{
		bool perspective = m_camera->GetProjectionType() == Camera::ProjectionType::Perspective;
		if (!perspective)
		{
			m_camera->SetPerspective(m_camera->GetFov(), m_camera->GetAspectRatio(), m_camera->GetNearPlane(), m_camera->GetFarPlane());
			UpdateMatricesUBO(renderer);

			m_skybox->Draw();

			m_camera->SetOrthographic(m_camera->GetOrthographicSize(), m_camera->GetNearPlane(), m_camera->GetFarPlane());
			UpdateMatricesUBO(renderer);
		}
		else
		{
			m_skybox->Draw();
		}
	}
}

void Scene::UpdateMatricesUBO(Renderer *renderer) const
{
	glm::mat4 ubo[2] = { m_camera->GetViewMatrix(), m_camera->GetProjectionMatrix() };
	glBindBuffer(GL_UNIFORM_BUFFER, renderer->GetMatricesUBO());
	glBufferSubData(GL_UNIFORM_BUFFER, 0, 2 * sizeof(glm::mat4), reinterpret_cast<float *>(&(*ubo)));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Scene::UpdateFogUBO(Renderer *renderer) const
{
	glBindBuffer(GL_UNIFORM_BUFFER, renderer->GetFogUBO());
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Fog), &m_fog);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
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

void Scene::RemoveLight(std::shared_ptr<Light> light)
{
	m_lightManager.RemoveLight(light);
	// TODO: Remove light from scene graph
	// m_root->RemoveChild(light);
}

void Scene::SetFog(const glm::vec3 &color, float density)
{
	m_fog.color = glm::vec4(color, density);
}

void Scene::GetFog(glm::vec3 &color, float &density) const
{
	color = glm::vec3(m_fog.color);
	density = m_fog.color.a;
}

void Scene::EnableFog(bool enable)
{
	m_fog.enabled = enable;
}

bool Scene::IsFogEnabled() const
{
	return m_fog.enabled;
}

LightManager &Scene::GetLightManager()
{
	return m_lightManager;
}
