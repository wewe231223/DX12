#include "pch.h"
#include "Engine.h"
#include "Timer.h"
#include "Input.h"

#pragma warning(disable:6387)
#define _WITH_SWAPCHAIN_FULLSCREEN_STATE // 전체화면 

static HWND hWnd{};



Engine::Engine(){
	for (int i = 0; i < m_nSwapChainBuffersNumber; i++) {
		m_pd3dRenderTargetBuffers[i] = { nullptr };
	}

	_tcscpy_s(m_pszFrameRate, _T("Project ( "));

}

Engine::~Engine(){
}

bool Engine::Initialize(HINSTANCE Instance,int Cmd) {

	WCHAR Windowclass[100]{};
	WCHAR WindowTitle[100]{};
	LoadStringW(m_hInstance, IDC_DX12, Windowclass, 100);
	LoadStringW(m_hInstance, IDS_APP_TITLE, WindowTitle, 100);

	m_hInstance = Instance;

	WNDCLASSEXW wcex{};
	::ZeroMemory(&wcex, sizeof(WNDCLASSEXW));

	wcex.cbSize = sizeof(WNDCLASSEXW);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = Engine::Proc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = m_hInstance;
	wcex.hIcon = LoadIcon(m_hInstance, MAKEINTRESOURCE(IDI_DX12));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = Windowclass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));



	RegisterClassExW(&wcex);

	hWnd = CreateWindowW(Windowclass, WindowTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, Instance, nullptr);
	

	if (!hWnd) {
		exit(EXIT_FAILURE);
	}


	::ShowWindow(hWnd, Cmd);
	::UpdateWindow(hWnd);



	CreateDirect3DDevice();
	CreateCommandQueueAndList();
	CreateSwapChain();
	CreateRenderTargetAndDepthStencilDescriptorHeaps();
	CreateRenderTargetView();
	CreateDepthStencilView();

	m_timer = std::make_unique<Timer>();
	INPUT->Init(hWnd,m_hInstance);


	return false;
}

void Engine::Terminate(){
	::CloseHandle(m_hFenceEvent);
#if defined(_DEBUG)
	if (m_pd3dDebugController) m_pd3dDebugController->Release();
#endif
	for (int i = 0; i < m_nSwapChainBuffersNumber; i++)
		if (m_pd3dRenderTargetBuffers[i])
			m_pd3dRenderTargetBuffers[i]->Release();

	if (m_pd3dDepthStencilBuffer)	m_pd3dDepthStencilBuffer->Release();
	if (m_pd3dRtvDescriptiorHeap)	m_pd3dRtvDescriptiorHeap->Release();
	if (m_pd3dDsvDescriptorHeap)	m_pd3dDsvDescriptorHeap->Release();
	if (m_pd3dCommandAllocator)		m_pd3dCommandAllocator->Release();
	if (m_pd3dCommandQueue)			m_pd3dCommandQueue->Release();
	if (m_pd3dCommandList)			m_pd3dCommandList->Release();
	if (m_pd3dFence)				m_pd3dFence->Release();

	m_pdxgiSwapChain->SetFullscreenState(FALSE, NULL);

	//TODO: 예외처리 - 삭제된게 또 삭제 되는것 같다.
	if (m_pdxgiSwapChain)	m_pdxgiSwapChain->Release();
	if (m_pdxgiFactory)		m_pdxgiFactory->Release();
	if (m_pd3dDevice)		m_pd3dDevice->Release();
	
}

void Engine::Loop(){
	HACCEL hAccelTable = LoadAccelerators(m_hInstance, MAKEINTRESOURCE(IDC_DX12));
	MSG message{};


	while (true) {

		if (::PeekMessage(&message, NULL, 0, 0, PM_REMOVE)){
			if (message.message == WM_QUIT) break;
			if (!::TranslateAccelerator(message.hwnd, hAccelTable, &message)){
				::TranslateMessage(&message);
				::DispatchMessage(&message);
			}
		}
		else{
			// Game Loop 
			Update();


			Render();
		}


	}



}

void Engine::Render(){
	// 명령 할당자와 명령 리스트를 초기화한다 
	// => 이전 프레임의 그리기 명령과 그 명령 할당자를 초기화 해야 한다 
	HRESULT hResult = m_pd3dCommandAllocator->Reset();
	hResult = m_pd3dCommandList->Reset(m_pd3dCommandAllocator, NULL);

	D3D12_RESOURCE_BARRIER D3dResourceBarrier;
	::ZeroMemory(&D3dResourceBarrier, sizeof(D3D12_RESOURCE_BARRIER));

	D3dResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	D3dResourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	D3dResourceBarrier.Transition.pResource = m_pd3dRenderTargetBuffers[m_nSwapChainBufferIndex];
	D3dResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	D3dResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	D3dResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	
	// 현재 렌더 타겟에 대한 Present 가 끝나기를 기다린다. 
	// Present 가 끝난 렌더 타켓 버퍼의 상태는 Present 상태,
	// 이 함수를 통해 렌더 타겟의 상태를 Present에서 State_Render_Target으로 전환한다
	// 위 구조체 초기화의 StateBefore 과 StateAfter 를 참고하면 State 의 흐름에 대해 알 수 있다 
	// State_Render_Target 으로 State 를 전환한다는 이제 렌더 타겟에 CPU 가 Write 하겠다는 의미이다.
	// 즉 이 상태 동안은 GPU는 렌더 타겟에 Write 할 권한을 잃는다 
	m_pd3dCommandList->ResourceBarrier(1, &D3dResourceBarrier);


	m_pd3dCommandList->RSSetViewports(1, &m_d3dViewPort);
	m_pd3dCommandList->RSSetScissorRects(1, &m_d3dSissorRect);


	// 현재 렌더 타겟에 해당하는 서술자의 CPU 핸들러를 계산한다 
	D3D12_CPU_DESCRIPTOR_HANDLE D3dRtvCPUDescriptorHandle = m_pd3dRtvDescriptiorHeap->GetCPUDescriptorHandleForHeapStart();
	D3dRtvCPUDescriptorHandle.ptr += (m_nSwapChainBufferIndex * m_nRtvDescriptorIncrementSize);

	// 현재 깊이 스텐실 서술자의 CPU 핸들러를 계산한다 
	D3D12_CPU_DESCRIPTOR_HANDLE D3dDsvCPUDescriptorHandle = m_pd3dDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();


	// 렌더 타켓 뷰를 지정한 색으로 클리어한다 
	float pfClearColor[4] = { 0.f,0.f,0.f,1.f };
	m_pd3dCommandList->ClearRenderTargetView(D3dRtvCPUDescriptorHandle, pfClearColor, 0, NULL);

	// 깊이 스텐실 뷰도 클리어 한다 
	m_pd3dCommandList->ClearDepthStencilView(D3dDsvCPUDescriptorHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);

	// 렌더 타겟 뷰(서술자)와 깊이 스텐실 뷰(서술자) 를 출력-병합 단계에 연결한다 
	m_pd3dCommandList->OMSetRenderTargets(1, &D3dRtvCPUDescriptorHandle, TRUE, &D3dDsvCPUDescriptorHandle);


	/*
	* 렌더링 코드는 여기에 추가됨 
	*/


	// 이제 렌더 타겟에 원하는 정보를 모두 썻으니, 다시 GPU에게 Write 권한을 넘겨줘야 한다
	// 아래 구조체 초기화의 StateBefore 과 StateAfter 를 참고하면 State 의 흐름에 대해 알 수 있다 
	// 이제 렌더 타켓의 상태는 Render_target 에서 Present(표현 상태) 로 바뀔 것이다
	D3dResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	D3dResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	D3dResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	m_pd3dCommandList->ResourceBarrier(1, &D3dResourceBarrier);


	// 명령 리스트도 닫아준다 
	hResult = m_pd3dCommandList->Close();

	// 명령 리스트를 이제 명령 큐에 한번에 넘겨준다 
	ID3D12CommandList* pD3dCommandLists[] = { m_pd3dCommandList };
	m_pd3dCommandQueue->ExecuteCommandLists(1, pD3dCommandLists);

	// 이제 넘겨준 명령을 GPU가 모두 수행해 CPU의 Fence Value 가 GPU의 Fence Value 와 같아질때까지 기다린다 
	WaitForGpuComplete();
	

	// 이제 SwapChain 을 Present한다. 렌더 타겟(후면 버퍼)의 내용이 전면 버퍼로 이동하고, 렌더 타겟 인덱스가 바뀔 것이다.
	m_pdxgiSwapChain->Present(0, 0);

	// Discussion : 혹시 다른 의견이나 틀린점이 있다면 추가바람 
	// 
	// 이 함수가 하는 일은 사실 WaitforGPUComplete 와 비슷하다.
	// 그 이유는 현재 시스템은 SwapChain 을 2개 즉 후면버퍼가 두개로 3중 버퍼링을 수행중이기 때문에
	// 한번의 프레임 렌더링에 두번의 Buffer를 기다려야한다 
	// 참고자료 : https://www.3dgep.com/wp-content/uploads/2017/11/GPU-Synchronization.png
	// 쉽게 생각해서 한번의 렌더링 과정 중에 모든 swapchain이 처리되어야 하므로,
	// 두번 기다려주는것 


	MovetoNextFrame();

	m_timer->GetFrameRate(m_pszFrameRate + 10, 35);
	::SetWindowText(hWnd, m_pszFrameRate);

	m_nSwapChainBufferIndex = m_pdxgiSwapChain->GetCurrentBackBufferIndex();
}

void Engine::Update(){

	m_timer->Tick(0.f);

	INPUT->Update();



}

void Engine::LateUpdate(){
}

LRESULT __stdcall Engine::Proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message){
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// 메뉴 선택을 구문 분석합니다:
		switch (wmId)
		{
		case IDM_ABOUT:
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void Engine::CreateSwapChain(){
	RECT ClientRect{};
	::GetClientRect(hWnd, &ClientRect);


	m_nWindowClientWidth = ClientRect.right - ClientRect.left;
	// 윈도우 좌표계 기준이므로, bottom > top 이다 
	m_nWindowClientHeight = ClientRect.bottom - ClientRect.top;

	// 구조체 초기화 
	DXGI_SWAP_CHAIN_DESC DxgiSwapChainDesc{};
	::ZeroMemory(&DxgiSwapChainDesc, sizeof(DxgiSwapChainDesc));
	// 구조체 데이터 입력 
	DxgiSwapChainDesc.BufferCount							= m_nSwapChainBuffersNumber;
	DxgiSwapChainDesc.BufferDesc.Width						= m_nWindowClientWidth;
	DxgiSwapChainDesc.BufferDesc.Height						= m_nWindowClientHeight;
	DxgiSwapChainDesc.BufferDesc.Format						= DXGI_FORMAT_R8G8B8A8_UNORM;
	DxgiSwapChainDesc.BufferUsage							= DXGI_USAGE_RENDER_TARGET_OUTPUT;
	DxgiSwapChainDesc.SwapEffect							= DXGI_SWAP_EFFECT_FLIP_DISCARD;
	DxgiSwapChainDesc.OutputWindow							= hWnd;
	DxgiSwapChainDesc.SampleDesc.Count						= (m_bMsaa4xEnable) ? 4 : 1;
	DxgiSwapChainDesc.SampleDesc.Quality					= (m_bMsaa4xEnable) ? m_nMsaa4xQualityLevels - 1 : 0;
	DxgiSwapChainDesc.Windowed								= true;
	DxgiSwapChainDesc.BufferDesc.RefreshRate.Numerator		= 0;
	DxgiSwapChainDesc.BufferDesc.RefreshRate.Denominator	= 0;
	DxgiSwapChainDesc.Flags									= DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	// SwapChain 생성 
	HRESULT hResult = m_pdxgiFactory->CreateSwapChain(m_pd3dCommandQueue, &DxgiSwapChainDesc, (IDXGISwapChain**)&m_pdxgiSwapChain);
	m_nSwapChainBufferIndex = m_pdxgiSwapChain->GetCurrentBackBufferIndex();
	// ALT+ENTER 가 작동하지 않도록 변경 
	hResult = m_pdxgiFactory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER);

#ifndef _WITH_SWAPCHAIN_FULLSCREEN_STATE
	CreateRenderTargetView();
#endif // !_WITH_SWAPCHAIN_FULLSCREEN_STATE

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

	// 뷰포트 설정 
	m_d3dViewPort.TopLeftX = 0.f;
	m_d3dViewPort.TopLeftY = 0.f;
	m_d3dViewPort.Width = static_cast<float>(m_nWindowClientWidth);
	m_d3dViewPort.Height = static_cast<float>(m_nWindowClientHeight);
	m_d3dViewPort.MinDepth = 0.f;
	m_d3dViewPort.MaxDepth = 1.f;


	// Sissor Rect 설정 
	m_d3dSissorRect = { 0,0,m_nWindowClientWidth,m_nWindowClientHeight };


	// 사용이 끝났으니 해제 
	if (pD3dAdapter) pD3dAdapter->Release();

}

void Engine::CreateRenderTargetAndDepthStencilDescriptorHeaps(){
	// 구조체 초기화 
	D3D12_DESCRIPTOR_HEAP_DESC D3dDescriptorHeapDesc{};
	::ZeroMemory(&D3dDescriptorHeapDesc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
	// 구조체 데이터 입력 
	D3dDescriptorHeapDesc.NumDescriptors = m_nSwapChainBuffersNumber; // 서술자의 개수는 SwapChain 버퍼의 개수이다 
	D3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	D3dDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	D3dDescriptorHeapDesc.NodeMask = 0;

	// RenderTarget 서술자 힙 생성 
	HRESULT hResult = m_pd3dDevice->CreateDescriptorHeap(&D3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)&m_pd3dRtvDescriptiorHeap);
	// 서술자 힙의 원소 개수를 가져온다 
	m_nRtvDescriptorIncrementSize = m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);


	// 이제 깊이 스텐실 서술자의 힙을 생성하는 구조체를 작성( 기존 구조체 재활용 ) 
	D3dDescriptorHeapDesc.NumDescriptors = 1; // 깊이 스텐실 버퍼의 서술자 개수는 1개이다 
	D3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	
	// 깊이 스텐실 서술자 힙 생성 
	hResult = m_pd3dDevice->CreateDescriptorHeap(&D3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)&m_pd3dDsvDescriptorHeap);
	
	m_nDsvDescriptorIncrementSize = m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);	


}

void Engine::CreateCommandQueueAndList(){
	D3D12_COMMAND_QUEUE_DESC D3dCommandQueueDesc{};
	::ZeroMemory(&D3dCommandQueueDesc, sizeof(D3D12_COMMAND_QUEUE_DESC));
	// 커맨드 큐를 생성하기 위한 구조체 작성( 명령을  Direct ( 직접 ) 전달 )  
	D3dCommandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	D3dCommandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	// 커맨드 큐를 작성 
	HRESULT hResult = m_pd3dDevice->CreateCommandQueue(&D3dCommandQueueDesc, __uuidof(ID3D12CommandQueue), (void**)&m_pd3dCommandQueue);

	// 커맨드 명령 하달자 생성 ( 명령을 직접 전달 )
	hResult = m_pd3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, __uuidof(ID3D12CommandAllocator), (void**)&m_pd3dCommandAllocator);
	
	// 커맨드 리스트 생성 ( 명령을 직접 전달 )
	hResult = m_pd3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_pd3dCommandAllocator, NULL, __uuidof(ID3D12GraphicsCommandList), (void**)&m_pd3dCommandList);
	

	// 커맨드 리스트는 생성 시 열린 상태이므로( Open ) 닫아주어야 한다.  
	hResult = m_pd3dCommandList->Close();
}

void Engine::CreateRenderTargetView(){
	D3D12_CPU_DESCRIPTOR_HANDLE D3dRtvCPUDescriptorHandle = m_pd3dRtvDescriptiorHeap->GetCPUDescriptorHandleForHeapStart();

	for (UINT i = 0; i < m_nSwapChainBuffersNumber; ++i) {
		HRESULT hResult = m_pdxgiSwapChain->GetBuffer(i, __uuidof(ID3D12Resource), (void**)&m_pd3dRenderTargetBuffers[i]);
		m_pd3dDevice->CreateRenderTargetView(m_pd3dRenderTargetBuffers[i], NULL, D3dRtvCPUDescriptorHandle);
		D3dRtvCPUDescriptorHandle.ptr += m_nRtvDescriptorIncrementSize;
	}
}

void Engine::CreateDepthStencilView(){

	D3D12_RESOURCE_DESC D3dResourcesDesc;
	::ZeroMemory(&D3dResourcesDesc, sizeof(D3D12_RESOURCE_DESC));

	D3dResourcesDesc.Dimension						= D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	D3dResourcesDesc.Alignment						= 0;
	D3dResourcesDesc.Width							= m_nWindowClientWidth;
	D3dResourcesDesc.Height							= m_nWindowClientHeight;
	D3dResourcesDesc.DepthOrArraySize				= 1;
	D3dResourcesDesc.MipLevels						= 1;
	D3dResourcesDesc.Format							= DXGI_FORMAT_D24_UNORM_S8_UINT;
	D3dResourcesDesc.SampleDesc.Count				= (m_bMsaa4xEnable) ? 4 : 1;
	D3dResourcesDesc.SampleDesc.Quality				= (m_bMsaa4xEnable) ? (m_nMsaa4xQualityLevels - 1) : 0;
	D3dResourcesDesc.Layout							= D3D12_TEXTURE_LAYOUT_UNKNOWN;
	D3dResourcesDesc.Flags							= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;


	D3D12_HEAP_PROPERTIES D3dHeapProperties;
	::ZeroMemory(&D3dHeapProperties, sizeof(D3D12_HEAP_PROPERTIES));

	D3dHeapProperties.Type							= D3D12_HEAP_TYPE_DEFAULT;
	D3dHeapProperties.CPUPageProperty				= D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	D3dHeapProperties.MemoryPoolPreference			= D3D12_MEMORY_POOL_UNKNOWN;
	D3dHeapProperties.CreationNodeMask				= 1;
	D3dHeapProperties.VisibleNodeMask				= 1;

	D3D12_CLEAR_VALUE D3dClearValue;

	D3dClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	D3dClearValue.DepthStencil.Depth = 1.0f;
	D3dClearValue.DepthStencil.Stencil = 0;

	// 깊이-스텐실 버퍼 생성 
	m_pd3dDevice->CreateCommittedResource(
		&D3dHeapProperties, D3D12_HEAP_FLAG_NONE, 
		&D3dResourcesDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, 
		&D3dClearValue, __uuidof(ID3D12Resource), (void**)&m_pd3dDepthStencilBuffer);

	// 깊이 스텐실 뷰 생서
	D3D12_CPU_DESCRIPTOR_HANDLE D3dDsvCPUDescriptorHandle = m_pd3dDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_pd3dDevice->CreateDepthStencilView(m_pd3dDepthStencilBuffer, NULL, D3dDsvCPUDescriptorHandle);
}

void Engine::WaitForGpuComplete(){
	// CPU Fence 의 값을 증가시킨다
	// Gpu가 Fence의 값을 설정하는 명령을 큐에 추가한다. 
	UINT64 nFence = ++m_nFenceValue[m_nSwapChainBufferIndex];
	HRESULT hResult = m_pd3dCommandQueue->Signal(m_pd3dFence, nFence);

	if (m_pd3dFence->GetCompletedValue() < nFence) {
		//펜스의 현재 값이 CPU Fence 값 보다 작으면 펜스의 현재 값이 CPU Fence 값이 될 때까지 기다린다.
		hResult = m_pd3dFence->SetEventOnCompletion(nFence, m_hFenceEvent);
		::WaitForSingleObject(m_hFenceEvent, INFINITE);
	}

}




void Engine::MovetoNextFrame(){
	m_nSwapChainBufferIndex = m_pdxgiSwapChain->GetCurrentBackBufferIndex();

	UINT64 FenceValue = ++m_nFenceValue[m_nSwapChainBufferIndex];
	HRESULT hResult = m_pd3dCommandQueue->Signal(m_pd3dFence, FenceValue);

		
	if (m_pd3dFence->GetCompletedValue() < FenceValue) {
		hResult = m_pd3dFence->SetEventOnCompletion(FenceValue, m_hFenceEvent);
		::WaitForSingleObject(m_hFenceEvent, INFINITE);
	}

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

