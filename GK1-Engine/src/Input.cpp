#include "Input.h"

std::unordered_map<int, bool> Input::currentKeyState;
std::unordered_map<int, bool> Input::previousKeyState;
GLFWwindow* Input::window = nullptr;

void Input::Init(GLFWwindow* windowHandle) {
	window = windowHandle;
}

void Input::Update() {
	previousKeyState = currentKeyState;
	currentKeyState.clear();

	// Update current key states
	for (const auto& [key, _] : previousKeyState) {
		currentKeyState[key] = glfwGetKey(window, key) == GLFW_PRESS;
	}
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