#include "pch.h"
#include "Engine.h"

Engine::~Engine()
{
}

bool Engine::Initialize(HINSTANCE Instance, HWND MainWindowHandle) {
	m_hInstance = Instance;
	m_hWnd = MainWindowHandle;


	CreateDirect3DDevice();


	

	return false;
}

void Engine::Terminate(){
}

void Engine::CreateSwapChain(){
}

void Engine::CreateDirect3DDevice(){
#if defined(_DEBUG)
	D3D12GetDebugInterface(__uuidof(ID3D12Debug), (void**)&m_pd3dDebugController);
	m_pd3dDebugController->EnableDebugLayer();
#endif // !defined(_DEBUG)

	// DirectX Graphics Interface Factory(DXGIFACTORY) 를 생성한다 
	::CreateDXGIFactory1(__uuidof(IDXGIFactory4), (void**)&m_pdxgiFactory);
	IDXGIAdapter1* pD3dAdapter = NULL;


	// 모든 하드웨어 어댑터에 대해 D3D 특성 레벨 12.0 을 만족하는 하드웨어 디바이스를 생성한다 ( 아마도 그래픽카드일때 for 문이 끝나겠지? 그래픽 연산을 지원하는 디바이스를 만나면 반복문이 종료된다 ) 
	for (UINT i = 0; m_pdxgiFactory->EnumAdapters1(i, &pD3dAdapter) != DXGI_ERROR_NOT_FOUND; ++i) {
		DXGI_ADAPTER_DESC1 DxgiAdapterDesc{};
		pD3dAdapter->GetDesc1(&DxgiAdapterDesc);

		if (DxgiAdapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) continue;
		if (SUCCEEDED(D3D12CreateDevice(pD3dAdapter, D3D_FEATURE_LEVEL_12_0, __uuidof(ID3D12Device), (void**)&m_pd3dDevice))) break;
	}

	// 만약 특성 레벨 12.0 을 지원하는 하드웨어 디바이스를 생성 할 수 없다면 Warp 디바이스를 생성한다 
	// WARP 란 Windows Advanced Rasterization Platform 이며, 소프트웨어 래스터라이저이다
	// 즉 말 그대로 소프트웨어 그래픽카드( Cpu로 Gpu의 연산을 하겠다 라는 의미 ) 
	if (!pD3dAdapter) {
		m_pdxgiFactory->EnumWarpAdapter(__uuidof(IDXGIFactory), (void**)&pD3dAdapter);
		D3D12CreateDevice(pD3dAdapter, D3D_FEATURE_LEVEL_12_0, __uuidof(ID3D12Device), (void**)&m_pd3dDevice);
	}

	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS D3dMsaaQualityLevels{};

	// D3dMsaa4x를 지원하는지 확인하기 위한 구조체 작성 
	D3dMsaaQualityLevels.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	D3dMsaaQualityLevels.SampleCount = 4; // MSAA4x 샘플링 
	D3dMsaaQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	D3dMsaaQualityLevels.NumQualityLevels = 0;
	
	// 디바이스가 지정한 샘플링 수준을 만족하는지 확인 
	m_pd3dDevice->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &D3dMsaaQualityLevels, sizeof(D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS));

	// 확인한 값에 따라 멤버변수에 값을 작성 
	m_nMsaa4xQualityLevels = D3dMsaaQualityLevels.NumQualityLevels;
	m_bMsaa4xEnable = (m_nMsaa4xQualityLevels > 1) ? true : false;

	
	// Fence를 생성하고 값을 0으로 초기화한다 
	m_pd3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), (void**)&m_pd3dFence);
	// Fence의 동기화를 위한 이벤트 핸들러를 False 로 초기화한다 - 당연히 지금 시점은 그래픽 처리를 하지 않고 있으므로, False 로 초기화 해야 한다
	// 그래픽 카드가 연산을 하고 있을 때에만 Signal(True) 여야 하고, 나머지 구간에서는 모두 False 여야 한다( 그래야 Cpu 연산 중인 것으로 간주되니까 ) 
	m_hFenceEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);


	// 사용이 끝났으니 해제 
	if (pD3dAdapter) pD3dAdapter->Release();

}

void Engine::CreateRenderTargetAndDepthStencilDescriptorHeaps(){
}

void Engine::CreateCommandQueueAndList(){

}

void Engine::CreateRenderTargetView(){
}

void Engine::CreateDepthStencilView(){
}

void Engine::WaitForGpuComplete(){
}

void Engine::ChangeSwapChainState(){
}

void Engine::OnProcessMouseMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
}

void Engine::OnProcessKeyboardMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
}

LRESULT Engine::OnProcessWindowMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
	return LRESULT();
}

