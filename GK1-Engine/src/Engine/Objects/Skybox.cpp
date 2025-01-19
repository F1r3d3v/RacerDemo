#include "Engine/Objects/Skybox.h"

// Skybox vertices - a cube centered at origin
static constexpr float kSkyboxVertices[] = {
	// positions          
	-1.0f,  1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	-1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f
};

Skybox::Skybox() : m_vao(0), m_vbo(0)
{
	SetupMesh();
}

Skybox::~Skybox()
{
	Cleanup();
}

void Skybox::Cleanup()
{
	if (m_vbo != 0)
	{
		glDeleteBuffers(1, &m_vbo);
		m_vbo = 0;
	}
	if (m_vao != 0)
	{
		glDeleteVertexArrays(1, &m_vao);
		m_vao = 0;
	}
}

std::shared_ptr<Skybox> Skybox::LoadFromCubemap(std::shared_ptr<Texture> cubemap)
{
	if (!cubemap || cubemap->GetType() != Texture::TextureType::Cubemap)
	{
		return nullptr;
	}

	auto skybox = std::make_shared<Skybox>();
	skybox->m_cubemap = cubemap;
	return skybox;
}

std::shared_ptr<Skybox> Skybox::LoadFromFiles(const std::vector<std::string> &faces)
{
	auto cubemap = Texture::CreateCubemap(faces);
	if (!cubemap)
	{
		return nullptr;
	}
	return LoadFromCubemap(cubemap);
}

void Skybox::SetupMesh()
{
	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_vbo);

	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(kSkyboxVertices), &kSkyboxVertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);

	glBindVertexArray(0);
}

void Skybox::Draw()
{
	if (!m_shader || !m_cubemap) return;

	// Store current OpenGL state
	GLint oldCullFaceMode;
	glGetIntegerv(GL_CULL_FACE_MODE, &oldCullFaceMode);
	GLint oldDepthFuncMode;
	glGetIntegerv(GL_DEPTH_FUNC, &oldDepthFuncMode);

	glCullFace(GL_FRONT);
	glDepthFunc(GL_LEQUAL);

	constexpr unsigned int kSlot = 0;
	m_shader->Use();
	m_cubemap->Bind(kSlot);
	m_shader->SetInt("skybox", kSlot);

	glBindVertexArray(m_vao);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);

	// Restore OpenGL state
	glCullFace(oldCullFaceMode);
	glDepthFunc(oldDepthFuncMode);
}
