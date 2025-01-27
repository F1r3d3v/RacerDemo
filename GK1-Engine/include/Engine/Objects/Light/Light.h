#pragma once
#include "Engine/Objects/GraphicsObject.h"
#include <glm/glm.hpp>
#include <memory>

class Light : public GraphicsObject {
public:
	struct Properties {
		glm::vec3 color{ 1.0f };
		float intensity{ 1.0f };
		float radius{ 50.0f };
	};

	Light(const Properties &props = Properties{}) : m_properties(props) {}
	virtual ~Light() = default;

	void SetProperties(const Properties &props) { m_properties = props; }
	const Properties &GetProperties() const { return m_properties; }

	void SetColor(const glm::vec3 &color) { m_properties.color = color; }
	const glm::vec3 &GetColor() const { return m_properties.color; }

	void SetIntensity(float intensity) { m_properties.intensity = intensity; }
	float GetIntensity() const { return m_properties.intensity; }

	void SetRadius(float radius) { m_properties.radius = radius; }
	float GetRadius() const { return m_properties.radius; }

	friend class LightManager;

protected:
	struct alignas(16) LightData {
		glm::vec4 position;
		glm::vec4 direction;
		glm::vec4 color;          // RGB + intensity in w
		glm::vec4 attenuation;    // x=constant, y=linear, z=quadratic, w=radius
	};

	virtual void FillLightData(LightData &data) const = 0;
	Properties m_properties;
};