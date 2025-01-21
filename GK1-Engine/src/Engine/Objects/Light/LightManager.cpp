#include "Engine/Objects/Light/LightManager.h"
#include <glad/gl.h>


LightManager::LightManager()
{
	glGenBuffers(1, &m_ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, m_ubo);
	GLsizei bufSize = (MAX_POINT_LIGHTS + MAX_SPOT_LIGHTS) * sizeof(Light::LightData) + sizeof(glm::ivec4);
	glBufferData(GL_UNIFORM_BUFFER, bufSize, NULL, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, m_ubo);
}

LightManager::~LightManager()
{
	glDeleteBuffers(1, &m_ubo);
}

void LightManager::AddLight(std::shared_ptr<Light> light) {
	if (auto pointLight = std::dynamic_pointer_cast<PointLight>(light)) {
		if (m_pointLights.size() < MAX_POINT_LIGHTS) {
			m_pointLights.push_back(pointLight);
		}
	}
	else if (auto spotLight = std::dynamic_pointer_cast<SpotLight>(light)) {
		if (m_spotLights.size() < MAX_SPOT_LIGHTS) {
			m_spotLights.push_back(spotLight);
		}
	}
}

void LightManager::RemoveLight(std::shared_ptr<Light> light)
{
	if (auto pointLight = std::dynamic_pointer_cast<PointLight>(light))
	{
		auto it = std::find(m_pointLights.begin(), m_pointLights.end(), pointLight);
		if (it != m_pointLights.end())
		{
			m_pointLights.erase(it);
		}
	}
	else if (auto spotLight = std::dynamic_pointer_cast<SpotLight>(light))
	{
		auto it = std::find(m_spotLights.begin(), m_spotLights.end(), spotLight);
		if (it != m_spotLights.end())
		{
			m_spotLights.erase(it);
		}
	}
}

void LightManager::UpdateLights() {
	struct LightBuffer {
		Light::LightData pointLights[MAX_POINT_LIGHTS];
		Light::LightData spotLights[MAX_SPOT_LIGHTS];
		glm::ivec4 counts;
	} lightBuffer;

	for (size_t i = 0; i < m_pointLights.size(); i++) {
		GetLightData(m_pointLights[i], lightBuffer.pointLights[i]);
	}

	for (size_t i = 0; i < m_spotLights.size(); i++) {
		GetLightData(m_spotLights[i], lightBuffer.spotLights[i]);
	}

	lightBuffer.counts = glm::ivec4(
		static_cast<int>(m_pointLights.size()),
		static_cast<int>(m_spotLights.size()),
		0, 0
	);

	glBindBuffer(GL_UNIFORM_BUFFER, m_ubo);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(LightBuffer), &lightBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glBindBufferBase(GL_UNIFORM_BUFFER, 1, m_ubo);
}

void LightManager::GetLightData(const std::shared_ptr<Light> &light, Light::LightData &data) const
{
	light->FillLightData(data);
}
