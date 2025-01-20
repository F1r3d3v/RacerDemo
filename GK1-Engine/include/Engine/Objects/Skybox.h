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
	void SetCubemap(std::shared_ptr<Texture> cubemap) { m_cubemap = cubemap; }
	std::shared_ptr<Texture> GetCubemap() const { return m_cubemap; }

private:
	void SetupGeometry();
	void Cleanup();

	GLuint m_vao;
	GLuint m_vbo;
	std::shared_ptr<Texture> m_cubemap;
	std::shared_ptr<Shader> m_shader;
};