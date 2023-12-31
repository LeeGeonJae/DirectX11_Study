#pragma once

// STL
#include <vector>
#include <list>
#include <map>
#include <string>
#include <unordered_map>
#include <memory>
#include <random>
#include <time.h>
#include <queue>
using namespace std;

// WIN
#include <Windows.h>
#include <assert.h>

// assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// DX
#include <d3d11.h>
#include <d3dcompiler.h>
#include <wrl.h>
#include <DirectXMath.h>
#include <DirectXTex/DirectXTex.h>
#include <DirectXTex/DirectXTex.inl>
using namespace DirectX;
using namespace Microsoft::WRL;

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

#ifdef _DEBUG
#pragma comment(lib, "DirectXTex\\DirectXTex_debug.lib")
#else
#pragma comment(lib, "DirectXTex\\DirectXTex.lib")
#endif