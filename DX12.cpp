// DX12.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//
#include "pch.h"
#include "DX12.h"
#include "Engine.h"
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow){
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    std::cout << "Hello World" << std::endl;
    std::unique_ptr<Engine> engine = std::make_unique<Engine>();
    engine->Initialize(hInstance, nCmdShow);
    engine->Loop();
    engine->Terminate();
    return 0;
}


