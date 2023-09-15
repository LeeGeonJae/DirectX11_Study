#include "ImGuiMenu.h"

#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>

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
	//ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(device, devicecontext);


	m_CameraPos[0] = 0.0f;
	m_CameraPos[1] = 1.0f;
	m_CameraPos[2] = -15.0f;
	m_CameraPos[3] = 0.0f;
	m_CameraFov = 50;
	m_CameraNearFar[0] = 0.01f;
	m_CameraNearFar[1] = 100.0f;

	m_FirstCubePosition = {};
	m_SecondCubePosition = { -4.0f, 0.0f, 0.0f };
	m_ThirdCubePosition = { 5.0f, 0.0f, 0.0f };

	ImGui::SetNextWindowSize({ 500.f, 500.f });
}

void ImGuiMenu::Update()
{

}

void ImGuiMenu::Render()
{
	// Start the Dear ImGui frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	{
		ImGui::Begin("Menu");

		if (ImGui::CollapsingHeader("Camera Setting"))
		{
			ImGui::SliderFloat4("Camera Position", m_CameraPos, -50.f, 50.f);
			ImGui::SliderFloat("Camera Fov", &m_CameraFov, 0.01f, 180.f);
			ImGui::SliderFloat2("Camera Near&Far", m_CameraNearFar, 0.1f, 100.f);
		}
		
		if (ImGui::CollapsingHeader("Cube Position Setting"))
		{
			static float firstcube[3] = {};
			static float secondcube[3] = { -4.0f, 0.0f, 0.0f };
			static float thirdcube[3] = { 5.0f, 0.0f, 0.0f };
			ImGui::SliderFloat3("First Cube Position", firstcube, -20.f, 20.f);
			ImGui::SliderFloat3("Second Cube Position", secondcube, -20.f, 20.f);
			ImGui::SliderFloat3("Third Cube Position", thirdcube, -20.f, 20.f);

			m_FirstCubePosition.x = firstcube[0];
			m_FirstCubePosition.y = firstcube[1];
			m_FirstCubePosition.z = firstcube[2];
			m_SecondCubePosition.x = secondcube[0];
			m_SecondCubePosition.y = secondcube[1];
			m_SecondCubePosition.z = secondcube[2];
			m_ThirdCubePosition.x = thirdcube[0];
			m_ThirdCubePosition.y = thirdcube[1];
			m_ThirdCubePosition.z = thirdcube[2];
		}

		ImGui::End();
	}

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void ImGuiMenu::Release()
{

}
