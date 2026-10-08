#include "renderer.h"

#include <string>
#include <d3d12.h>
#include <dxgi1_6.h>

const UINT g_inflightFrameCount = 2;

ID3D12Device* g_graphicsDevice = nullptr;
ID3D12CommandQueue* g_commandQueue = nullptr;
IDXGISwapChain1* g_swapChain = nullptr;
ID3D12DescriptorHeap* g_backBuffersRTV;
UINT g_backBuffersRTVIncrementSize;
ID3D12PipelineState* g_pso;
D3D12_VIEWPORT g_viewPort;
D3D12_RECT g_scissorRect;
ID3D12CommandAllocator* g_commandAllocators[g_inflightFrameCount];
ID3D12GraphicsCommandList* g_commandLists[g_inflightFrameCount];
UINT g_frameIndex = UINT_MAX;
ID3D12Fence* g_fence;
UINT g_fenceValues[g_inflightFrameCount];
HANDLE g_fenceEvent;

const char* CreateDevice(IDXGIFactory* baseFactory)
{
    HRESULT hr = S_OK;

    IDXGIFactory6* factory = nullptr;
    hr = baseFactory->QueryInterface(IID_PPV_ARGS(&factory));
    if (FAILED(hr)) return "Failed to query IDXGIFactory6 interface";

    IDXGIAdapter1* adapter = nullptr;
    for (UINT adapterIndex = 0;
        factory->EnumAdapterByGpuPreference(adapterIndex,
            DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&adapter)) != DXGI_ERROR_NOT_FOUND;
        ++adapterIndex)
    {
        if (adapter == nullptr) continue;
        hr = D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_12_0, __uuidof(ID3D12Device), nullptr);
        if (SUCCEEDED(hr)) break;
    }

    if (adapter == nullptr) return "Failed to find a suitable GPU adapter";
    hr = D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&g_graphicsDevice));
    if (FAILED(hr)) return "Failed to create D3D12 device";

    return nullptr;
}

bool InitRenderer(HINSTANCE hInstance, HWND hwnd, int width, int height)
{
    IDXGIFactory1* factory = nullptr;

    HRESULT hr = CreateDXGIFactory1(IID_PPV_ARGS(&factory));
    if (FAILED(hr)) return false;

    const char* errorMessage = CreateDevice(factory);
    if (errorMessage != nullptr)
    {
        OutputDebugStringA(errorMessage);
        return false;
    }

    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    hr = g_graphicsDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&g_commandQueue));
    if (FAILED(hr))
    {
        OutputDebugStringA("Failed to create D3D12 command queue");
        return false;
    }

    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    swapChainDesc.BufferCount = g_inflightFrameCount;
    swapChainDesc.Width = width;
    swapChainDesc.Height = height;
    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;

    IDXGIFactory2* factory2 = nullptr;
    hr = factory->QueryInterface(IID_PPV_ARGS(&factory2));
    if (FAILED(hr))
    {
        OutputDebugStringA("Failed to query IDXGIFactory2 interface");
        return false;
    }

    hr = factory2->CreateSwapChainForHwnd(g_commandQueue, hwnd, &swapChainDesc, nullptr, nullptr, &g_swapChain);
    if (FAILED(hr))
    {
        OutputDebugStringA("Failed to create swap chain");
        return false;
    }
    IDXGISwapChain3* swapchain3 = nullptr; g_swapChain->QueryInterface(IID_PPV_ARGS(&swapchain3));
    g_frameIndex = swapchain3->GetCurrentBackBufferIndex();
    
    D3D12_DESCRIPTOR_HEAP_DESC backBuffersRTVDesc{};
    backBuffersRTVDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    backBuffersRTVDesc.NumDescriptors = g_inflightFrameCount;
    hr = g_graphicsDevice->CreateDescriptorHeap(&backBuffersRTVDesc, IID_PPV_ARGS(&g_backBuffersRTV));
    g_backBuffersRTVIncrementSize = g_graphicsDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    for (UINT i = 0; i < g_inflightFrameCount; ++i)
    {
        D3D12_CPU_DESCRIPTOR_HANDLE backBufferDescriptor = g_backBuffersRTV->GetCPUDescriptorHandleForHeapStart();
        backBufferDescriptor.ptr += i * g_backBuffersRTVIncrementSize;
        ID3D12Resource* backBuffer = nullptr;  g_swapChain->GetBuffer(i, IID_PPV_ARGS(&backBuffer));
        g_graphicsDevice->CreateRenderTargetView(backBuffer, nullptr, backBufferDescriptor);

        g_graphicsDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&g_commandAllocators[i]));
        g_graphicsDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, g_commandAllocators[i], nullptr, IID_PPV_ARGS(&g_commandLists[i]));
    }

    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc{};
    psoDesc.NumRenderTargets = 1;
    psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

    g_graphicsDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&g_pso));

    g_viewPort.TopLeftX = 0;
    g_viewPort.TopLeftY = 0;
    g_viewPort.Width = width;
    g_viewPort.Height = height;
    g_viewPort.MinDepth = 0.0f;
    g_viewPort.MaxDepth = 1.0f;

    g_scissorRect.left = 0;
    g_scissorRect.top = 0;
    g_scissorRect.right = width;
    g_scissorRect.bottom = height;

    g_graphicsDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&g_fence));
    g_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);

    return true;
}

void OnRender()
{
    ID3D12GraphicsCommandList7* commandList = nullptr;
    g_commandLists[g_frameIndex]->QueryInterface(IID_PPV_ARGS(&commandList));

    commandList->RSSetViewports(1, &g_viewPort);
    commandList->RSSetScissorRects(1, &g_scissorRect);

    ID3D12Resource* backBuffer; g_swapChain->GetBuffer(g_frameIndex, IID_PPV_ARGS(&backBuffer));

    D3D12_TEXTURE_BARRIER backBufferBarrierPresentToRTV{};
    backBufferBarrierPresentToRTV.AccessBefore = D3D12_BARRIER_ACCESS_COMMON;
    backBufferBarrierPresentToRTV.AccessAfter = D3D12_BARRIER_ACCESS_RENDER_TARGET;
    backBufferBarrierPresentToRTV.LayoutBefore = D3D12_BARRIER_LAYOUT_PRESENT;
    backBufferBarrierPresentToRTV.LayoutAfter = D3D12_BARRIER_LAYOUT_RENDER_TARGET;
    backBufferBarrierPresentToRTV.SyncBefore = D3D12_BARRIER_SYNC_NONE;
    backBufferBarrierPresentToRTV.SyncAfter = D3D12_BARRIER_SYNC_RENDER_TARGET;
    backBufferBarrierPresentToRTV.pResource = backBuffer;
    D3D12_BARRIER_GROUP swapchainToRtvBarrier{};
    swapchainToRtvBarrier.NumBarriers = 1;
    swapchainToRtvBarrier.pTextureBarriers = &backBufferBarrierPresentToRTV;

    commandList->Barrier(1, &swapchainToRtvBarrier);

    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle(g_backBuffersRTV->GetCPUDescriptorHandleForHeapStart().ptr + g_frameIndex * g_backBuffersRTVIncrementSize);
    commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

    FLOAT clearColor[4] = { 0.0f, 0.0f, 1.0f, 1.0f };
    commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

    D3D12_TEXTURE_BARRIER backBufferBarrierRTVToPresent{};
    backBufferBarrierRTVToPresent.AccessBefore = D3D12_BARRIER_ACCESS_RENDER_TARGET;
    backBufferBarrierRTVToPresent.AccessAfter = D3D12_BARRIER_ACCESS_COMMON;
    backBufferBarrierRTVToPresent.LayoutBefore = D3D12_BARRIER_LAYOUT_RENDER_TARGET;
    backBufferBarrierRTVToPresent.LayoutAfter = D3D12_BARRIER_LAYOUT_PRESENT;
    backBufferBarrierRTVToPresent.SyncBefore = D3D12_BARRIER_SYNC_RENDER_TARGET;
    backBufferBarrierRTVToPresent.SyncAfter = D3D12_BARRIER_SYNC_NONE;
    backBufferBarrierRTVToPresent.pResource = backBuffer;
    D3D12_BARRIER_GROUP swapchainToPresentBarrier{};
    swapchainToPresentBarrier.NumBarriers = 1;
    swapchainToPresentBarrier.pTextureBarriers = &backBufferBarrierRTVToPresent;

    commandList->Barrier(1, &swapchainToPresentBarrier);

    commandList->Close();

    ID3D12CommandList* ppCommandLists[] = { commandList };
    g_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
    g_swapChain->Present(0, 0);

    UINT fenceValue = ++g_fenceValues[g_frameIndex];
    g_commandQueue->Signal(g_fence, fenceValue);

    g_frameIndex = (g_frameIndex + 1) % g_inflightFrameCount;
    if (g_fence->GetCompletedValue() < g_fenceValues[g_frameIndex])
    {
        g_fence->SetEventOnCompletion(g_fenceValues[g_frameIndex], g_fenceEvent);
        WaitForSingleObject(g_fenceEvent, INFINITE);
    }

    g_fenceValues[g_frameIndex] = fenceValue + 1;
}