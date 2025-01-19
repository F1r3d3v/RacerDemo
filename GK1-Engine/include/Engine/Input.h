#pragma once
#include <unordered_map>
#include <GLFW/glfw3.h>

class Input
{
public:
	static void Init(GLFWwindow* windowHandle);
	static void Update();

	// Keyboard keys
	static bool IsKeyPressed(int key);
	static bool IsKeyReleased(int key);
	static bool IsKeyDown(int key);
	static bool IsKeyUp(int key);

	// Mouse buttons
	static bool IsMouseButtonPressed(int button);
	static bool IsMouseButtonReleased(int button);
	static bool IsMouseButtonDown(int button);
	static bool IsMouseButtonUp(int button);

	// Mouse position
	static void GetMousePosition(double &outX, double &outY);
	static double GetMouseX();
	static double GetMouseY();
	static void GetMouseDelta(double &outDeltaX, double &outDeltaY);

	// Mouse scroll
	static double GetMouseScrollDelta();

private:
	static std::unordered_map<int, bool> currentKeyState;
	static std::unordered_map<int, bool> previousKeyState;
	static std::unordered_map<int, bool> currentMouseState;
	static std::unordered_map<int, bool> previousMouseState;
	static GLFWwindow* window;

	static double lastMouseX, lastMouseY;
	static double currMouseX, currMouseY;
	static double scrollDelta;

	Input() = default;

};
