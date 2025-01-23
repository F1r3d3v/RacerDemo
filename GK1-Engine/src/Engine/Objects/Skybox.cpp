#include "Engine/Objects/Skybox.h"

// Skybox vertices - a cube centered at origin
static constexpr float kSkyboxVertices[] = {         
	-1.0f,  1.0f, -1.0f, // Front face
	 1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,

	-1.0f, -1.0f, -1.0f, // Left face
	-1.0f, -1.0f,  1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,

	 1.0f, -1.0f, -1.0f, // Right face
	 1.0f,  1.0f, -1.0f,
	 1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f,  1.0f,

	-1.0f, -1.0f,  1.0f, // Back face
	 1.0f, -1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,

	-1.0f,  1.0f, -1.0f, // Top face
	-1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,

	-1.0f, -1.0f, -1.0f, // Bottom face
	 1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f,  1.0f
};

static constexpr char kSkyboxVertexShader[] = R"(
#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

layout (std140) uniform Matrices
{
    mat4 view;
    mat4 projection;
};

void main()
{
    TexCoords = aPos;
    mat4 _view = mat4(mat3(view)); // Remove translation
    vec4 pos = projection * _view * vec4(aPos, 1.0);
    gl_Position = pos.xyww; // Set z to w so it's always 1.0
}
)";

static constexpr char kSkyboxFragmentShader[] = R"(
#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skyboxDay;
uniform samplerCube skyboxNight;
uniform float blendFactor;
uniform int hasNight;

void main()
{    
	vec3 day = texture(skyboxDay, TexCoords).rgb;

	if (hasNight == 0)
	{
		FragColor = vec4(day, 1.0);
		return;
	}

	vec3 night = texture(skyboxNight, TexCoords).rgb;
	vec3 color = mix(night, day, blendFactor);
	FragColor = vec4(color, 1.0);
}
)";


Skybox::Skybox() : m_vao(0), m_vbo(0)
{
	SetupGeometry();
	m_shader = Shader::LoadFromString(kSkyboxVertexShader, kSkyboxFragmentShader);
	m_shader->BindUBO("Matrices", 0);
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
	skybox->m_dayCubemap = cubemap;
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

void Skybox::SetupGeometry()
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
	if (!m_shader || !m_dayCubemap) return;

	// Store current OpenGL state
	GLint oldCullFaceMode;
	glGetIntegerv(GL_CULL_FACE_MODE, &oldCullFaceMode);
	GLint oldDepthFuncMode;
	glGetIntegerv(GL_DEPTH_FUNC, &oldDepthFuncMode);

	glCullFace(GL_FRONT);
	glDepthFunc(GL_LEQUAL);

	int kSlot = 0;
	m_shader->Use();
	m_dayCubemap->Bind(kSlot);
	m_shader->SetInt("skyboxDay", kSlot);
	m_shader->SetInt("hasNight", m_nightCubemap ? 1 : 0);
	if (m_nightCubemap)
	{
		m_nightCubemap->Bind(++kSlot);
		m_shader->SetInt("skyboxNight", kSlot);
		m_shader->SetFloat("blendFactor", m_blendFactor);
	}

	glBindVertexArray(m_vao);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);

	// Restore OpenGL state
	glCullFace(oldCullFaceMode);
	glDepthFunc(oldDepthFuncMode);
}
