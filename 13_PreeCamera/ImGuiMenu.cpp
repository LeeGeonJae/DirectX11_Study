#include "ImGuiMenu.h"
#include "../Engine/TimeManager.h"
#include "Model.h";
#include "Node.h"
#include "Struct.h"
#include "ResourceManager.h"
#include "Material.h"
#include "SkeletalMesh.h"

#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>

#include <dxgidebug.h>
#include <Psapi.h>
#include <directxtk/GeometricPrimitive.h>

#pragma comment(lib,"dxgi.lib")

float ImGuiMenu::CameraFov = 50.f * 3.14f / 180.f;
float ImGuiMenu::CameraNearFar[2] = { 1.f , 30000.0f };

Vector3 ImGuiMenu::ModelPosition = {};
Vector2 ImGuiMenu::ModelRotation = {};
Vector3 ImGuiMenu::ModelScale = { 1.f, 1.f, 1.f };
float ImGuiMenu::ModelAnimationSpeed = 1.f;

Vector4 ImGuiMenu::DirectionLightDir = { 0.f, 0.f, 1.f, 0.0f };
Vector4 ImGuiMenu::DirectionLightColor = { 1.f, 1.f, 1.f, 1.f };
Vector3 ImGuiMenu::AmbientColor = { 0.1f, 0.1f, 0.1f };
float ImGuiMenu::SpecularPower = 50.f;

bool ImGuiMenu::bIsNormalMap = true;
bool ImGuiMenu::bIsSpecularMap = true;
bool ImGuiMenu::bIsEmissiveMap = true;
bool ImGuiMenu::bIsGammaCorrection = true;
float ImGuiMenu::EmissivePower = 1.f;
float ImGuiMenu::OpacityValue = 0.5f;

bool ImGuiMenu::DyingAnimationFBX = false;
bool ImGuiMenu::SkinningTestFBX = false;
bool ImGuiMenu::ZeldaFBX = false;
bool ImGuiMenu::VampireFBX = false;
bool ImGuiMenu::cerberusFBX = false;

ImGuiMenu::ImGuiMenu(DemoApp* owner)
	:m_Owner(owner)
{
}

ImGuiMenu::~ImGuiMenu()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	if (!m_DXGIFactory)
	{
		m_DXGIFactory->Release();
		m_DXGIFactory = nullptr;
	}
	if (!m_DXGIAdapter)
	{
		m_DXGIAdapter->Release();
		m_DXGIAdapter = nullptr;
	}
}

void ImGuiMenu::Init(HWND hwnd, ID3D11Device* device, ID3D11DeviceContext* devicecontext)
{
	HRESULT hr = CreateDXGIFactory1(__uuidof(IDXGIFactory4), (void**)&m_DXGIFactory);
	hr = m_DXGIFactory->EnumAdapters(0, reinterpret_cast<IDXGIAdapter**>(&m_DXGIAdapter));

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
	static float currentTime = 0.f;
	static unsigned int fpsCount = 0;
	static string fps;
	static string pfs;
	durationTime += TimeManager::GetInstance()->GetfDT();
	currentTime += TimeManager::GetInstance()->GetfDT();
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

			ImGui::SliderFloat("Camera Fov", &camerafov, 0.01f, 180.f);
			ImGui::SliderFloat2("Camera Near&Far", CameraNearFar, 1.f, 30000.f);

			CameraFov = camerafov * 3.14f / 180.f;

			if (CameraNearFar[0] > CameraNearFar[1])
			{
				CameraNearFar[0] = CameraNearFar[1] - 0.1f;
			}
		}

		// Cube
		if (ImGui::CollapsingHeader("Model Position Setting"))
		{
			ImGui::SliderFloat3("Model Position", (float*)&ModelPosition, -20.f, 20.f);
			ImGui::SliderFloat3("Model Scale", (float*)&ModelScale, 0.f, 20.f);
			ImGui::SliderFloat2("Model Rotation", (float*)&ModelRotation, -1.f, 1.f);
		}

		ImGui::End();
	}

	// Material
	{
		ImGui::Begin("Material Menu");

		// ¸Ê Ã¼Å©
		ImGui::Checkbox("NormalMap Check", &bIsNormalMap);
		ImGui::Checkbox("SpecularMap Check", &bIsSpecularMap);
		ImGui::Checkbox("EmissiveMap Check", &bIsEmissiveMap);
		ImGui::Checkbox("GammaCorrection Check", &bIsGammaCorrection);

		ImGui::SliderFloat("OpacityValue", &OpacityValue, 0.f, 1.f);
		ImGui::SliderFloat("EmissivePower", &EmissivePower, 0.f, 5.f);
		ImGui::SliderFloat("Model Animation Speed", &ModelAnimationSpeed, 0.f, 5.f);

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

	// Resource
	{
		ImGui::Begin("Resource Menu");

		if (ImGui::CollapsingHeader("Resource"))
		{
			DyingAnimationFBX = ImGui::Button("Dying Animation FBX Create");
			SkinningTestFBX = ImGui::Button("Skinning Test Animation FBX Create");
			ZeldaFBX =ImGui::Button("Zelda FBX Create");
			VampireFBX = ImGui::Button("Vampire FBX Create");
			cerberusFBX = ImGui::Button("cerberus Create");
		}

		if (durationTime >= 1.f)
		{
			fps = "FPS : ";
			pfs = "PFS(second) : ";
			fps += to_string(fpsCount);
			pfs += to_string(durationTime / fpsCount);

			durationTime += -1.f;
			fpsCount = 0;


		}

		string str;

		ImGui::Text(fps.c_str());
		ImGui::Text(pfs.c_str());

		str = to_string(currentTime);
		ImGui::Text("Time : %s", str.c_str());
		getVideoMemoryInfo(str);
		ImGui::Text("VideoMemory: %s", str.c_str());
		getSystemMemoryInfo(str);
		ImGui::Text("SystemMemory: %s", str.c_str());

		ImGui::End();
	}

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void ImGuiMenu::getVideoMemoryInfo(std::string& out)
{
	DXGI_QUERY_VIDEO_MEMORY_INFO videoMemoryInfo;
	m_DXGIAdapter->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &videoMemoryInfo);

	out = std::to_string(videoMemoryInfo.CurrentUsage / 1024 / 1024) + " MB" + "/" + std::to_string(videoMemoryInfo.Budget / 1024 / 1024) + " MB";
}

void ImGuiMenu::getSystemMemoryInfo(std::string& out)
{
	HANDLE hProcess = GetCurrentProcess();
	PROCESS_MEMORY_COUNTERS_EX pmc;
	pmc.cb = sizeof(PROCESS_MEMORY_COUNTERS_EX);
	GetProcessMemoryInfo(hProcess, (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
	out = std::to_string((pmc.PagefileUsage) / 1024 / 1024) + " MB";
}

void ImGuiMenu::Release()
{

}
