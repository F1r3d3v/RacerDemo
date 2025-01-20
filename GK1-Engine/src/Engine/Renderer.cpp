#include "Engine/Renderer.h"
#include "Engine/Log.h"

#include <GLFW/glfw3.h>

Window *Renderer::m_Window = nullptr;

Renderer::Renderer(Window *window)
{
	m_Window = window;
	glm::vec2 size = GetViewportSize();

	glGenRenderbuffers(1, &m_DepthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, m_DepthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, (GLsizei)size.x, (GLsizei)size.y);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_DepthBuffer);
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glGenBuffers(1, &m_ubo);

	glBindBuffer(GL_UNIFORM_BUFFER, m_ubo);
	glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glBindBufferRange(GL_UNIFORM_BUFFER, 0, m_ubo, 0, 2 * sizeof(glm::mat4));
}

Renderer::~Renderer()
{
}

void Renderer::Clear(glm::vec4 color)
{
	glClearColor(color.x * color.w, color.y * color.w, color.z * color.w, color.w);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

glm::vec2 Renderer::GetViewportSize()
{
	if (m_Window)
	{
		int width, height;
		glfwGetFramebufferSize(m_Window->GetHandle(), &width, &height);
		return glm::vec2(width, height);
	}
	else
	{
		return glm::vec2(0.0f);
	}
}
