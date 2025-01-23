#include "MyApp.h"

#include <Engine/Objects/Skybox.h>
#include <Engine/Objects/Model.h>
#include <Engine/Objects/Camera.h>
#include <Engine/Objects/Light/PointLight.h>
#include <Engine/Objects/Terrain.h>
#include <Engine/ResourceManager.h>
#include <Engine/Log.h>
#include <Engine/Renderer.h>
#include <Engine/Input.h>
#include <Engine/Scene.h>
#include <format>

#include "Config.h"

static std::unique_ptr<Scene> scene;
static std::shared_ptr<Skybox> skybox;
static std::shared_ptr<Model> model1;
static std::shared_ptr<Model> model2;
static std::shared_ptr<Terrain> terrain;

static glm::vec3 cubeRot = glm::vec3(0.0f);
static glm::vec3 cubePos = glm::vec3(0.0f);
static glm::vec3 rot;

MyApp::MyApp(std::string title, int width, int height)
	: App(title, width, height)
{
	SetVSync(false);
}

MyApp::~MyApp()
{
}

void MyApp::OnStart()
{
	Log::Info("App initialized");

	scene = std::make_unique<Scene>();

	// Load skybox
	skybox = Skybox::LoadFromFiles(
		{
			"assets/textures/skybox/miramar/front.tga",
			"assets/textures/skybox/miramar/back.tga",
			"assets/textures/skybox/miramar/top.tga",
			"assets/textures/skybox/miramar/bottom.tga",
			"assets/textures/skybox/miramar/right.tga",
			"assets/textures/skybox/miramar/left.tga",
		}
	);
	auto shader = Shader::LoadFromFile("assets/shaders/skybox.vert", "assets/shaders/skybox.frag");
	unsigned int uniformBlockIndex = glGetUniformBlockIndex(shader->GetID(), "Matrices");
	glUniformBlockBinding(shader->GetID(), uniformBlockIndex, 0);
	skybox->SetShader(shader);

	// Set camera properties
	auto camera = std::make_shared<Camera>();
	camera->SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
	camera->LookAt(glm::vec3(0.0f, 0.0f, -1.0f));
	camera->SetPerspective(45.0f, (float)Config::WINDOW_WIDTH / Config::WINDOW_HEIGHT, 0.1f, 2000.0f);
	scene->SetCamera(camera);

	model1 = Model::LoadFromFile("assets/models/ball/ball.obj");
	model1->SetPosition(glm::vec3(5.0f, 0.0f, 0.0f));
	model1->SetRotation(glm::vec3(0.0f, 00.0f, 0.0f));
	model1->SetScale(glm::vec3(0.5f));
	auto node1 = scene->AddObject(model1);

	model2 = Model::LoadFromFile("assets/models/eye/eyeball.obj");
	model2->SetPosition(glm::vec3(0.0f, 5.0f, 0.0f));
	model2->SetScale(glm::vec3(0.5f));
	scene->AddObject(model2, node1.get());

	auto light = std::make_shared<PointLight>(Light::Properties{ glm::vec3(1.0f), 1.0f, 100.0f });
	light->SetPosition(glm::vec3(0.0f, 5.0f, 10.0f));
	scene->AddLight(light);

	auto heightmap = Texture::LoadFromFile("assets/textures/terrain1/terrain_height.png", Texture::TextureType::Height, false);
	terrain = std::make_shared<Terrain>();
	terrain->SetHeightmap(heightmap);
	auto material = terrain->GetMaterial();
	material->SetProperties({ glm::vec3(0.1f), glm::vec3(1.0f), glm::vec3(1.0f), 256 });
	auto tex = Texture::LoadFromFile("assets/textures/terrain1/terrain_diffuse.png");
	material->AddTexture(Texture::TextureType::Ambient, tex);
	material->AddTexture(Texture::TextureType::Diffuse, tex);
	terrain->SetHeightScale(128.0);
	terrain->SetWorldScale(0.5);
	terrain->SetPosition({ 0.0f, -24, 0.0f });

	scene->AddObject(terrain);
}

void MyApp::OnUpdate(float deltaTime)
{
	if (Input::IsKeyPressed(GLFW_KEY_ESCAPE))
		Close();

	if (Input::IsKeyPressed(GLFW_KEY_F1))
		SetWireframe(!GetWireframe());

	terrain->SetRotation(rot);

	auto camera = scene->GetCamera();

	// Cube pos and rot
	cubePos.x = 5.0f * (float)glm::sin(glfwGetTime()/2);
	cubePos.z = 5.0f * (float)glm::cos(glfwGetTime()/2);
	cubePos.y = 0.0f;
	cubeRot.y += 30.0f * deltaTime;
	cubeRot.x += 30.0f * deltaTime;

	glm::vec3 forward = camera->GetForward();
	glm::vec3 right = camera->GetRight();
	glm::vec3 up = camera->GetUp();

	// Camera movement
	glm::vec3 cameraSpeed = glm::vec3(5.0f * deltaTime);
	if (Input::IsKeyDown(GLFW_KEY_LEFT_SHIFT))
		cameraSpeed *= 10.0f;
	if (Input::IsKeyDown(GLFW_KEY_LEFT_CONTROL))
		cameraSpeed *= 0.5f;
	if (Input::IsKeyDown(GLFW_KEY_W))
		camera->Move(forward * cameraSpeed);
	if (Input::IsKeyDown(GLFW_KEY_S))
		camera->Move(-forward * cameraSpeed);
	if (Input::IsKeyDown(GLFW_KEY_A))
		camera->Move(-right * cameraSpeed);
	if (Input::IsKeyDown(GLFW_KEY_D))
		camera->Move(right * cameraSpeed);
	if (Input::IsKeyDown(GLFW_KEY_Q))
		camera->Move(-up * cameraSpeed);
	if (Input::IsKeyDown(GLFW_KEY_E))
		camera->Move(up * cameraSpeed);

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

		float sensitivity = 0.1f;
		mouseDelta *= sensitivity;

		camera->Rotate(glm::vec3(-mouseDelta.y, -mouseDelta.x, 0.0f));
	}
}

void MyApp::OnRender(Renderer *renderer)
{
	renderer->Clear(Config::CLEAR_COLOR);
	model1->SetRotation(cubeRot);
	model1->SetPosition(cubePos);
	model2->SetRotation(glm::cross(glm::vec3(2.0f), cubeRot));

	scene->Draw(renderer);
	skybox->Draw();
}

void MyApp::OnImGuiRender()
{
	ImGuiIO &io = ImGui::GetIO(); (void)io;

	ImGui::SetNextWindowSize(ImVec2(448.0, 0.0), ImGuiCond_Once);
	ImGui::SetNextWindowCollapsed(false, ImGuiCond_Once);
	ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x, 0), ImGuiCond_Always, ImVec2(1.0f, 0.0f));
	ImGui::Begin("Control Panel", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

	// Rotation
	ImGui::SliderFloat3("Rot", &rot[0], -90.0f, 90.0f);

	ImGui::SetWindowSize(ImVec2(ImGui::GetWindowWidth(), 0.0));
	ImGui::End();
}

void MyApp::OnResize(int width, int height)
{
	scene->GetCamera()->SetViewportSize(width, height);
}
