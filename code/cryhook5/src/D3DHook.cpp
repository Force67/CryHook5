
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

#include <CryHook5.h>

#include <Jitasm.h>

struct D3D_Class_Legacy
{
    char padding[80];
    IDXGIFactory *factory;
};

struct D3D_Class
{
    char padding[48];
    IDXGIFactory *factory;
};

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

static HRESULT(*D3D11Present_Wrap_Orig)(int64_t*, int64_t*, int64_t*);

static HRESULT D3D11Present_Wrap(int64_t* Device3D, int64_t* a2, int64_t* a3)
{
    ImGui_ImplDX11_NewFrame();

    // draw gui
    if (input::g_showmenu) gui::RenderMenus();
    gui::RenderGenerics();

    // process render tasks
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    return D3D11Present_Wrap_Orig(Device3D, a2, a3);
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

static IDXGIFactory *GetFactory__Legacy(D3D_Class_Legacy *dat_class)
{
    auto vtable = (DWORD_PTR*)((DWORD_PTR*)dat_class->factory)[0];
   
    // hook the swap chain creation in a bit of a ugly way
    DWORD op;
    VirtualProtect(&vtable[10], 8, PAGE_EXECUTE_READWRITE, &op);
    CreateSwapChain_Org = (decltype(CreateSwapChain_Org))(vtable)[10];
    ((uintptr_t**)vtable)[10] = (uintptr_t*)&CreateSwapChain_Hook;

    return dat_class->factory;
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
    /* call moved from 
       mtl-zeta-whitefish\\Code\\platforms\\d3d11\\GraphicsToolbox\\ResolutionHandler.cpp
       to 
       \mtl-zeta-whitefish\\Code\\platforms\\d3d11\\GraphicsToolbox\\Device3D\\SwapChainNative.cpp
       in newer builds*/

    char *loc = nullptr;
    auto matches = nio::pattern("FF 50 10 4C 8B 8C 24");

    // old
    if (matches.size() > 0)
    {
        loc = matches.first(37 + 3);

        // in case we might need it later on !
        //g_factory_legacy = *(D3D_Class_Legacy**)(loc + *(int32_t*)loc + 4);

        nio::put_call(loc + 4, GetFactory__Legacy);

        // next step : hook the present
        // allways wanna be topmost, eh
        loc = nio::pattern("FF 50 40 8B D8 85 C0").first();
        nio::nop(loc, 16);
        nio::put_call(loc, D3D11Present_Hook);

    } else
    {
        // our beloved getter moved to the beginning of the function
        loc = nio::pattern("4D 89 C4 49 89 D5 E8 ? ? ? ? 49 89 C7").first(-4);

        //g_factory = *(D3D_Class**)(loc + *(int32_t*)loc + 4);
        nio::put_call(loc + 10, GetFactory);

        // the present function call got moved into its own function now
        /*loc = nio::pattern("5F C3 48 8B 4B 08 48 8B 01").first(9);
        nio::nop(loc, 13);
        nio::nop(loc + 13 + 4, 1);
        nio::put_call(loc, D3D11Present_Hook);*/

        loc = nio::pattern("74 19 41 8B 57 04").first(9);
        nio::set_call(&D3D11Present_Wrap_Orig, loc);
        nio::put_call(loc, D3D11Present_Wrap);
    }
});