#pragma once


class Engine {
	HINSTANCE m_hInstance{};
	HWND m_hWnd{};

	int m_hWndClientWidth{ FRAMEBUFFER_WIDTH };
	int m_hWndClientHeight{ FRAMEBUFFER_HEIGHT };


	IDXGIFactory4* m_pdxgiFactory{ nullptr };
	IDXGISwapChain3* m_pdxgiSwapChain{ nullptr };
	ID3D12Device* m_pd3dDevice{ nullptr };


	bool m_msaa4xEnable{ false };
	UINT m_msaa4xQualityLevels{ 0 };
	

	static const UINT m_swapChainBuffers{ 2 }; // 후면 버퍼 개수 
	UINT m_swapChainBufferIndex{ 0 }; // 현재 후면 버퍼 인덱스 




};

