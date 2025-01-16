#pragma once
#include "engine/resource/Resource.h"
#include "engine/resource/Texture.h"
#include "engine/resource/Shader.h"
#include <memory>

class Skybox : public Resource {
public:
	Skybox();
	~Skybox();

	static std::shared_ptr<Skybox> LoadFromCubemap(std::shared_ptr<Texture> cubemap);
	static std::shared_ptr<Skybox> LoadFromFiles(const std::vector<std::string> &faces);
	void Draw() const;

	void SetShader(std::shared_ptr<Shader> shader) { m_shader = shader; }
	std::shared_ptr<Texture> GetCubemap() const { return m_cubemap; }

private:
	void SetupMesh();
	void Cleanup();

	GLuint m_vao;
	GLuint m_vbo;
	std::shared_ptr<Texture> m_cubemap;
	std::shared_ptr<Shader> m_shader;
};