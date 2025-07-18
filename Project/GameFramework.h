#pragma once

#define FRAME_BUFFER_WIDTH 1920
#define FRAME_BUFFER_HEIGHT 1080

#include "Timer.h"
#include "Player.h"
#include "Scene.h"

class CGameFramework
{
public:
	CGameFramework();
	~CGameFramework();

	bool OnCreate(HINSTANCE hInstance, HWND hMainWnd);
	void OnDestroy();

	void CreateSwapChain();
	void CreateDirect3DDevice();
	void CreateCommandQueueAndList();

	void CreateRtvAndDsvDescriptorHeaps();

	void CreateRenderTargetViews();
	void CreateDepthStencilView();

	void ChangeSwapChainState();

	void BuildObjects();
	void ReleaseObjects();

	void ProcessInput();
	void AnimateObjects();
	void FrameAdvance();

	void WaitForGpuComplete();
	void MoveToNextFrame();

	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);


	void ItemToHand(int objectIndex);

	void OnOtherClientConnected()
	{
		m_ppScenes[m_nCurrentScene]->OnOtherClientConnedted();
	}
	void UpdateOtherPlayerPosition(int clinetnum, XMFLOAT3 position)
	{
		m_ppScenes[m_nCurrentScene]->UpdateOtherPlayerPosition(clinetnum, position);
	}
	void UpdateOtherPlayerAnimation(int clinetnum, int animNum)
	{
		m_ppScenes[m_nCurrentScene]->UpdateOtherPlayerAnimation(clinetnum, animNum);
	}

	//해볼려고 했음...
	void AddItemToScene(long long id, ITEM_TYPE type, const XMFLOAT3& position)
	{
		if (m_ppScenes && m_ppScenes[m_nCurrentScene]) {
			m_ppScenes[m_nCurrentScene]->AddItem(id, type, position);
		}
	}



	bool isLoading = false;
	bool isStartScene = true;

private:
	HINSTANCE m_hInstance;
	HWND m_hWnd;

	int m_nWndClientWidth;
	int m_nWndClientHeight;

	IDXGIFactory4* m_pdxgiFactory = NULL;
	IDXGISwapChain3* m_pdxgiSwapChain = NULL;
	ID3D12Device* m_pd3dDevice = NULL;

	bool m_bMsaa4xEnable = false;
	UINT m_nMsaa4xQualityLevels = 0;

	static const UINT m_nSwapChainBuffers = 2;
	UINT m_nSwapChainBufferIndex;

	ID3D12Resource* m_ppd3dSwapChainBackBuffers[m_nSwapChainBuffers];
	ID3D12DescriptorHeap* m_pd3dRtvDescriptorHeap = NULL;

	ID3D12Resource* m_pd3dDepthStencilBuffer = NULL;
	ID3D12DescriptorHeap* m_pd3dDsvDescriptorHeap = NULL;

	ID3D12CommandAllocator* m_pd3dCommandAllocator = NULL;
	ID3D12CommandQueue* m_pd3dCommandQueue = NULL;
	ID3D12GraphicsCommandList* m_pd3dCommandList = NULL;

	ID3D12Fence* m_pd3dFence = NULL;
	UINT64 m_nFenceValues[m_nSwapChainBuffers];
	HANDLE m_hFenceEvent;

#if defined(_DEBUG)
	ID3D12Debug* m_pd3dDebugController;
#endif

	CGameTimer m_GameTimer;


	CScene						*m_pScene = NULL;
	CPlayer						*m_pPlayer = NULL;
	CCamera						*m_pCamera = NULL;

	int m_nCurrentScene = 0;
	int m_nScene = 0;
	int m_nScenes = 0;
	CScene** m_ppScenes = NULL;

	POINT m_ptOldCursorPos;

	_TCHAR						m_pszFrameRate[70];

	bool flashlightToggle = false;

	//server
	//float m_fLastPositionSendTime = 0.0f;  // 위치 전송 시간 추적
};



