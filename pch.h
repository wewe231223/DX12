// header.h: 표준 시스템 포함 파일
// 또는 프로젝트 특정 포함 파일이 들어 있는 포함 파일입니다.
//

#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.
#define DIRECTINPUT_VERSION 0x0800		// Direct Input Version 을 8.0 으로 명시합니다.


// Windows 헤더 파일
#include <windows.h>
// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include <wrl.h>
#include <shellapi.h>


// C++ Header 
#include <map>
#include <vector>
#include <string>
#include <memory>
#include <iostream>
#include <array>
#include <cassert>

// 전역 헤더 
#include "resource.h"
#include "Math.h"
#include "DX12.h"






// DirectX 헤더
#include <d3d12.h>
#include <dxgi1_4.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>
#include <dinput.h>


using Microsoft::WRL::ComPtr;

#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")


// Create Console Log 
#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")

#define FRAMEBUFFER_WIDTH 1200
#define FRAMEBUFFER_HEIGHT 800
