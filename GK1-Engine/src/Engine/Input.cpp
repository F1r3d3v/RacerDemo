#include "Engine/Input.h"

std::unordered_map<int, bool> Input::currentKeyState;
std::unordered_map<int, bool> Input::previousKeyState;
std::unordered_map<int, bool> Input::currentMouseState;
std::unordered_map<int, bool> Input::previousMouseState;
double Input::currMouseX = 0.0;
double Input::currMouseY = 0.0;
double Input::lastMouseX = 0.0;
double Input::lastMouseY = 0.0;
double Input::scrollDelta = 0.0;
GLFWwindow* Input::window = nullptr;

void Input::Init(GLFWwindow* windowHandle)
{
	window = windowHandle;

	glfwSetScrollCallback(window,
		[](GLFWwindow *window, double xoffset, double yoffset)
		{
			scrollDelta = yoffset;
		}
	);
}

void Input::Update() {
	previousKeyState = currentKeyState;
	for (const auto& [key, _] : previousKeyState) {
		currentKeyState[key] = glfwGetKey(window, key) == GLFW_PRESS;
	}

	previousMouseState = currentMouseState;
	for (int button = GLFW_MOUSE_BUTTON_1; button <= GLFW_MOUSE_BUTTON_LAST; button++)
	{
		currentMouseState[button] = glfwGetMouseButton(window, button) == GLFW_PRESS;
	}

	lastMouseX = currMouseX;
	lastMouseY = currMouseY;
	glfwGetCursorPos(window, &currMouseX, &currMouseY);
	scrollDelta = 0.0;
}

bool Input::IsKeyPressed(int key) {
	bool currentState = IsKeyDown(key);
	bool wasPressed = previousKeyState.count(key) ? previousKeyState[key] : false;
	return currentState && !wasPressed;
}

bool Input::IsKeyDown(int key) {
	currentKeyState[key] = glfwGetKey(window, key) == GLFW_PRESS;
	return currentKeyState[key];
}

bool Input::IsKeyReleased(int key) {
	bool currentState = IsKeyDown(key);
	bool wasPressed = previousKeyState.count(key) ? previousKeyState[key] : false;
	return !currentState && wasPressed;
}

bool Input::IsKeyUp(int key) {
	return !IsKeyDown(key);
}

bool Input::IsMouseButtonPressed(int button)
{
	return currentMouseState[button] && !previousMouseState[button];
}

bool Input::IsMouseButtonReleased(int button)
{
	return !currentMouseState[button] && previousMouseState[button];
}

bool Input::IsMouseButtonDown(int button)
{
	return currentMouseState[button];
}

bool Input::IsMouseButtonUp(int button)
{
	return !currentMouseState[button];
}

void Input::GetMousePosition(double &outX, double &outY)
{
	outX = currMouseX;
	outY = currMouseY;
}

double Input::GetMouseX()
{
	return currMouseX;
}

double Input::GetMouseY()
{
	return currMouseY;
}

void Input::GetMouseDelta(double &outDeltaX, double &outDeltaY)
{
	outDeltaX = currMouseX - lastMouseX;
	outDeltaY = currMouseY - lastMouseY;
}

double Input::GetMouseScrollDelta()
{
	return scrollDelta;
}
