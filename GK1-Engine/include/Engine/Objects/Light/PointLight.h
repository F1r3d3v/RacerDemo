#pragma once
#include "Engine/Objects/Light/Light.h"

class PointLight : public Light {
public:
	struct Attenuation {
		float constant{ 1.0f };
		float linear{ 0.09f };
		float quadratic{ 0.032f };
	};

	PointLight(const Properties &props = Properties{}, const Attenuation &attn = Attenuation{}) : Light(props) { m_attenuation = attn; }

	void SetAttenuation(const Attenuation &atten) { m_attenuation = atten; }
	const Attenuation &GetAttenuation() const { return m_attenuation; }

protected:
	void FillLightData(LightData &data) const override
	{
		data.position = glm::vec4(GetWorldPosition(), 1.0f);
		data.direction = glm::vec4(0.0f);
		data.color = glm::vec4(m_properties.color, m_properties.intensity);
		data.attenuation = glm::vec4(m_attenuation.constant, m_attenuation.linear, m_attenuation.quadratic, m_properties.radius);
	}
	Attenuation m_attenuation;
};
