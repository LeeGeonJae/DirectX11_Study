#include "ImGuiMenu.h"
#include "../Engine/TimeManager.h"

#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>

Vector4 ImGuiMenu::CameraPos = { 0.f, 0.f, -1000.f, 0.f };
float ImGuiMenu::CameraFov = 50.f * 3.14f / 180.f;
float ImGuiMenu::CameraNearFar[2] = { 0.01f , 30000.0f };

Vector3 ImGuiMenu::CubePosition = {};
Vector2 ImGuiMenu::CubeRotation = {};
Vector3 ImGuiMenu::CubeScale = { 1.f, 1.f, 1.f };

Vector4 ImGuiMenu::DirectionLightDir = { 0.f, 0.f, 1.f, 0.0f };
Vector4 ImGuiMenu::DirectionLightColor = { 1.f, 1.f, 1.f, 1.f };
Vector3 ImGuiMenu::AmbientColor = { 0.1f, 0.1f, 0.1f };
float ImGuiMenu::SpecularPower = 50.f;

bool ImGuiMenu::bIsNormalMap = true;
bool ImGuiMenu::bIsSpecularMap = true;
bool ImGuiMenu::bIsGammaCorrection = true;

ImGuiMenu::ImGuiMenu(DemoApp* owner)
	:m_Owner(owner)
{
}

ImGuiMenu::~ImGuiMenu()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void ImGuiMenu::Init(HWND hwnd, ID3D11Device* device, ID3D11DeviceContext* devicecontext)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(device, devicecontext);

	ImGui::SetNextWindowSize({ 500.f, 500.f });
}

void ImGuiMenu::Render()
{
	static float durationTime = 0.f;
	static unsigned int fpsCount = 0;
	static string fps;
	static string pfs;
	durationTime += TimeManager::GetInstance()->GetfDT();
	fpsCount++;

	// Start the Dear ImGui frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// Camera & Cube
	{
		ImGui::Begin("Camera & Cube Menu");

		// Camera
		if (ImGui::CollapsingHeader("Camera Setting"))
		{
			static float camerafov = 50.f;

			ImGui::SliderFloat4("Camera Position", (float*)&CameraPos, -600.f, 500.f);
			ImGui::SliderFloat("Camera Fov", &camerafov, 0.01f, 180.f);
			ImGui::SliderFloat2("Camera Near&Far", CameraNearFar, 0.1f, 30000.f);

			CameraFov = camerafov * 3.14f / 180.f;

			if (CameraNearFar[0] > CameraNearFar[1])
			{
				CameraNearFar[0] = CameraNearFar[1] - 0.1f;
			}
		}

		// �� üũ
		ImGui::Checkbox("NormalMap Check", &bIsNormalMap);
		ImGui::Checkbox("SpecularMap Check", &bIsSpecularMap);
		ImGui::Checkbox("GammaCorrection Check", &bIsGammaCorrection);

		if (durationTime >= 1.f)
		{
			fps = "FPS : ";
			pfs = "PFS(second) : ";
			fps += to_string(fpsCount);
			pfs += to_string(durationTime / fpsCount);

			fpsCount = 0;
			durationTime = -1.f;
		}

		ImGui::Text(fps.c_str());
		ImGui::Text(pfs.c_str());

		ImGui::End();
	}

	// Light
	{
		ImGui::Begin("Light Menu");

		if (ImGui::CollapsingHeader("Light"))
		{
			ImGui::SliderFloat4("Light Direction", (float*)&DirectionLightDir, -1.f, 1.f);
			ImGui::ColorEdit4("Light Color", (float*)&DirectionLightColor);
			ImGui::ColorEdit3("Ambient Color", (float*)&AmbientColor);
			ImGui::SliderFloat("Specular power", &SpecularPower, 1.f, 1500.f);
		}

		ImGui::End();
	}

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void ImGuiMenu::Release()
{

}
