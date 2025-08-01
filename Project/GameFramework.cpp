﻿//-----------------------------------------------------------------------------
// File: CGameFramework.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "GameFramework.h"
#include "Network.h"
#include "Hpbar.h"

CGameFramework::CGameFramework()
{
	m_pdxgiFactory = NULL;
	m_pdxgiSwapChain = NULL;
	m_pd3dDevice = NULL;

	for (int i = 0; i < m_nSwapChainBuffers; i++) m_ppd3dSwapChainBackBuffers[i] = NULL;
	m_nSwapChainBufferIndex = 0;

	m_pd3dCommandAllocator = NULL;
	m_pd3dCommandQueue = NULL;
	m_pd3dCommandList = NULL;

	m_pd3dRtvDescriptorHeap = NULL;
	m_pd3dDsvDescriptorHeap = NULL;

	m_hFenceEvent = NULL;
	m_pd3dFence = NULL;
	for (int i = 0; i < m_nSwapChainBuffers; i++) m_nFenceValues[i] = 0;

	m_nWndClientWidth = FRAME_BUFFER_WIDTH;
	m_nWndClientHeight = FRAME_BUFFER_HEIGHT;

	m_pScene = NULL;
	m_pPlayer = NULL;

	_tcscpy_s(m_pszFrameRate, _T("EVADO "));
}

CGameFramework::~CGameFramework()
{
}

bool CGameFramework::OnCreate(HINSTANCE hInstance, HWND hMainWnd)
{
	m_hInstance = hInstance;
	m_hWnd = hMainWnd;

	CreateDirect3DDevice();
	CreateCommandQueueAndList();
	CreateRtvAndDsvDescriptorHeaps();
	CreateSwapChain();
	CreateDepthStencilView();

	CoInitialize(NULL);

	BuildObjects();
	//LoadingDoneToServer();

	return(true);
}

void CGameFramework::CreateSwapChain()
{
	RECT rcClient;
	::GetClientRect(m_hWnd, &rcClient);
	m_nWndClientWidth = rcClient.right - rcClient.left;
	m_nWndClientHeight = rcClient.bottom - rcClient.top;

#ifdef _WITH_CREATE_SWAPCHAIN_FOR_HWND
	DXGI_SWAP_CHAIN_DESC1 dxgiSwapChainDesc;
	::ZeroMemory(&dxgiSwapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC1));
	dxgiSwapChainDesc.Width = m_nWndClientWidth;
	dxgiSwapChainDesc.Height = m_nWndClientHeight;
	dxgiSwapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dxgiSwapChainDesc.SampleDesc.Count = (m_bMsaa4xEnable) ? 4 : 1;
	dxgiSwapChainDesc.SampleDesc.Quality = (m_bMsaa4xEnable) ? (m_nMsaa4xQualityLevels - 1) : 0;
	dxgiSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	dxgiSwapChainDesc.BufferCount = m_nSwapChainBuffers;
	dxgiSwapChainDesc.Scaling = DXGI_SCALING_NONE;
	dxgiSwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	dxgiSwapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	dxgiSwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	DXGI_SWAP_CHAIN_FULLSCREEN_DESC dxgiSwapChainFullScreenDesc;
	::ZeroMemory(&dxgiSwapChainFullScreenDesc, sizeof(DXGI_SWAP_CHAIN_FULLSCREEN_DESC));
	dxgiSwapChainFullScreenDesc.RefreshRate.Numerator = 60;
	dxgiSwapChainFullScreenDesc.RefreshRate.Denominator = 1;
	dxgiSwapChainFullScreenDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	dxgiSwapChainFullScreenDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	dxgiSwapChainFullScreenDesc.Windowed = TRUE;

	HRESULT hResult = m_pdxgiFactory->CreateSwapChainForHwnd(m_pd3dCommandQueue, m_hWnd, &dxgiSwapChainDesc, &dxgiSwapChainFullScreenDesc, NULL, (IDXGISwapChain1 **)&m_pdxgiSwapChain);
#else
	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
	::ZeroMemory(&dxgiSwapChainDesc, sizeof(dxgiSwapChainDesc));
	dxgiSwapChainDesc.BufferCount = m_nSwapChainBuffers;
	dxgiSwapChainDesc.BufferDesc.Width = m_nWndClientWidth;
	dxgiSwapChainDesc.BufferDesc.Height = m_nWndClientHeight;
	dxgiSwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	dxgiSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	dxgiSwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	dxgiSwapChainDesc.OutputWindow = m_hWnd;
	dxgiSwapChainDesc.SampleDesc.Count = (m_bMsaa4xEnable) ? 4 : 1;
	dxgiSwapChainDesc.SampleDesc.Quality = (m_bMsaa4xEnable) ? (m_nMsaa4xQualityLevels - 1) : 0;
	dxgiSwapChainDesc.Windowed = TRUE;
	dxgiSwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	HRESULT hResult = m_pdxgiFactory->CreateSwapChain(m_pd3dCommandQueue, &dxgiSwapChainDesc, (IDXGISwapChain **)&m_pdxgiSwapChain);
#endif
	m_nSwapChainBufferIndex = m_pdxgiSwapChain->GetCurrentBackBufferIndex();

	hResult = m_pdxgiFactory->MakeWindowAssociation(m_hWnd, DXGI_MWA_NO_ALT_ENTER);

#ifndef _WITH_SWAPCHAIN_FULLSCREEN_STATE
	CreateRenderTargetViews();
#endif
}

void CGameFramework::CreateDirect3DDevice()
{
	HRESULT hResult;

	UINT nDXGIFactoryFlags = 0;
#if defined(_DEBUG)
	ID3D12Debug *pd3dDebugController = NULL;
	hResult = D3D12GetDebugInterface(__uuidof(ID3D12Debug), (void **)&pd3dDebugController);
	if (pd3dDebugController)
	{
		pd3dDebugController->EnableDebugLayer();
		pd3dDebugController->Release();
	}
	nDXGIFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
#endif

	hResult = ::CreateDXGIFactory2(nDXGIFactoryFlags, __uuidof(IDXGIFactory4), (void **)&m_pdxgiFactory);

	IDXGIAdapter1 *pd3dAdapter = NULL;

	for (UINT i = 0; DXGI_ERROR_NOT_FOUND != m_pdxgiFactory->EnumAdapters1(i, &pd3dAdapter); i++)
	{
		DXGI_ADAPTER_DESC1 dxgiAdapterDesc;
		pd3dAdapter->GetDesc1(&dxgiAdapterDesc);
		if (dxgiAdapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) continue;
		if (SUCCEEDED(D3D12CreateDevice(pd3dAdapter, D3D_FEATURE_LEVEL_12_0, _uuidof(ID3D12Device), (void **)&m_pd3dDevice))) break;
	}

	if (!pd3dAdapter)
	{
		m_pdxgiFactory->EnumWarpAdapter(_uuidof(IDXGIFactory4), (void **)&pd3dAdapter);
		hResult = D3D12CreateDevice(pd3dAdapter, D3D_FEATURE_LEVEL_12_0, _uuidof(ID3D12Device), (void **)&m_pd3dDevice);
	}

	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS d3dMsaaQualityLevels;
	d3dMsaaQualityLevels.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dMsaaQualityLevels.SampleCount = 4;
	d3dMsaaQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	d3dMsaaQualityLevels.NumQualityLevels = 0;
	hResult = m_pd3dDevice->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &d3dMsaaQualityLevels, sizeof(D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS));
	m_nMsaa4xQualityLevels = d3dMsaaQualityLevels.NumQualityLevels;
	m_bMsaa4xEnable = (m_nMsaa4xQualityLevels > 1) ? true : false;

	hResult = m_pd3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), (void **)&m_pd3dFence);
	for (UINT i = 0; i < m_nSwapChainBuffers; i++) m_nFenceValues[i] = 0;

	m_hFenceEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);

	::gnCbvSrvDescriptorIncrementSize = m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	::gnRtvDescriptorIncrementSize = m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	::gnDsvDescriptorIncrementSize = m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

	if (pd3dAdapter) pd3dAdapter->Release();
}

void CGameFramework::CreateCommandQueueAndList()
{
	HRESULT hResult;

	D3D12_COMMAND_QUEUE_DESC d3dCommandQueueDesc;
	::ZeroMemory(&d3dCommandQueueDesc, sizeof(D3D12_COMMAND_QUEUE_DESC));
	d3dCommandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	d3dCommandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	hResult = m_pd3dDevice->CreateCommandQueue(&d3dCommandQueueDesc, _uuidof(ID3D12CommandQueue), (void **)&m_pd3dCommandQueue);

	hResult = m_pd3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, __uuidof(ID3D12CommandAllocator), (void **)&m_pd3dCommandAllocator);

	hResult = m_pd3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_pd3dCommandAllocator, NULL, __uuidof(ID3D12GraphicsCommandList), (void **)&m_pd3dCommandList);
	hResult = m_pd3dCommandList->Close();
}

void CGameFramework::CreateRtvAndDsvDescriptorHeaps()
{
	D3D12_DESCRIPTOR_HEAP_DESC d3dDescriptorHeapDesc;
	::ZeroMemory(&d3dDescriptorHeapDesc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
	d3dDescriptorHeapDesc.NumDescriptors = m_nSwapChainBuffers;
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	d3dDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	d3dDescriptorHeapDesc.NodeMask = 0;
	HRESULT hResult = m_pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void **)&m_pd3dRtvDescriptorHeap);
	::gnRtvDescriptorIncrementSize = m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	d3dDescriptorHeapDesc.NumDescriptors = 1;
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	hResult = m_pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void **)&m_pd3dDsvDescriptorHeap);
	::gnDsvDescriptorIncrementSize = m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
}

void CGameFramework::CreateRenderTargetViews()
{
	D3D12_CPU_DESCRIPTOR_HANDLE d3dRtvCPUDescriptorHandle = m_pd3dRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	for (UINT i = 0; i < m_nSwapChainBuffers; i++)
	{
		m_pdxgiSwapChain->GetBuffer(i, __uuidof(ID3D12Resource), (void **)&m_ppd3dSwapChainBackBuffers[i]);
		m_pd3dDevice->CreateRenderTargetView(m_ppd3dSwapChainBackBuffers[i], NULL, d3dRtvCPUDescriptorHandle);
		d3dRtvCPUDescriptorHandle.ptr += ::gnRtvDescriptorIncrementSize;
	}
}

void CGameFramework::CreateDepthStencilView()
{
	D3D12_RESOURCE_DESC d3dResourceDesc;
	d3dResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	d3dResourceDesc.Alignment = 0;
	d3dResourceDesc.Width = m_nWndClientWidth;
	d3dResourceDesc.Height = m_nWndClientHeight;
	d3dResourceDesc.DepthOrArraySize = 1;
	d3dResourceDesc.MipLevels = 1;
	d3dResourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dResourceDesc.SampleDesc.Count = (m_bMsaa4xEnable) ? 4 : 1;
	d3dResourceDesc.SampleDesc.Quality = (m_bMsaa4xEnable) ? (m_nMsaa4xQualityLevels - 1) : 0;
	d3dResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	d3dResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_HEAP_PROPERTIES d3dHeapProperties;
	::ZeroMemory(&d3dHeapProperties, sizeof(D3D12_HEAP_PROPERTIES));
	d3dHeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
	d3dHeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	d3dHeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	d3dHeapProperties.CreationNodeMask = 1;
	d3dHeapProperties.VisibleNodeMask = 1;

	D3D12_CLEAR_VALUE d3dClearValue;
	d3dClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dClearValue.DepthStencil.Depth = 1.0f;
	d3dClearValue.DepthStencil.Stencil = 0;

	m_pd3dDevice->CreateCommittedResource(&d3dHeapProperties, D3D12_HEAP_FLAG_NONE, &d3dResourceDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &d3dClearValue, __uuidof(ID3D12Resource), (void **)&m_pd3dDepthStencilBuffer);

	D3D12_DEPTH_STENCIL_VIEW_DESC d3dDepthStencilViewDesc;
	::ZeroMemory(&d3dDepthStencilViewDesc, sizeof(D3D12_DEPTH_STENCIL_VIEW_DESC));
	d3dDepthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dDepthStencilViewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	d3dDepthStencilViewDesc.Flags = D3D12_DSV_FLAG_NONE;

	D3D12_CPU_DESCRIPTOR_HANDLE d3dDsvCPUDescriptorHandle = m_pd3dDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_pd3dDevice->CreateDepthStencilView(m_pd3dDepthStencilBuffer, &d3dDepthStencilViewDesc, d3dDsvCPUDescriptorHandle);
}

void CGameFramework::ChangeSwapChainState()
{
	WaitForGpuComplete();

	BOOL bFullScreenState = FALSE;
	m_pdxgiSwapChain->GetFullscreenState(&bFullScreenState, NULL);
	m_pdxgiSwapChain->SetFullscreenState(!bFullScreenState, NULL);

	DXGI_MODE_DESC dxgiTargetParameters;
	dxgiTargetParameters.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dxgiTargetParameters.Width = m_nWndClientWidth;
	dxgiTargetParameters.Height = m_nWndClientHeight;
	dxgiTargetParameters.RefreshRate.Numerator = 60;
	dxgiTargetParameters.RefreshRate.Denominator = 1;
	dxgiTargetParameters.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	dxgiTargetParameters.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	m_pdxgiSwapChain->ResizeTarget(&dxgiTargetParameters);

	for (int i = 0; i < m_nSwapChainBuffers; i++) if (m_ppd3dSwapChainBackBuffers[i]) m_ppd3dSwapChainBackBuffers[i]->Release();

	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
	m_pdxgiSwapChain->GetDesc(&dxgiSwapChainDesc);
	m_pdxgiSwapChain->ResizeBuffers(m_nSwapChainBuffers, m_nWndClientWidth, m_nWndClientHeight, dxgiSwapChainDesc.BufferDesc.Format, dxgiSwapChainDesc.Flags);

	m_nSwapChainBufferIndex = m_pdxgiSwapChain->GetCurrentBackBufferIndex();

	CreateRenderTargetViews();
}

void CGameFramework::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	if (m_pScene) m_pScene->OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);

	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
		::SetCapture(hWnd);
		::GetCursorPos(&m_ptOldCursorPos);
		break;
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		::ReleaseCapture();
		break;
	case WM_MOUSEMOVE:
		//::SetCapture(hWnd);
		//::GetCursorPos(&m_ptOldCursorPos);
		break;
	default:
		break;
	}

}

void CGameFramework::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	if (m_pScene) m_pScene->OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
	if (!isStartScene) {
		switch (nMessageID)
		{
		case WM_KEYUP:
			switch (wParam)
			{
			case VK_ESCAPE:
				exit(0);
				break;
			case VK_F1:
			case VK_F2:
			case VK_F3:
				m_pCamera = m_pPlayer->ChangeCamera((DWORD)(wParam - VK_F1 + 1), m_GameTimer.GetTimeElapsed());
				break;
			case VK_F9:
				ChangeSwapChainState();
				break;
			}
			break;
		default:
			break;
		}
	}
}

LRESULT CALLBACK CGameFramework::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
		case WM_ACTIVATE:
		{
			if (LOWORD(wParam) == WA_INACTIVE)
				m_GameTimer.Stop();
			else
				m_GameTimer.Start();
			break;
		}
		case WM_SIZE:
			break;
		case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_RBUTTONUP:
        case WM_MOUSEMOVE:
			OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
            break;
        case WM_KEYDOWN:
        case WM_KEYUP:
			OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
			break;
	}
	return(0);
}

// server 추가해봄....  경민.ver --------------------------------------------------------------

//long long CGameFramework::FindNearestItemInRange(float range, XMFLOAT3 playerPos) {
//	std::lock_guard<std::mutex> lock(g_item_mutex);
//	long long nearestId = -1;
//	float nearestDistSqr = range * range;
//
//	for (const auto& pair : g_items) {
//		Item* pItem = pair.second;
//		if (!pItem) continue;
//
//		XMFLOAT3 itemPos = pItem->GetPosition();
//		float dx = itemPos.x - playerPos.x;
//		float dy = itemPos.y - playerPos.y;
//		float dz = itemPos.z - playerPos.z;
//		float distSqr = dx * dx + dy * dy + dz * dz;
//
//		if (distSqr < nearestDistSqr) {
//			nearestDistSqr = distSqr;
//			nearestId = pair.first;
//		}
//	}
//	return nearestId;
//}
//
//void CGameFramework::CheckNearbyItemPrompt()
//{
//	XMFLOAT3 playerPos = m_pPlayer->GetPosition();
//	long long nearestItemID = FindNearestItemInRange(Recognized_Range, playerPos);
//	if (nearestItemID != -1) {
//		std::cout << "물건을 잡으세요(F)" << std::endl;
//	}
//}
//
//void CGameFramework::ItemToHand(Item* pItem)
//{
//	if (!pItem) return;
//
//	long long targetID = pItem->GetUniqueID();
//
//	CGameObject* pRightHand = m_pPlayer->FindFrame("hand_r");
//
//	// 이미 들고 있는지 검사 (고유 ID 기준)
//	CGameObject* pCurr = pRightHand->GetChild();
//	CGameObject* pPrev = nullptr;
//	bool alreadyHeld = false;
//
//	while (pCurr) {
//		Item* pCurrItem = dynamic_cast<Item*>(pCurr);
//		if (pCurrItem && pCurrItem->GetUniqueID() == targetID) {
//			alreadyHeld = true;
//			break;
//		}
//		pPrev = pCurr;
//		pCurr = pCurr->GetSibling();
//	}
//
//	if (!alreadyHeld) {
//		// 들기
//		if (pRightHand->GetChild() == nullptr) {
//			pRightHand->SetChild(pItem);
//		}
//		else {
//			CGameObject* last = pRightHand->GetChild();
//			while (last->GetSibling()) last = last->GetSibling();
//			last->m_pSibling = pItem;
//		}
//		pItem->m_pParent = pRightHand;
//		pItem->m_pSibling = nullptr;
//
//		// 포지션 조정
//		pItem->SetPosition(0.05f, -0.05f, 0.1f);
//
//		m_pPlayer->UpdateTransform(nullptr);
//
//		// 수정된 위치로 서버에 전송
//		SendItemMove(targetID, pItem->GetPosition());
//	}
//	else {
//		// 놓기
//		if (pPrev) {
//			pPrev->m_pSibling = pItem->GetSibling();
//		}
//		else {
//			pRightHand->SetChild(pItem->GetSibling());
//		}
//		pItem->m_pParent = nullptr;
//		pItem->m_pSibling = nullptr;
//
//		// 필요하면 아이템 놓인 위치 서버에 전송
//		SendItemMove(targetID, pItem->GetPosition());
//	}
//}
//
//void CGameFramework::ItemDropFromHand(Item* pItem)
//{
//	if (!pItem) return;
//
//	CGameObject* pRightHand = m_pPlayer->FindFrame("hand_r");
//
//	// 손에서 아이템 분리
//	CGameObject* pCurr = pRightHand->GetChild();
//	CGameObject* pPrev = nullptr;
//	while (pCurr) {
//		if (pCurr == pItem) break;
//		pPrev = pCurr;
//		pCurr = pCurr->GetSibling();
//	}
//	if (!pCurr) return;
//
//	if (pPrev) {
//		pPrev->m_pSibling = pItem->GetSibling();
//	}
//	else {
//		pRightHand->SetChild(pItem->GetSibling());
//	}
//	pItem->m_pParent = nullptr;
//	pItem->m_pSibling = nullptr;
//
//	// 땅에 놓는 위치로 이동 (예: 플레이어 앞쪽 1m 지점, 높이는 아이템 바닥에 맞게 조절)
//	XMFLOAT3 playerPos = m_pPlayer->GetPosition();
//	XMFLOAT3 forward = m_pPlayer->GetLook();  // 플레이어가 보는 방향
//	XMFLOAT3 dropPos = XMFLOAT3(playerPos.x + forward.x, playerPos.y, playerPos.z + forward.z);
//
//	pItem->SetPosition(dropPos);
//	pItem->UpdateTransform();
//
//	m_pPlayer->UpdateTransform(nullptr);
//
//	// 서버에도 위치 갱신 전송 필요하면 호출
//	SendItemMove(pItem->GetUniqueID(), dropPos);
//}


// -------------------------------------------------------------------------------------------


//void CGameFramework::ItemToHand(int objectIndex)
//{
//	CGameObject* pItem = m_pScene->m_ppHierarchicalGameObjects[objectIndex];
//	CGameObject* pRightHand = m_pPlayer->FindFrame("hand_r");
//
//	// 이미 붙어있나 확인
//	CGameObject* pCurr = pRightHand->GetChild();
//	CGameObject* pPrev = nullptr;
//	bool alreadyHeld = false;
//
//	while (pCurr) {
//		if (pCurr == pItem) {
//			alreadyHeld = true;
//			break;
//		}
//		pPrev = pCurr;
//		pCurr = pCurr->GetSibling();
//	}
//
//	if (!alreadyHeld) {
//		// 들기
//		if (pRightHand->GetChild() == nullptr) {
//			pRightHand->SetChild(pItem);
//		}
//		else {
//			CGameObject* last = pRightHand->GetChild();
//			while (last->GetSibling()) last = last->GetSibling();
//			last->m_pSibling = pItem;
//		}
//		pItem->m_pParent = pRightHand;
//		pItem->m_pSibling = nullptr;
//
//		if (objectIndex == 2) {
//			pItem->SetPosition(0.05f, -0.05f, 1.f); 
//		}
//		else { 
//			pItem->SetPosition(0.05f, -0.05f, 0.1f); 
//		}
//
//		m_pPlayer->UpdateTransform(nullptr);
//	}
//	else {
//		// 놓기
//		if (pPrev) {
//			pPrev->m_pSibling = pItem->GetSibling();
//		}
//		else {
//			pRightHand->SetChild(pItem->GetSibling());
//		}
//		pItem->m_pParent = nullptr;
//		pItem->m_pSibling = nullptr;
//	}
//}

void CGameFramework::OnDestroy()
{
    ReleaseObjects();

	::CloseHandle(m_hFenceEvent);

	if (m_pd3dDepthStencilBuffer) m_pd3dDepthStencilBuffer->Release();
	if (m_pd3dDsvDescriptorHeap) m_pd3dDsvDescriptorHeap->Release();

	for (int i = 0; i < m_nSwapChainBuffers; i++) if (m_ppd3dSwapChainBackBuffers[i]) m_ppd3dSwapChainBackBuffers[i]->Release();
	if (m_pd3dRtvDescriptorHeap) m_pd3dRtvDescriptorHeap->Release();

	if (m_pd3dCommandAllocator) m_pd3dCommandAllocator->Release();
	if (m_pd3dCommandQueue) m_pd3dCommandQueue->Release();
	if (m_pd3dCommandList) m_pd3dCommandList->Release();

	if (m_pd3dFence) m_pd3dFence->Release();

	m_pdxgiSwapChain->SetFullscreenState(FALSE, NULL);
	if (m_pdxgiSwapChain) m_pdxgiSwapChain->Release();
    if (m_pd3dDevice) m_pd3dDevice->Release();
	if (m_pdxgiFactory) m_pdxgiFactory->Release();

#if defined(_DEBUG)
	IDXGIDebug1	*pdxgiDebug = NULL;
	DXGIGetDebugInterface1(0, __uuidof(IDXGIDebug1), (void **)&pdxgiDebug);
	HRESULT hResult = pdxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_DETAIL);
	pdxgiDebug->Release();
#endif
}

#define _WITH_TERRAIN_PLAYER

void CGameFramework::BuildObjects()
{
	isLoading = true;

	m_pd3dCommandList->Reset(m_pd3dCommandAllocator, NULL);

	m_nScenes = 2; // 총 Scene 개수
	m_ppScenes = new CScene * [m_nScenes];

	bool b = false;

	if (m_nCurrentScene == 0) {
		m_ppScenes[0] = new CStartScene();
		m_ppScenes[0]->BuildObjects(m_pd3dDevice, m_pd3dCommandList);
		CTerrainPlayer* pPlayer = new CTerrainPlayer(m_pd3dDevice, m_pd3dCommandList, m_ppScenes[0]->GetGraphicsRootSignature(),NULL);
		m_ppScenes[0]->SetPlayer(pPlayer);
	}
	else if (m_nCurrentScene == 1) {
		m_ppScenes[1] = new CScene();
		m_ppScenes[1]->BuildObjects(m_pd3dDevice, m_pd3dCommandList);
		CTerrainPlayer* pPlayer = new CTerrainPlayer(m_pd3dDevice, m_pd3dCommandList, m_ppScenes[1]->GetGraphicsRootSignature(), NULL);

		m_ppScenes[1]->SetPlayer(pPlayer);
		m_pPlayer->SetPosition(XMFLOAT3(3, 0, 20));

		m_ppScenes[1]->GenerateGameObjectsBoundingBox();
		m_ppScenes[1]->InitializeCollisionSystem();
	}

//#ifdef _WITH_TERRAIN_PLAYER
//	CTerrainPlayer *pPlayer = new CTerrainPlayer(m_pd3dDevice, m_pd3dCommandList, m_pScene->GetGraphicsRootSignature(), m_pScene->m_pTerrain);
//#else
//	CAirplanePlayer *pPlayer = new CAirplanePlayer(m_pd3dDevice, m_pd3dCommandList, m_pScene->GetGraphicsRootSignature(), NULL);
//	pPlayer->SetPosition(XMFLOAT3(425.0f, 240.0f, 640.0f));
//#endif

	m_nScene = m_nCurrentScene; // 현재 활성화 Scene 인덱스
	m_pScene = m_ppScenes[m_nScene];
	m_pScene->m_pPlayer = m_pPlayer = m_pScene->GetPlayer();
	m_pCamera = m_pPlayer->ChangeCamera(THIRD_PERSON_CAMERA, m_GameTimer.GetTimeElapsed());

	if (m_nCurrentScene == 1)
	{ 

		m_pScene->m_ppMonsters[0]->SetPlayer(m_pPlayer);
	}

	m_pd3dCommandList->Close();
	ID3D12CommandList *ppd3dCommandLists[] = { m_pd3dCommandList };
	m_pd3dCommandQueue->ExecuteCommandLists(1, ppd3dCommandLists);

	WaitForGpuComplete();

	if (m_pScene) m_pScene->ReleaseUploadBuffers();
	if (m_pPlayer) m_pPlayer->ReleaseUploadBuffers();

	m_GameTimer.Reset();
	isLoading = false;
}

void CGameFramework::ReleaseObjects()
{
	if (m_pPlayer) m_pPlayer->Release();

	if (m_pScene) m_pScene->ReleaseObjects();
	if (m_pScene) delete m_pScene;
}

void CGameFramework::ProcessInput()
{
	static UCHAR pKeysBuffer[256];
	
	static bool bPrevSpace = false;

	bool bProcessedByScene = false;
	if (GetKeyboardState(pKeysBuffer) && m_pScene) bProcessedByScene = m_pScene->ProcessInput(pKeysBuffer);
	if (!bProcessedByScene)
	{
		float cxDelta = 0.0f, cyDelta = 0.0f;
		POINT ptCursorPos;
		if (GetCapture() == m_hWnd && !(m_pScene && m_pScene->isShop))
		{
			SetCursor(NULL);
			GetCursorPos(&ptCursorPos);
			cxDelta = (float)(ptCursorPos.x - m_ptOldCursorPos.x) / 3.0f;
			cyDelta = (float)(ptCursorPos.y - m_ptOldCursorPos.y) / 3.0f;
			SetCursorPos(m_ptOldCursorPos.x, m_ptOldCursorPos.y);
		}
		m_pScene->m_ptPos = m_ptOldCursorPos;
		DWORD dwDirection = 0;
		if (pKeysBuffer['W'] & 0xF0) dwDirection |= DIR_FORWARD;
		if (pKeysBuffer['S'] & 0xF0) dwDirection |= DIR_BACKWARD;
		if (pKeysBuffer['A'] & 0xF0) dwDirection |= DIR_LEFT;
		if (pKeysBuffer['D'] & 0xF0) dwDirection |= DIR_RIGHT;
		if (pKeysBuffer[VK_SPACE] & 0xF0) dwDirection |= DIR_UP;
		if (pKeysBuffer[VK_SHIFT] & 0xF0) dwDirection |= DIR_DOWN;
		if (pKeysBuffer[VK_CONTROL] & 0xF0) dwDirection |= DIR_CROUCH;

		bool bCurrSpace = (pKeysBuffer[VK_SPACE] & 0xF0);      // jump

		CTerrainPlayer* terrainPlayer = dynamic_cast<CTerrainPlayer*>(m_pPlayer);
		if (!terrainPlayer) return;
		AnimationState currentState = terrainPlayer->m_currentAnim;

		// 점프 중이 아닐 때만 상태 전환
		if (currentState != AnimationState::SWING && currentState != AnimationState::JUMP)
		{
			bool isMoving = dwDirection & (DIR_FORWARD | DIR_BACKWARD | DIR_LEFT | DIR_RIGHT);
			bool isCrouching = dwDirection & DIR_CROUCH;
			bool isRunning = dwDirection & DIR_DOWN;

			if (bCurrSpace && !bPrevSpace)
			{
				terrainPlayer->m_currentAnim = AnimationState::JUMP;
			}
			
			else if (isCrouching && isMoving)
			{
				terrainPlayer->m_currentAnim = AnimationState::CROUCH_WALK;
				terrainPlayer->Move(dwDirection, 2.0f, true);
			}
			else if (isCrouching)
			{
				terrainPlayer->m_currentAnim = AnimationState::CROUCH;
			}
			else if (isRunning && isMoving)
			{
				terrainPlayer->m_currentAnim = AnimationState::RUN;
				terrainPlayer->Move(dwDirection, 2.0f, true);
			}
			else if (isMoving)
			{
				terrainPlayer->m_currentAnim = AnimationState::WALK;
				terrainPlayer->Move(dwDirection, 2.0f, true);
			}
			else
			{
				terrainPlayer->m_currentAnim = AnimationState::IDLE;
			}
		}

		bPrevSpace = bCurrSpace;

		if (cxDelta || cyDelta)
		{
			if (pKeysBuffer[VK_RBUTTON] & 0xF0)
				m_pPlayer->Rotate(cyDelta, 0.0f, -cxDelta);
			else
				m_pPlayer->Rotate(cyDelta, cxDelta, 0.0f);
		}

	}
	m_pPlayer->Update(m_GameTimer.GetTimeElapsed());
}

void CGameFramework::AnimateObjects()
{
	float fTimeElapsed = m_GameTimer.GetTimeElapsed();

	if (m_pScene) { 
		m_pScene->AnimateObjects(fTimeElapsed);
		if (m_pScene->m_ppOtherPlayers) m_pScene->m_ppOtherPlayers[0]->Animate(m_pScene->m_ppOtherPlayers[0]->targetAnim, fTimeElapsed);
		if (m_pScene->m_ppMonsters) 
			for(int i=0;i< m_pScene->m_nMonster; ++i)
			m_pScene->m_ppMonsters[i]->Animate(fTimeElapsed);
	}

	m_pPlayer->Animate(fTimeElapsed);

	if (m_nCurrentScene == 0) m_pPlayer->SetPosition(XMFLOAT3(3, 0, 20));
	//if (m_nCurrentScene == 1) {
	//	for (int i = 0; i < 4; ++i)
	//	{
	//		if (!m_pPlayer->items[i]) // 손에 들리지 않은 상태일 때만
	//		{	
	//			if (m_pScene->m_ppHierarchicalGameObjects[i]) {
	//				XMFLOAT3 pos = m_pScene->m_ppHierarchicalGameObjects[i]->GetPosition();
	//				if (pos.y > 0.1f) pos.y -= 0.1;
	//				m_pScene->m_ppHierarchicalGameObjects[i]->SetPosition(pos);
	//			}
	//		}
	//	}
	//}
}

void CGameFramework::WaitForGpuComplete()
{
	const UINT64 nFenceValue = ++m_nFenceValues[m_nSwapChainBufferIndex];
	HRESULT hResult = m_pd3dCommandQueue->Signal(m_pd3dFence, nFenceValue);

	if (m_pd3dFence->GetCompletedValue() < nFenceValue)
	{
		hResult = m_pd3dFence->SetEventOnCompletion(nFenceValue, m_hFenceEvent);
		::WaitForSingleObject(m_hFenceEvent, INFINITE);
	}
}

void CGameFramework::MoveToNextFrame()
{
	m_nSwapChainBufferIndex = m_pdxgiSwapChain->GetCurrentBackBufferIndex();

	UINT64 nFenceValue = ++m_nFenceValues[m_nSwapChainBufferIndex];
	HRESULT hResult = m_pd3dCommandQueue->Signal(m_pd3dFence, nFenceValue);

	if (m_pd3dFence->GetCompletedValue() < nFenceValue)
	{
		hResult = m_pd3dFence->SetEventOnCompletion(nFenceValue, m_hFenceEvent);
		::WaitForSingleObject(m_hFenceEvent, INFINITE);
	}
}

void CGameFramework::MoveToNextScene()
{
	m_ppScenes[m_nScene]->ReleaseObjects();
	m_nCurrentScene = 1;
	BuildObjects();
	LoadingDoneToServer();
	isStartScene = false;
}

//#define _WITH_PLAYER_TOP

void CGameFramework::FrameAdvance()
{    
	m_GameTimer.Tick(60.0f);
	

	HRESULT hResult = m_pd3dCommandAllocator->Reset();
	hResult = m_pd3dCommandList->Reset(m_pd3dCommandAllocator, NULL);

	D3D12_RESOURCE_BARRIER d3dResourceBarrier;
	::ZeroMemory(&d3dResourceBarrier, sizeof(D3D12_RESOURCE_BARRIER));
	d3dResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	d3dResourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	d3dResourceBarrier.Transition.pResource = m_ppd3dSwapChainBackBuffers[m_nSwapChainBufferIndex];
	d3dResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	d3dResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	d3dResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	m_pd3dCommandList->ResourceBarrier(1, &d3dResourceBarrier);

	D3D12_CPU_DESCRIPTOR_HANDLE d3dRtvCPUDescriptorHandle = m_pd3dRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	d3dRtvCPUDescriptorHandle.ptr += (m_nSwapChainBufferIndex * ::gnRtvDescriptorIncrementSize);

	float pfClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f };
	m_pd3dCommandList->ClearRenderTargetView(d3dRtvCPUDescriptorHandle, pfClearColor/*Colors::Azure*/, 0, NULL);

	D3D12_CPU_DESCRIPTOR_HANDLE d3dDsvCPUDescriptorHandle = m_pd3dDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_pd3dCommandList->ClearDepthStencilView(d3dDsvCPUDescriptorHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);

	m_pd3dCommandList->OMSetRenderTargets(1, &d3dRtvCPUDescriptorHandle, TRUE, &d3dDsvCPUDescriptorHandle);

	if(!isStartScene) ProcessInput();

	AnimateObjects();

	if (m_pScene) m_pScene->Render(m_pd3dCommandList, m_pCamera);

#ifdef _WITH_PLAYER_TOP
	m_pd3dCommandList->ClearDepthStencilView(d3dDsvCPUDescriptorHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);
#endif
	if (m_pPlayer && !isStartScene) m_pPlayer->Render(m_pd3dCommandList, m_pCamera);

	d3dResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	d3dResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	d3dResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	m_pd3dCommandList->ResourceBarrier(1, &d3dResourceBarrier);

	hResult = m_pd3dCommandList->Close();
	
	ID3D12CommandList *ppd3dCommandLists[] = { m_pd3dCommandList };
	m_pd3dCommandQueue->ExecuteCommandLists(1, ppd3dCommandLists);

	WaitForGpuComplete();

#ifdef _WITH_PRESENT_PARAMETERS
	DXGI_PRESENT_PARAMETERS dxgiPresentParameters;
	dxgiPresentParameters.DirtyRectsCount = 0;
	dxgiPresentParameters.pDirtyRects = NULL;
	dxgiPresentParameters.pScrollRect = NULL;
	dxgiPresentParameters.pScrollOffset = NULL;
	m_pdxgiSwapChain->Present1(1, 0, &dxgiPresentParameters); 
#else
#ifdef _WITH_SYNCH_SWAPCHAIN
	m_pdxgiSwapChain->Present(1, 0);
#else
	m_pdxgiSwapChain->Present(0, 0);
#endif
#endif

	MoveToNextFrame();

	m_GameTimer.GetFrameRate(m_pszFrameRate + 7, 37);
	size_t nLength = _tcslen(m_pszFrameRate);
	std::wstring w_user_name(user_name.begin(), user_name.end());
	_stprintf_s(m_pszFrameRate + nLength, 70 - nLength, _T("ID : %s"), w_user_name.c_str());
	::SetWindowText(m_hWnd, m_pszFrameRate);
}

void CGameFramework::OnMonsterSpawned(int monsterID, const XMFLOAT3& pos, int state)
{
	auto it = g_monsters.find(monsterID);
	if (it != g_monsters.end())
	{
		// 기존 몬스터 위치/상태/HP 갱신
		it->second->SetPosition(pos);
		UpdateMonsterState(it->second, state);
	}
	else
	{
		//// 몬스터 객체 새로 생성 (생성자 파라미터는 적절히 수정)
		//CSpider* pMonster = new CSpider(pd3dDevice, pd3dCommandList, pRootSignature, pModel, 5);
		//pMonster->SetPosition(pos);
		//UpdateMonsterState(pMonster, state);


		//g_monsters[monsterID] = pMonster;

		//// 씬에서 관리하는 리스트나 배열에도 추가할 수 있음
	}
}

void CGameFramework::UpdateMonsterState(CSpider* pMonster, int state)
{
	// 애니메이션 트랙 설정 등
	for (int i = 0; i < 5; ++i)
		pMonster->m_pSkinnedAnimationController->SetTrackEnable(i, false);

	switch (state)
	{
	case 0: pMonster->m_pSkinnedAnimationController->SetTrackEnable(0, true); break; // idle
	case 1: pMonster->m_pSkinnedAnimationController->SetTrackEnable(1, true); break; // walk
	case 2: pMonster->m_pSkinnedAnimationController->SetTrackEnable(2, true); break; // run
	case 3: pMonster->m_pSkinnedAnimationController->SetTrackEnable(3, true); break; // attack
	case 4: pMonster->m_pSkinnedAnimationController->SetTrackEnable(4, true); break; // death
	default: break;
	}
}

void CGameFramework::UpdateMonsterPosition(int monsterID, const XMFLOAT3& pos, int state)
{
	auto it = g_monsters.find(monsterID);
	if (it == g_monsters.end())
	{
		std::cout << "[Error] Monster ID not found: " << monsterID << std::endl;
		return;
	}

	CSpider* pMonster = it->second;
	pMonster->SetPosition(pos);
	UpdateMonsterState(pMonster, state);
}