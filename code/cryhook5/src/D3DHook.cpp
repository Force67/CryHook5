
/*
*  This source file is part of the Far Cry 5 ScriptHook by Force67
*  More information regarding licensing can be found in LICENSE.md
*/

#include <Hooking.h>
#include <Nomad/nomad_base_function.h>

#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include <imgui_impl_dx11.h>
#include <imgui.h>

#include <Game.h>
#include <CryHook5.h>

#include <Jitasm.h>

class D3D_Class
{
public:

    char padding[80];
    IDXGIFactory *factory;
};

static D3D_Class *g_factory;
static IDXGISwapChain **g_swapchain;

static ID3D11Device* d3dDevice = nullptr;
static ID3D11DeviceContext* d3dContext = nullptr;
static ID3D11RenderTargetView*  DX11RenderTargetView = nullptr;

static bool Initialize(IDXGISwapChain* _SwapChain)
{
    _SwapChain->GetDevice(__uuidof(d3dDevice), reinterpret_cast<void**>(&d3dDevice));
    d3dDevice->GetImmediateContext(&d3dContext);

    // and create a texture
    DXGI_SWAP_CHAIN_DESC sd;
    _SwapChain->GetDesc(&sd);

    // Create the render target
    ID3D11Texture2D* pBackBuffer;
    D3D11_RENDER_TARGET_VIEW_DESC render_target_view_desc;
    ZeroMemory(&render_target_view_desc, sizeof(render_target_view_desc));
    render_target_view_desc.Format = sd.BufferDesc.Format;
    render_target_view_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    _SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
    d3dDevice->CreateRenderTargetView(pBackBuffer, &render_target_view_desc, &DX11RenderTargetView);
    d3dContext->OMSetRenderTargets(1, &DX11RenderTargetView, nullptr);
    pBackBuffer->Release();

    return ImGui_ImplDX11_Init(sd.OutputWindow, d3dDevice, d3dContext);
}

static HRESULT D3D11Present_Hook(IDXGISwapChain *self, UINT syncinterval, UINT flags)
{
    ImGui_ImplDX11_NewFrame();

    // draw gui
    if (input::g_showmenu) gui::RenderMenus();
    gui::RenderGenerics();

    // process render tasks
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    return (*g_swapchain)->Present(syncinterval, 0);
}

HRESULT (STDMETHODCALLTYPE *CreateSwapChain_Org)(IDXGIFactory *self,
    _In_  IUnknown *pDevice,
    _In_  DXGI_SWAP_CHAIN_DESC *pDesc,
    _COM_Outptr_  IDXGISwapChain **ppSwapChain);

HRESULT CreateSwapChain_Hook(IDXGIFactory *factory, ID3D11Device *pDevice, DXGI_SWAP_CHAIN_DESC *pDesc, IDXGISwapChain **ppSwapChain)
{
    auto result = CreateSwapChain_Org(factory, pDevice, pDesc, ppSwapChain);

    if (FAILED(result))
    {
        MessageBoxA(nullptr, "Not enough vram to run the game!", "CryHook5", MB_ICONERROR);
        exit(1);
    }

    // store this for later use
    g_swapchain = ppSwapChain;

    if (!Initialize(*ppSwapChain))
    {
        MessageBoxA(nullptr, "Unable to create CryHook UI", "CryHook5", MB_ICONWARNING);
    }

    return result;
}

static IDXGIFactory *GetFactory(D3D_Class *dat_class)
{
    auto vtable = (DWORD_PTR*)((DWORD_PTR*)dat_class->factory)[0];
   
    // hook the swap chain creation in a bit of a ugly way
    DWORD op;
    VirtualProtect(&vtable[10], 8, PAGE_EXECUTE_READWRITE, &op);
    CreateSwapChain_Org = (decltype(CreateSwapChain_Org))(vtable)[10];
    ((uintptr_t**)vtable)[10] = (uintptr_t*)&CreateSwapChain_Hook;

    return dat_class->factory;
}

static nomad::base_function init([]()
{
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui::SetNextWindowPos(ImVec2(10, 10));

    auto &io = ImGui::GetIO();
    io.IniFilename = nullptr;

    auto loc = nio::pattern("FF 50 10 4C 8B 8C 24").first(37 + 3);
    // in case we might need it later on !
    g_factory = *(D3D_Class**)(loc + *(int32_t*)loc + 4);

    nio::put_call(loc + 4, GetFactory);

    // next step : hook the present
    // allways wanna be topmost, eh
    loc = nio::pattern("FF 50 40 8B D8 85 C0").first();
    nio::nop(loc, 16);
    nio::put_call(loc, D3D11Present_Hook);
});
