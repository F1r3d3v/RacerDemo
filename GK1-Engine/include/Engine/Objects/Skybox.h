#pragma once
#include "Engine/Resource/Texture.h"
#include "Engine/Resource/Shader.h"
#include "Engine/Objects/GraphicsObject.h"

#include <memory>

class Skybox : public GraphicsObject {
public:
	Skybox();
	~Skybox();

	static std::shared_ptr<Skybox> LoadFromCubemap(std::shared_ptr<Texture> cubemap);
	static std::shared_ptr<Skybox> LoadFromFiles(const std::vector<std::string> &faces);
	void Draw() override;

	void SetShader(std::shared_ptr<Shader> shader) { m_shader = shader; }
	void SetCubemap(std::shared_ptr<Texture> cubemap) { m_dayCubemap = cubemap; }
	void SetNightCubemap(std::shared_ptr<Texture> cubemap) { m_nightCubemap = cubemap; }
	std::shared_ptr<Texture> GetCubemap() const { return m_dayCubemap; }
	std::shared_ptr<Texture> GetNightCubemap() const { return m_nightCubemap; }

	void SetBlendFactor(float factor) { m_blendFactor = factor; }
	float GetBlendFactor() const { return m_blendFactor; }

private:
	void SetupGeometry();
	void Cleanup();

	GLuint m_vao;
	GLuint m_vbo;
	std::shared_ptr<Texture> m_dayCubemap;
	std::shared_ptr<Texture> m_nightCubemap;
	float m_blendFactor = 1.0f;
	std::shared_ptr<Shader> m_shader;
};