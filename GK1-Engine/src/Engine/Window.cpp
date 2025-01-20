#include "Engine/Window.h"
#include "Engine/Log.h"

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <iostream>


Window::Window(int Width, int Height, std::string Title)
	: m_Title(Title)
{
	m_Handle = glfwCreateWindow(Width, Height, Title.c_str(), nullptr, nullptr);
	if (!m_Handle)
		throw std::runtime_error("Failed to create window");
}

Window::~Window()
{
	glfwDestroyWindow(m_Handle);
	m_Handle = nullptr;
	glfwTerminate();
}

int Window::GetWidth() const
{
	int width;
	glfwGetWindowSize(m_Handle, &width, nullptr);
	return width;
}

int Window::GetHeight() const
{
	int height;
	glfwGetWindowSize(m_Handle, nullptr, &height);
	return height;
}

void Window::SetTitle(const std::string &title)
{
	m_Title = title;
	glfwSetWindowTitle(m_Handle, title.c_str());
}

void Window::SetSize(int width, int height)
{
	glfwSetWindowSize(m_Handle, width, height);
}

void Window::SetCursorEnabled(bool enabled)
{
	glfwSetInputMode(m_Handle, GLFW_CURSOR, enabled ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
}
