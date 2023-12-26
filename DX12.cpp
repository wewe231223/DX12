// DX12.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//
#include "pch.h"
#include "DX12.h"
#include "Engine.h"
std::unique_ptr<Engine> engine{ nullptr };
#define PAUSE system("pause");

void ProgramClose(){
    engine->Close();
}


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow){
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    std::cout << "Hello World" << std::endl;
    engine = std::make_unique<Engine>();
    try {
        engine->Initialize(hInstance);
        engine->Loop();
        engine->Terminate();
    }
    catch (DxException& e) {
        MessageBox(nullptr, e.ToString().c_str(), L"Failed!", MB_OK);
    }
    PAUSE;
    return 0;
}

