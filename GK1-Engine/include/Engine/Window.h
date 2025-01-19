#pragma once

#include <stdexcept>
#include <string>

struct GLFWwindow;

class Window
{

public:
	Window(int Width, int Height, std::string Title);
	Window() = default;
	~Window();

	GLFWwindow* GetHandle() const { return m_Handle; }
	std::string GetTitle() const { return m_Title; }
	int GetWidth() const;
	int GetHeight() const;

	void SetTitle(const std::string &title);
	void SetSize(int width, int height);

	void SetCursorEnabled(bool enabled);
private:
	GLFWwindow* m_Handle;
	std::string m_Title;
};

