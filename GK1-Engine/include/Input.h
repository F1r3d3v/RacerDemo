#pragma once
#include <unordered_map>
#include <GLFW/glfw3.h>

class Input
{
public:
	static void Init(GLFWwindow* windowHandle);
	static void Update();

	static bool IsKeyPressed(int key);
	static bool IsKeyReleased(int key);
	static bool IsKeyDown(int key);
	static bool IsKeyUp(int key);

private:
	static std::unordered_map<int, bool> currentKeyState;
	static std::unordered_map<int, bool> previousKeyState;
	static GLFWwindow* window;

	Input() = default;

};
