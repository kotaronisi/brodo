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
ID3D12CommandAllocator* g_commandAllocators[g_inflightFrameCount];
ID3D12GraphicsCommandList* g_commandLists[g_inflightFrameCount];

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

    return true;
}