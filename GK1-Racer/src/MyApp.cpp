#include "MyApp.h"

#include <Engine/Objects/Skybox.h>
#include <Engine/Objects/Model.h>
#include <Engine/Objects/Camera.h>
#include <Engine/ResourceManager.h>
#include <Engine/Log.h>
#include <Engine/Renderer.h>
#include <Engine/Input.h>
#include <format>

#include "Config.h"

static std::vector<std::shared_ptr<GraphicsObject>> objects;
static std::shared_ptr<Skybox> skybox;
static Camera camera;

static glm::vec3 cubeRot = glm::vec3(0.0f);

MyApp::MyApp(std::string title, int width, int height)
	: App(title, width, height)
{
}

MyApp::~MyApp()
{
}

void MyApp::OnStart()
{
	Log::Info("App initialized");

	// Load skybox
	skybox = Skybox::LoadFromFiles(
		{
			"assets/textures/skybox/default/right.jpg",
			"assets/textures/skybox/default/left.jpg",
			"assets/textures/skybox/default/top.jpg",
			"assets/textures/skybox/default/bottom.jpg",
			"assets/textures/skybox/default/front.jpg",
			"assets/textures/skybox/default/back.jpg"
		}
	);
	auto shader = Shader::LoadFromFile("assets/shaders/skybox.vert", "assets/shaders/skybox.frag");
	skybox->SetShader(shader);

	auto &rm = ResourceManager::GetInstance();
	rm.Add("Skybox", shader);

	// Set camera properties
	camera.SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
	camera.LookAt(glm::vec3(1.0f, 0.0f, 0.0f));
	camera.SetPerspective(45.0f, (float)Config::WINDOW_WIDTH / Config::WINDOW_HEIGHT, 0.1f, 100.0f);

	auto model = Model::LoadFromFile("assets/models/test/untitled.obj");
	model->SetPosition(glm::vec3(5.0f, 0.0f, 0.0f));
	objects.push_back(model);
}

void MyApp::OnUpdate(float deltaTime)
{
	if (Input::IsKeyPressed(GLFW_KEY_ESCAPE))
		Close();

	cubeRot.y += 25.0f * deltaTime;
	cubeRot.x += 25.0f * deltaTime;

	glm::vec3 forward = camera.GetForward();
	glm::vec3 right = camera.GetRight();
	glm::vec3 up = camera.GetUp();

	// Camera movement
	glm::vec3 cameraSpeed = glm::vec3(2.5f * deltaTime);
	if (Input::IsKeyDown(GLFW_KEY_LEFT_SHIFT))
		cameraSpeed *= 2.0f;
	if (Input::IsKeyDown(GLFW_KEY_LEFT_CONTROL))
		cameraSpeed *= 0.5f;
	if (Input::IsKeyDown(GLFW_KEY_W))
		camera.Move(forward * cameraSpeed);
	if (Input::IsKeyDown(GLFW_KEY_S))
		camera.Move(-forward * cameraSpeed);
	if (Input::IsKeyDown(GLFW_KEY_A))
		camera.Move(-right * cameraSpeed);
	if (Input::IsKeyDown(GLFW_KEY_D))
		camera.Move(right * cameraSpeed);
	if (Input::IsKeyDown(GLFW_KEY_Q))
		camera.Move(-up * cameraSpeed);
	if (Input::IsKeyDown(GLFW_KEY_E))
		camera.Move(up * cameraSpeed);

	if (Input::IsKeyPressed(GLFW_KEY_TAB))
	{
		m_EnterGame = !m_EnterGame;
		GetWindow()->SetCursorEnabled(!m_EnterGame);
	}

	if (m_EnterGame)
	{
		double xpos, ypos;
		Input::GetMouseDelta(xpos, ypos);
		glm::vec2 mouseDelta(xpos, ypos);
		Log::Info(std::format("Mouse delta: ({}, {})", mouseDelta.x, mouseDelta.y));

		float sensitivity = 0.1f;
		mouseDelta *= sensitivity;

		camera.Rotate(glm::vec3(-mouseDelta.y, -mouseDelta.x, 0.0f));
	}
}

void MyApp::OnRender(Renderer *renderer)
{
	renderer->Clear(Config::CLEAR_COLOR);
	auto shader = ResourceManager::GetInstance().Get<Shader>("Skybox");
	glm::mat4 view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
	shader->Use();
	shader->SetMat4("view", view);
	shader->SetMat4("projection", camera.GetProjectionMatrix());
	skybox->Draw();
	for (const auto &obj : objects)
	{
		obj->SetRotation(cubeRot);
		auto meshes = std::dynamic_pointer_cast<Model>(obj)->GetMeshes();
		for (const auto &mesh : meshes)
		{
			auto shader = mesh->material->GetShader();
			shader->Use();
			shader->SetMat4("model", obj->GetModelMatrix());
			shader->SetMat4("view", camera.GetViewMatrix());
			shader->SetMat4("projection", camera.GetProjectionMatrix());
		}
		obj->Draw();
	}
}

void MyApp::OnImGuiRender()
{
	ImGuiIO &io = ImGui::GetIO(); (void)io;

	ImGui::ShowDemoWindow();

	ImGui::SetNextWindowSize(ImVec2(448.0, 0.0), ImGuiCond_Once);
	ImGui::SetNextWindowCollapsed(false, ImGuiCond_Once);
	ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x, 0), ImGuiCond_Always, ImVec2(1.0f, 0.0f));
	ImGui::Begin("Control Panel", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

	ImGui::SetWindowSize(ImVec2(ImGui::GetWindowWidth(), 0.0));
	ImGui::End();
}
