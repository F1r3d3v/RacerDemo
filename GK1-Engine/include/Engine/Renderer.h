#pragma once
#include <glm/glm.hpp>
#include <glad/gl.h>

#include <memory>

#include "Engine/Window.h"
#include "Engine/Objects/Camera.h"

class Renderer
{
public:

	struct RenderState
	{
		glm::vec4 clearColor;
		glm::vec3 ambientColor;
		// std::shared_ptr<Scene> scene;
		std::shared_ptr<Camera> camera;
	};

	Renderer(Window *window);
	Renderer() = default;
	~Renderer();

	void Clear(glm::vec4 color);
	void SetWireframe(bool enabled);
	bool GetWireframe() const;
	GLuint GetDepthBuffer() const { return m_DepthBuffer; }
	GLuint GetMatricesUBO() const { return m_ubo; }
	static glm::vec2 GetViewportSize();

private:
	static Window *m_Window;
	GLuint m_DepthBuffer;
	GLuint m_ubo;
};

