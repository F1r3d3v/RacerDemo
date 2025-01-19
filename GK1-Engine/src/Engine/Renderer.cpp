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
}

Renderer::~Renderer()
{
}

unsigned int Renderer::LoadShader(const char *vertexShaderSource, const char *fragmentShaderSource)
{
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
	glCompileShader(vertexShader);

	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
		Log::Error("Vertex shader compilation failed: " + std::string(infoLog));
	}

	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
		Log::Error("Fragment shader compilation failed: " + std::string(infoLog));
	}

	unsigned int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
		Log::Error("Shader program linking failed: " + std::string(infoLog));
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return shaderProgram;
}

unsigned int Renderer::LoadShaderFromFile(const char *vertexShaderFilepath, const char *fragmentShaderFilepath)
{
	std::ifstream vertexShaderFile(vertexShaderFilepath);
	std::ifstream fragmentShaderFile(fragmentShaderFilepath);
	if (!vertexShaderFile.is_open())
		Log::Error("Failed to open vertex shader file: " + std::string(vertexShaderFilepath));
	if (!fragmentShaderFile.is_open())
		Log::Error("Failed to open fragment shader file: " + std::string(fragmentShaderFilepath));
	std::string vertexShaderSource((std::istreambuf_iterator<char>(vertexShaderFile)), std::istreambuf_iterator<char>());
	std::string fragmentShaderSource((std::istreambuf_iterator<char>(fragmentShaderFile)), std::istreambuf_iterator<char>());
	return LoadShader(vertexShaderSource.c_str(), fragmentShaderSource.c_str());
}

void Renderer::UnloadShader(unsigned int shaderProgram)
{
	glDeleteProgram(shaderProgram);
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
