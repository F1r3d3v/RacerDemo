#pragma once
#include "Engine/Objects/Light/PointLight.h"
#include "Engine/Objects/Light/SpotLight.h"
#include "Engine/Renderer.h"
#include <vector>

class LightManager {
public:
	LightManager();
	~LightManager();

	static const uint32_t MAX_POINT_LIGHTS = 4;
	static const uint32_t MAX_SPOT_LIGHTS = 4;

	void AddLight(std::shared_ptr<Light> light);
	void RemoveLight(std::shared_ptr<Light> light);
	void UpdateLights();
	GLuint GetLightsUBO() const { return m_ubo; }

private:
	void GetLightData(const std::shared_ptr<Light> &light, Light::LightData &data) const;

	struct LightBuffer {
		Light::LightData pointLights[MAX_POINT_LIGHTS];
		Light::LightData spotLights[MAX_SPOT_LIGHTS];
		glm::ivec4 counts;
	};

	GLuint m_ubo;
	std::vector<std::shared_ptr<PointLight>> m_pointLights;
	std::vector<std::shared_ptr<SpotLight>> m_spotLights;
};
