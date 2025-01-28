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

#include "Objects/Vehicle.h"
#include "Physics/BulletDebugDrawer.h"
#include "Config.h"

static std::unique_ptr<Scene> scene;
static std::shared_ptr<Camera> cameras[4];
static std::shared_ptr<PointLight> light2;
static std::shared_ptr<SpotLight> vehicleLight1;
static std::shared_ptr<SpotLight> vehicleLight2;
static std::shared_ptr<Model> model1;
static std::shared_ptr<Model> model2;
static std::shared_ptr<Terrain> terrain;
static std::shared_ptr<Vehicle> vehicle;
static BulletDebugDrawer *debugDrawer = nullptr;

static glm::vec3 cubeRot = glm::vec3(0.0f);
static glm::vec3 cubePos = glm::vec3(0.0f);

MyApp::MyApp(std::string title, int width, int height)
	: App(title, width, height)
	, m_physicsManager(std::make_unique<PhysicsManager>())
{
	// Physics simulation break when unlocking the framerate
	SetVSync(true);
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
	cameras[3] = std::make_shared<Camera>();

	cameras[0]->SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
	cameras[0]->SetPerspective(45.0f, (float)Config::WINDOW_WIDTH / Config::WINDOW_HEIGHT, 0.1f, 2000.0f);

	cameras[1]->SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
	cameras[1]->SetPerspective(45.0f, (float)Config::WINDOW_WIDTH / Config::WINDOW_HEIGHT, 0.1f, 2000.0f);

	cameras[2]->SetPosition(glm::vec3(-32.0f, 16.0f, 16.0f));
	cameras[2]->SetPerspective(60.0f, (float)Config::WINDOW_WIDTH / Config::WINDOW_HEIGHT, 0.1f, 2000.0f);

	cameras[3]->SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
	cameras[3]->SetPerspective(45.0f, (float)Config::WINDOW_WIDTH / Config::WINDOW_HEIGHT, 0.1f, 2000.0f);

	scene->SetCamera(cameras[0]);
	m_cameraController = std::make_unique<RacingCameraController>(cameras[0]);
	m_flyCameraController = std::make_unique<FlyCameraController>(cameras[3]);

	model1 = Model::LoadFromFile("assets/models/ball/ball.obj");
	model1->SetPosition(glm::vec3(5.0f, 0.0f, 0.0f));
	auto node1 = scene->AddObject(model1);

	model2 = Model::LoadFromFile("assets/models/eye/eyeball.obj");
	model2->SetPosition(glm::vec3(0.0f, 5.0f, 0.0f));
	model2->SetScale(glm::vec3(0.5f));
	auto node2 = scene->AddObject(model2, node1.get());

	auto light = std::make_shared<PointLight>(Light::Properties{ glm::vec3(0.0f, 0.0f, 1.0f), 1.0f, 100.0f });
	light->SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
	scene->AddLight(light);
	cameras[2]->LookAt(light->GetPosition());

	light2 = std::make_shared<SpotLight>(Light::Properties{ glm::vec3(1.0f, 0.0f, 0.0f), 1.0f, 100.0f });
	light2->LookAt(glm::vec3(0.0f, -1.0f, 0.0f));
	scene->AddLight(light2, node1.get());

	terrain = std::make_shared<Terrain>();
	terrain->SetHeightmap(ResourceManager::Get().Load<Texture>("TerrainHeight"));
	auto shader = terrain->GetMaterial()->GetShader();
	auto mat = ResourceManager::Get().Load<Material>("TerrainMaterial");
	mat->SetShader(shader);
	terrain->SetMaterial(mat);
	terrain->SetHeightScale(128.0);
	terrain->SetWorldScale(0.5);
	terrain->SetPosition({ 0.0f, -15, 0.0f });
	scene->AddObject(terrain);

	// Generate collision mesh for terrain
	std::vector<glm::vec3> vertices;
	std::vector<uint32_t> indices;
	terrain->GenerateCollisionMesh(1024, vertices, indices);
	btTriangleMesh *bulletMesh = new btTriangleMesh();
	for (size_t i = 0; i < indices.size(); i += 4)
	{
		glm::vec3 v0 = vertices[indices[i]]; // topLeft
		glm::vec3 v1 = vertices[indices[i + 1]]; //bottomLeft
		glm::vec3 v2 = vertices[indices[i + 2]]; //topRight
		glm::vec3 v3 = vertices[indices[i + 3]]; //bottomRight

		bulletMesh->addTriangle(
			btVector3(v0.x, v0.y, v0.z),
			btVector3(v1.x, v1.y, v1.z),
			btVector3(v2.x, v2.y, v2.z)
		);

		bulletMesh->addTriangle(
			btVector3(v2.x, v2.y, v2.z),
			btVector3(v1.x, v1.y, v1.z),
			btVector3(v3.x, v3.y, v3.z)
		);
	}
	btBvhTriangleMeshShape *terrainShape = new btBvhTriangleMeshShape(bulletMesh, true);
	btDefaultMotionState *motionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, -15.0, 0)));
	btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(0, motionState, terrainShape, btVector3(0, 0, 0));
	btRigidBody *terrainBody = new btRigidBody(rigidBodyCI);
	m_physicsManager->AddRigidBody(terrainBody);

	auto wheelModel = Model::LoadFromFile("assets/models/pickup_car/Wheel.obj");
	auto vehicleModel = Model::LoadFromFile("assets/models/pickup_car/Hilux.obj");
	vehicle = std::make_shared<Vehicle>(
		m_physicsManager->GetDynamicsWorld(),
		vehicleModel, wheelModel,
		glm::vec3(0, -2, 10)
	);
	auto vehicleNode = scene->AddObject(vehicle);
	vehicleLight1 = std::make_shared<SpotLight>(Light::Properties{ glm::vec3(1.0f, 1.0f, 1.0f), 0.0f, 200.0f });
	vehicleLight1->LookAt(glm::vec3(0.0f, 0.0f, 1.0f));
	vehicleLight1->SetPosition(glm::vec3(-0.755f, -0.1f, 2.2f));
	scene->AddLight(vehicleLight1, vehicleNode.get());
	vehicleLight2 = std::make_shared<SpotLight>(Light::Properties{ glm::vec3(1.0f, 1.0f, 1.0f), 0.0f, 200.0f });
	vehicleLight2->LookAt(glm::vec3(0.0f, 0.0f, 1.0f));
	vehicleLight2->SetPosition(glm::vec3(0.75f, -0.1f, 2.2f));
	scene->AddLight(vehicleLight2, vehicleNode.get());

	auto cube = Model::LoadFromFile("assets/models/cottage/cottage_obj.obj");
	cube->SetPosition(glm::vec3(50.0f, -4.0f, -30.0f));
	scene->AddObject(cube);

	//debugDrawer = new BulletDebugDrawer();
	//m_physicsManager->GetDynamicsWorld()->setDebugDrawer(debugDrawer);
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
		m_controlPanel = !m_controlPanel;

	if (Input::IsKeyPressed(GLFW_KEY_F2))
		SetWireframe(!GetWireframe());

	if (Input::IsKeyPressed(GLFW_KEY_TAB))
	{
		m_EnterGame = !m_EnterGame;
		GetWindow()->SetCursorEnabled(!m_EnterGame);
	}

	if (m_EnterGame)
	{
		if (Input::IsKeyPressed(GLFW_KEY_1))
		{
			scene->SetCamera(cameras[0]);
			m_SelectedCamera = 0;
		}
		else if (Input::IsKeyPressed(GLFW_KEY_2))
		{
			scene->SetCamera(cameras[1]);
			m_SelectedCamera = 1;
		}
		else if (Input::IsKeyPressed(GLFW_KEY_3))
		{
			scene->SetCamera(cameras[2]);
			m_SelectedCamera = 2;
		}
		else if (Input::IsKeyPressed(GLFW_KEY_4))
		{
			scene->SetCamera(cameras[3]);
			m_SelectedCamera = 3;
		}
	}

	if (m_SelectedCamera == 3)
	{
		if (m_EnterGame)
			m_flyCameraController->Update(deltaTime);
	}

	cubePos.x = 5.0f * (float)glm::sin(glfwGetTime() / 2);
	cubePos.z = 5.0f * (float)glm::cos(glfwGetTime() / 2);
	cubePos.y = 0.0f;
	cubeRot.y += 30.0f * deltaTime;
	cubeRot.x += 30.0f * deltaTime;

	cameras[1]->LookAt(model1->GetPosition());

	auto controller = vehicle->GetController();
	controller->GetChassisBody()->activate(true);

	if (m_SelectedCamera == 0)
	{
		static float steeringAngle = 0.0f;
		const float maxSteeringAngle = 0.65f;
		const float steeringSpeed = 1.0f;

		float targetSteeringAngle = 0.0f;
		if (Input::IsKeyDown(GLFW_KEY_A) && m_EnterGame)
			targetSteeringAngle = maxSteeringAngle;
		else if (Input::IsKeyDown(GLFW_KEY_D) && m_EnterGame)
			targetSteeringAngle = -maxSteeringAngle;

		if (steeringAngle < targetSteeringAngle)
		{
			steeringAngle += steeringSpeed * deltaTime;
			if (steeringAngle > targetSteeringAngle)
				steeringAngle = targetSteeringAngle;
		}
		else if (steeringAngle > targetSteeringAngle)
		{
			steeringAngle -= steeringSpeed * deltaTime;
			if (steeringAngle < targetSteeringAngle)
				steeringAngle = targetSteeringAngle;
		}

		controller->Steer(steeringAngle);

		static float engineForce = 0.0f;
		const float maxEngineForce = 500.0f;
		const float engineForceSpeed = 750.0f;

		float targetEngineForce = 0.0f;
		if (Input::IsKeyDown(GLFW_KEY_W) && m_EnterGame)
			targetEngineForce = maxEngineForce;
		else if (Input::IsKeyDown(GLFW_KEY_S) && m_EnterGame)
			targetEngineForce = -maxEngineForce;

		if (engineForce < targetEngineForce)
		{
			engineForce += engineForceSpeed * deltaTime;
			if (engineForce > targetEngineForce)
				engineForce = targetEngineForce;
		}
		else if (engineForce > targetEngineForce)
		{
			engineForce -= engineForceSpeed * deltaTime;
			if (engineForce < targetEngineForce)
				engineForce = targetEngineForce;
		}

		controller->ApplyEngineForce(engineForce);

		float brakeForce = 0.0f;
		if (Input::IsKeyDown(GLFW_KEY_SPACE) && m_EnterGame)
			brakeForce = 25.0f;

		controller->Brake(brakeForce);

		if (Input::IsKeyPressed(GLFW_KEY_R) && m_EnterGame)
			controller->Flip();
	}
	else
	{
		controller->Steer(0.0f);
		controller->ApplyEngineForce(0.0f);
		controller->Brake(25.0f);
	}

	vehicle->Update(deltaTime);
	m_physicsManager->Update(deltaTime);

	btVector3 forwardVec = controller->GetBulletVehicle()->getForwardVector();
	m_cameraController->SetTarget(
		controller->GetPosition(),
		glm::vec3(forwardVec.x(), forwardVec.y(), forwardVec.z()),
		vehicle->GetWorldUp(),
		controller->GetVehicleVelocity()
	);
	m_cameraController->Update(deltaTime);
}

void MyApp::OnRender(Renderer *renderer)
{
	renderer->Clear(Config::CLEAR_COLOR);
	model1->SetRotation(cubeRot);
	model1->SetPosition(cubePos);
	model2->SetRotation(glm::cross(glm::vec3(2.0f), cubeRot));

	scene->Draw(renderer);
	//static bool first = true;
	//if (first)
	//{
	//	first = false;
	//}
	//m_physicsManager->GetDynamicsWorld()->debugDrawWorld();
	//debugDrawer->Draw();
}

void MyApp::OnImGuiRender()
{
	ImGuiIO &io = ImGui::GetIO(); (void)io;

	if (m_controlPanel)
	{
		ImGui::SetNextWindowSize(ImVec2(448.0, 0.0), ImGuiCond_Once);
		ImGui::SetNextWindowCollapsed(false, ImGuiCond_Once);
		ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x, 0), ImGuiCond_Always, ImVec2(1.0f, 0.0f));
		ImGui::Begin("Control Panel", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

		ImGui::Text("Camera");
		if (ImGui::Combo("##Camera", &m_SelectedCamera, "Camera 1\0Camera 2\0Camera 3\0Camera 4\0"))
			scene->SetCamera(cameras[m_SelectedCamera]);

		auto camera = scene->GetCamera();
		auto pos = camera->GetPosition();
		auto rot = camera->GetRotation();
		auto fov = camera->GetFov();
		auto forward = camera->GetForward();
		auto right = camera->GetRight();
		auto up = camera->GetUp();
		ImGui::Text("Camera Forward: %.2f %.2f %.2f", forward.x, forward.y, forward.z);
		ImGui::Text("Camera Right: %.2f %.2f %.2f", right.x, right.y, right.z);
		ImGui::Text("Camera Up: %.2f %.2f %.2f", up.x, up.y, up.z);
		ImGui::Text("Camera Position: %.2f %.2f %.2f", pos.x, pos.y, pos.z);
		ImGui::Text("Camera Rotation: %.2f %.2f %.2f", rot.x, rot.y, rot.z);

		auto vehiclePos = vehicle->GetController()->GetPosition();
		auto vehicleRot = glm::degrees(glm::eulerAngles(vehicle->GetController()->GetOrientation()));
		auto forwardVec = vehicle->GetController()->GetBulletVehicle()->getForwardVector();
		auto velocity = vehicle->GetController()->GetVehicleVelocity();
		ImGui::Text("Vehicle Forward: %.2f %.2f %.2f", forwardVec.x(), forwardVec.y(), forwardVec.z());
		ImGui::Text("Vehicle Position: %.2f %.2f %.2f", vehiclePos.x, vehiclePos.y, vehiclePos.z);
		ImGui::Text("Vehicle Rotation: %.2f %.2f %.2f", vehicleRot.x, vehicleRot.y, vehicleRot.z);
		ImGui::Text("Vehicle Velocity: %.2f %.2f %.2f", velocity.x, velocity.y, velocity.z);
		ImGui::Text("Vehicle Speed: %.2f", glm::length(velocity));

		if (ImGui::CollapsingHeader("Camera Settings"))
		{
			bool perspective = camera->GetProjectionType() == Camera::ProjectionType::Perspective;
			float orthographicSize = camera->GetOrthographicSize();
			float nearPlane = camera->GetNearPlane();
			float farPlane = camera->GetFarPlane();

			if (!perspective)
			{
				if (ImGui::SliderFloat("Orthographic Size", &orthographicSize, 1.0f, 300.0f))
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

			bool wireframe = GetWireframe();
			if (ImGui::Checkbox("Wireframe", &wireframe))
				SetWireframe(wireframe);

			ImGui::SameLine();

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
			glm::vec3 fogColor;
			float fogDensity;
			scene->GetFog(fogColor, fogDensity);
			float blendFactor = scene->GetSkybox()->GetBlendFactor();

			ImGui::SeparatorText("Skybox");
			if (ImGui::SliderFloat("Day Time", &blendFactor, 0.0f, 1.0f))
			{
				scene->GetSkybox()->SetBlendFactor(blendFactor);
				scene->GetLightManager()->SetAmbientIntensity(glm::vec3(blendFactor));
				scene->SetFog(glm::mix(glm::vec3(0.15f), glm::vec3(0.6f), blendFactor), fogDensity);
			}

			ImGui::SeparatorText("Fog");
			if (ImGui::ColorEdit3("Fog Color", &fogColor.x))
				scene->SetFog(fogColor, fogDensity);

			if (ImGui::SliderFloat("Fog Density", &fogDensity, 0.0f, 1.0f))
				scene->SetFog(fogColor, fogDensity);

			bool fogEnabled = scene->IsFogEnabled();
			if (ImGui::Checkbox("Enable Fog", &fogEnabled))
				scene->EnableFog(fogEnabled);
		}


		if (ImGui::CollapsingHeader("Vehicle"))
		{
			// Turn on/off vehicle lights
			static bool vehicleLights = false;
			if (ImGui::Checkbox("Vehicle Lights", &vehicleLights))
			{
				// Set intensity to 0 if lights are off
				float intensity = vehicleLights ? 1.0f : 0.0f;
				vehicleLight1->SetIntensity(intensity);
				vehicleLight2->SetIntensity(intensity);
			}

			static float lightAngle = 0;
			if (ImGui::SliderFloat("Light Angle", &lightAngle, -45.0f, 45.0f))
			{
				vehicleLight1->LookAt(vehicleLight1->GetPosition() + glm::vec3(0.0f, 0.0f, 1.0f));
				vehicleLight1->Rotate(glm::vec3(-lightAngle, 0.0f, 0.0f));

				vehicleLight2->LookAt(vehicleLight2->GetPosition() + glm::vec3(0.0f, 0.0f, 1.0f));
				vehicleLight2->Rotate(glm::vec3(-lightAngle, 0.0f, 0.0f));
			}
			// Front light focus
			float lightFocus = vehicleLight1->GetFocus();
			if (ImGui::SliderFloat("Light Focus", &lightFocus, 1.0f, 32.0f))
			{
				vehicleLight1->SetFocus(lightFocus);
				vehicleLight2->SetFocus(lightFocus);
			}

			if (ImGui::Button("Flip"))
				vehicle->GetController()->Flip();
		}

		if (ImGui::CollapsingHeader("Controls"))
		{
			ImGui::BulletText("F1 - Toggle Control Panel");
			ImGui::BulletText("F2 - Toggle Wireframe");
			ImGui::BulletText("TAB - Enter Game");
			ImGui::BulletText("1-4 - Switch Camera");
			ImGui::BulletText("WASD - Control Vehicle");
			ImGui::BulletText("Space - Brake");
			ImGui::BulletText("R - Flip Vehicle");
		}

		if (ImGui::CollapsingHeader("Documentation"))
		{
			ImGui::SeparatorText("Racing Camera");
			ImGui::TextWrapped("The racing camera is a custom camera controller that follows the vehicle.");
			ImGui::TextWrapped("It uses the vehicle's position, velocity, and orientation to calculate the camera's position and rotation.");
			ImGui::TextWrapped("The camera's position is calculated by interpolating between the current camera position and the target camera position.");
			ImGui::TextWrapped("The camera's target position is calculated by looking at the vehicle's position and adding offsets with given coefficients.");
			ImGui::TextWrapped("The camera's rotation is calculated by looking at the vehicle's position plus the vehicle's forward vector multiplied by a coefficient and speed.");
			ImGui::NewLine();
			ImGui::SeparatorText("Terrain");
			ImGui::TextWrapped("The terrain is generated from a heightmap texture.");
			ImGui::TextWrapped("The terrain is a grid of vertices with a given grid size.");
			ImGui::TextWrapped("The terrain is tessellated using the tessellation control and evaluation shaders.");
			ImGui::TextWrapped("The terrain tessellation level could be based on the distance from the camera for optimization.");
		}

		ImGui::SetWindowSize(ImVec2(ImGui::GetWindowWidth(), 0.0));
		ImGui::End();
	}
}

void MyApp::OnResize(int width, int height)
{
	scene->GetCamera()->SetViewportSize(width, height);
}
