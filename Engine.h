#pragma once
class Timer;
#include <atomic>

std::atomic<int> a;

class Engine {
private:
	HINSTANCE m_hInstance{};
	HWND m_hWnd{};

	int m_nWindowClientWidth{ FRAMEBUFFER_WIDTH };
	int m_nWindowClientHeight{ FRAMEBUFFER_HEIGHT };


	// DXGI == DirectX Graphics Interface 
	IDXGIFactory4* m_pdxgiFactory{ nullptr }; // DXGI 팩토리 포인터 
	IDXGISwapChain3* m_pdxgiSwapChain{ nullptr }; // DXGI 스왑 체인 포인터 
	ID3D12Device* m_pd3dDevice{ nullptr }; // DirectX 디바이스 포인터 


	bool m_bMsaa4xEnable{ false }; // 다중 샘플링 MSAA 활성화 ( Multi-Sampling-Anti-Aliasing ) 
	UINT m_nMsaa4xQualityLevels{ 0 }; // MSAA 레벨 
	

	static const UINT m_nSwapChainBuffersNumber{ 2 }; // 후면 버퍼 개수 
	UINT m_nSwapChainBufferIndex{ 0 }; // 현재 후면 버퍼 인덱스 



	// Resource <=> Descriptor 은 쌍으로 붙음 
	// Resource 는 버퍼의 개념 == > 프로그래밍 단계의 버퍼 
	// Descriptor 은 GPU에 전해주기 위한 버퍼를 데이터 블록으로 만든 것 => Resource 의 내용을 GPU에 서술한다( 서술자 ) 
	// 따라서 Buffer(Resource) 는 Resource 뿐만 아니라, Descriptor heap, 그리고 heap 의 크기(원소의 개수) 가 하나의 세트를 형성한다
	ID3D12Resource* m_pd3dRenderTargetBuffers[m_nSwapChainBuffersNumber]{};// 렌더 타켓 버퍼 
	// 렌더 타겟의 개수만큼 SwapChain buffer 생성 <==> SwapChain 의 개수만큼 RenderTarget 생성 
	// RTV == RenderTarget
	ID3D12DescriptorHeap* m_pd3dRtvDescriptiorHeap{ nullptr }; // Descriptor Heap 인터페이스 포인터 
	UINT m_nRtvDescriptorIncrementSize{ 0 }; // Render Target Descriptor 원소의 크기 

	ID3D12Resource* m_pd3dDepthStencilBuffer{ nullptr }; // 깊이 스텐실 포인터 - 깊이 버퍼 
	ID3D12DescriptorHeap* m_pd3dDsvDescriptorHeap{ nullptr }; // 깊이 스텐실 버퍼의 서술자 힙 인터페이스 포인터 
	UINT m_nDsvDescriptorIncrementSize{ 0 }; // 깊이 스텐실 버퍼 서술자 원소의 크기 

	ID3D12CommandQueue* m_pd3dCommandQueue{ nullptr }; // Command Queue
	ID3D12CommandAllocator* m_pd3dCommandAllocator{ nullptr }; // 커맨드 큐의 명령 할당자 
	ID3D12GraphicsCommandList* m_pd3dCommandList{ nullptr }; // 명령 리스트 인터페이스 포인터 

	// DirectX 의 Pipeline 은 상태기계(State Machine) 이다 
	// 따라서 State를 담을 변수가 필요함
	ID3D12PipelineState* m_pd3dPipelineState{ nullptr };

	// CPU와 GPU는 독자적으로 행동한다 
	// 이를 멀티코어 프로그래밍( 멀티쓰레드 ) 의 관점으로 보면
	// Lock(Mutex) 변수가 필요하다. Fence 가 그 역할.
	// Fence 를 생성하면 GPU가 n번째 프레임의 처리를 마치기 전에 CPU가 n+1번째 프레임의 처리를 하지 못하도록 막는다.
	ID3D12Fence* m_pd3dFence{ nullptr };
	UINT64 m_nFenceValue{ 0 }; // 펜스 값 ( 몇 번째 프레임을 처리중인지 저장 -> 2^64 의 크기이므로 overflow 될 일이 없다) 
	/*
	* 이벤트 핸들러를 이해하기 위해 커널 기반 동기화에 대해 알아야함 
	* 생산자-소비자 패턴을 생각해보자 
	* CPU가 버퍼를 생산하는 생산자라고 하고, GPU가 버퍼를 소비하는 소비자라고 하자 
	* 
	* 여기서 발생하는 문제는, 소비자가 버퍼를 가져가는 시점에서, 소비자는 생산자가 버퍼를 모두 가져다 놓았는지, 올바른 버퍼가 놓여 있는지 알 수 없다.
	* 
	* 따라서 소비자는 생산자가 모든 데이터를 생산하고, Handler 를 통해 Signaled 상태로 변경하기를 기다려야함 
	* 생산자는 모든 버퍼를 생성하고, 인터페이스에 올려둔 뒤, Handler를 통해 Signaled 신호를 보내면,
	* 소비자는 준비된 버퍼를 가져가 일을 시작한다.
	* 
	*/
	HANDLE m_hFenceEvent{}; // 펜스 이벤트 핸들러 

#if defined(_DEBUG)
	ID3D12Debug* m_pd3dDebugController{ nullptr };
#endif // !defined(_DEBUG)
	D3D12_VIEWPORT m_d3dViewPort{}; // 뷰포트

	// 래스터화 단계에서 이 시저 사각형 내부에 들어가지 않는 부분은 모두 삭제한다 
	// 따라서 대부분의 경우 뷰포트와 같은 크기로 정의한다 
	// CommandList 가 Clear 될 때마다 새로 정의해 주어야 한다 
	D3D12_RECT m_d3dSissorRect{}; 

	std::unique_ptr<Timer> m_timer{};



public:

	Engine();
	~Engine();

	// Create Engine( DX12 Device ) 
	bool Initialize(HINSTANCE Instance, HWND MainWindowHandle);
	void Terminate();


private:
	// Device Initialize 
	void CreateSwapChain();
	void CreateDirect3DDevice();
	void CreateRenderTargetAndDepthStencilDescriptorHeaps();
	void CreateCommandQueueAndList();
	void CreateRenderTargetView();
	void CreateDepthStencilView();

	// Get a sync with Gpu 
	void WaitForGpuComplete();
	
	void ChangeSwapChainState();


	// Process Window Message 
	void OnProcessMouseMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	void OnProcessKeyboardMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK OnProcessWindowMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

};

