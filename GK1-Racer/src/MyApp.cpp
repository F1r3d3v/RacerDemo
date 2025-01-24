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
static std::shared_ptr<Camera> cameras[3];
static std::shared_ptr<PointLight> light2;
static std::shared_ptr<Model> model1;
static std::shared_ptr<Model> model2;

static glm::vec3 cubeRot = glm::vec3(0.0f);
static glm::vec3 cubePos = glm::vec3(0.0f);

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

	auto skybox = std::make_shared<Skybox>();
	skybox->SetCubemap(ResourceManager::Get().Load<Texture>("SkyboxDay"));
	skybox->SetNightCubemap(ResourceManager::Get().Load<Texture>("SkyboxNight"));
	scene->SetSkybox(skybox);

	cameras[0] = std::make_shared<Camera>();
	cameras[1] = std::make_shared<Camera>();
	cameras[2] = std::make_shared<Camera>();

	cameras[0]->SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
	cameras[0]->LookAt(glm::vec3(0.0f, 0.0f, 1.0f));
	cameras[0]->SetPerspective(45.0f, (float)Config::WINDOW_WIDTH / Config::WINDOW_HEIGHT, 0.1f, 2000.0f);

	cameras[1]->SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
	cameras[1]->SetPerspective(45.0f, (float)Config::WINDOW_WIDTH / Config::WINDOW_HEIGHT, 0.1f, 2000.0f);

	cameras[2]->SetPosition(glm::vec3(-32.0f, 16.0f, 16.0f));
	cameras[2]->SetPerspective(60.0f, (float)Config::WINDOW_WIDTH / Config::WINDOW_HEIGHT, 0.1f, 2000.0f);

	scene->SetCamera(cameras[0]);

	model1 = Model::LoadFromFile("assets/models/ball/ball.obj");
	model1->SetPosition(glm::vec3(5.0f, 0.0f, 0.0f));
	auto node1 = scene->AddObject(model1);

	model2 = Model::LoadFromFile("assets/models/eye/eyeball.obj");
	model2->SetPosition(glm::vec3(0.0f, 5.0f, 0.0f));
	model2->SetScale(glm::vec3(0.5f));
	auto node2 = scene->AddObject(model2, node1.get());

	auto light = std::make_shared<PointLight>(Light::Properties{ glm::vec3(0.8f), 1.0f, 100.0f });
	light->SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
	scene->AddLight(light);
	cameras[2]->LookAt(light->GetPosition());

	light2 = std::make_shared<SpotLight>(Light::Properties{ glm::vec3(1.0f, 0.0f, 0.0f), 1.0f, 100.0f });
	light2->LookAt(glm::vec3(0.0f, -1.0f, 0.0f));
	scene->AddLight(light2, node1.get());

	auto terrain = std::make_shared<Terrain>();
	terrain->SetHeightmap(ResourceManager::Get().Load<Texture>("TerrainHeight"));
	auto shader = terrain->GetMaterial()->GetShader();
	auto mat = ResourceManager::Get().Load<Material>("TerrainMaterial");
	mat->SetShader(shader);
	terrain->SetMaterial(mat);
	terrain->SetHeightScale(128.0);
	terrain->SetWorldScale(0.5);
	terrain->SetPosition({ 0.0f, -15, 0.0f });

	scene->AddObject(terrain);
}

void MyApp::OnLoad(ResourceManager *rm)
{
	rm->Add<Texture>("SkyboxDay", Texture::CreateCubemap(
		{
			"assets/textures/skybox/miramar/front.tga",
			"assets/textures/skybox/miramar/back.tga",
			"assets/textures/skybox/miramar/top.tga",
			"assets/textures/skybox/miramar/bottom.tga",
			"assets/textures/skybox/miramar/right.tga",
			"assets/textures/skybox/miramar/left.tga",
		}
		));

	rm->Add<Texture>("SkyboxNight", Texture::CreateCubemap(
		{
			"assets/textures/skybox/night/right.png",
			"assets/textures/skybox/night/left.png",
			"assets/textures/skybox/night/top.png",
			"assets/textures/skybox/night/bottom.png",
			"assets/textures/skybox/night/back.png",
			"assets/textures/skybox/night/front.png",
		}
		));

	rm->Add<Texture>("TerrainHeight", Texture::LoadFromFile("assets/textures/terrain/terrain_height.png", Texture::TextureType::Height, false));
	rm->Add<Texture>("TerrainDiffuse", Texture::LoadFromFile("assets/textures/terrain/terrain_diffuse.png"));

	auto mat = rm->Create<Material>("TerrainMaterial");
	mat->SetProperties({ glm::vec3(0.7f), glm::vec3(1.0f), glm::vec3(0.1f), 8 });
	mat->AddTexture(Texture::TextureType::Ambient, rm->Load<Texture>("TerrainDiffuse"));
	mat->AddTexture(Texture::TextureType::Diffuse, rm->Load<Texture>("TerrainDiffuse"));
}

void MyApp::OnUpdate(float deltaTime)
{
	if (Input::IsKeyPressed(GLFW_KEY_ESCAPE))
		Close();

	if (Input::IsKeyPressed(GLFW_KEY_F1))
		SetWireframe(!GetWireframe());

	cameras[1]->LookAt(model1->GetPosition());
	auto camera = scene->GetCamera();

	// Cube pos and rot
	cubePos.x = 5.0f * (float)glm::sin(glfwGetTime() / 2);
	cubePos.z = 5.0f * (float)glm::cos(glfwGetTime() / 2);
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

		camera->Rotate(glm::vec3(-mouseDelta.y, mouseDelta.x, 0.0f));
	}
}

void MyApp::OnRender(Renderer *renderer)
{
	renderer->Clear(Config::CLEAR_COLOR);
	model1->SetRotation(cubeRot);
	model1->SetPosition(cubePos);
	model2->SetRotation(glm::cross(glm::vec3(2.0f), cubeRot));

	scene->Draw(renderer);
}

void MyApp::OnImGuiRender()
{
	ImGuiIO &io = ImGui::GetIO(); (void)io;

	ImGui::SetNextWindowSize(ImVec2(448.0, 0.0), ImGuiCond_Once);
	ImGui::SetNextWindowCollapsed(false, ImGuiCond_Once);
	ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x, 0), ImGuiCond_Always, ImVec2(1.0f, 0.0f));
	ImGui::Begin("Control Panel", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

	// Set Camera index via combo
	ImGui::Text("Camera");
	if (ImGui::Combo("##Camera", &m_SelectedCamera, "Camera 1\0Camera 2\0Camera 3\0"))
		scene->SetCamera(cameras[m_SelectedCamera]);

	// Write as text Camera position, rotation and FOV
	auto camera = scene->GetCamera();
	auto pos = camera->GetPosition();
	auto rot = camera->GetRotation();
	auto fov = camera->GetFov();
	ImGui::Text("Position: %.2f %.2f %.2f", pos.x, pos.y, pos.z);
	ImGui::Text("Rotation: %.2f %.2f %.2f", rot.x, rot.y, rot.z);
	ImGui::Text("FOV: %.2f", fov);

	// Write as text Model position and rotation
	auto modelPos = model2->GetWorldPosition();
	auto modelRot = model2->GetRotation();
	ImGui::Text("Model Position: %.2f %.2f %.2f", modelPos.x, modelPos.y, modelPos.z);
	ImGui::Text("Model Rotation: %.2f %.2f %.2f", modelRot.x, modelRot.y, modelRot.z);

	// Change camera perspective
	if (ImGui::CollapsingHeader("Camera Settings"))
	{
		bool perspective = camera->GetProjectionType() == Camera::ProjectionType::Perspective;
		float orthographicSize = camera->GetOrthographicSize();
		float nearPlane = camera->GetNearPlane();
		float farPlane = camera->GetFarPlane();

		if (!perspective)
		{
			if (ImGui::SliderFloat("Orthographic Size", &orthographicSize, 1.0f, 200.0f))
				camera->SetOrthographic(orthographicSize, camera->GetNearPlane(), camera->GetFarPlane());
			if (ImGui::SliderFloat("Near Plane", &nearPlane, -2000.0f, 1.0f))
				camera->SetOrthographic(camera->GetOrthographicSize(), nearPlane, camera->GetFarPlane());
			if (ImGui::SliderFloat("Far Plane", &farPlane, 1.0f, 2000.0f))
				camera->SetOrthographic(camera->GetOrthographicSize(), camera->GetNearPlane(), farPlane);
		}
		else
		{
			if (ImGui::SliderFloat("FOV", &fov, 30.0f, 120.0f))
				camera->SetFov(fov);
			if (ImGui::SliderFloat("Near Plane", &nearPlane, 0.1f, 100.0f))
				camera->SetPerspective(camera->GetFov(), camera->GetAspectRatio(), nearPlane, camera->GetFarPlane());
			if (ImGui::SliderFloat("Far Plane", &farPlane, 1.0f, 2000.0f))
				camera->SetPerspective(camera->GetFov(), camera->GetAspectRatio(), camera->GetNearPlane(), farPlane);
		}


		// Toggle wireframe
		bool wireframe = GetWireframe();
		if (ImGui::Checkbox("Wireframe", &wireframe))
			SetWireframe(wireframe);

		ImGui::SameLine();

		// Toggle perspective
		if (ImGui::Checkbox("Perspective", &perspective))
		{
			if (perspective)
				camera->SetPerspective(fov, camera->GetAspectRatio(), camera->GetNearPlane(), camera->GetFarPlane());
			else
				camera->SetOrthographic(camera->GetOrthographicSize(), camera->GetNearPlane(), camera->GetFarPlane());
		}
	}

	if (ImGui::CollapsingHeader("Scene"))
	{
		ImGui::SeparatorText("Skybox");

		float blendFactor = scene->GetSkybox()->GetBlendFactor();
		if (ImGui::SliderFloat("Day Time", &blendFactor, 0.0f, 1.0f))
		{
			scene->GetSkybox()->SetBlendFactor(blendFactor);
			scene->GetLightManager().SetAmbientIntensity(glm::vec3(blendFactor));
		}

		ImGui::SeparatorText("Fog");

		// Fog
		glm::vec3 fogColor;
		float fogDensity;
		scene->GetFog(fogColor, fogDensity);

		if (ImGui::ColorEdit3("Fog Color", &fogColor.x))
			scene->SetFog(fogColor, fogDensity);

		if (ImGui::SliderFloat("Fog Density", &fogDensity, 0.0f, 1.0f))
			scene->SetFog(fogColor, fogDensity);

		bool fogEnabled = scene->IsFogEnabled();
		if (ImGui::Checkbox("Enable Fog", &fogEnabled))
			scene->EnableFog(fogEnabled);
	}


	ImGui::SetWindowSize(ImVec2(ImGui::GetWindowWidth(), 0.0));
	ImGui::End();
}

void MyApp::OnResize(int width, int height)
{
	scene->GetCamera()->SetViewportSize(width, height);
}
