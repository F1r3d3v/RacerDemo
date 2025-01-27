#pragma once
#include "Engine/Objects/Light/PointLight.h"
#include <algorithm>

class SpotLight : public PointLight {
public:
	SpotLight(const Properties &props = Properties{},
			  const Attenuation &attn = Attenuation{}, float focus = 1.0f)
		: PointLight(props, attn), m_focusExponent(focus)
	{}

	void SetFocus(float exponent) { m_focusExponent = std::max(exponent, 1.0f); }
	float GetFocus() const { return m_focusExponent; }

protected:
	void FillLightData(LightData &data) const override
	{
		PointLight::FillLightData(data);
		data.direction = glm::vec4(GetWorldForward(), m_focusExponent);
	}
	float m_focusExponent;
};