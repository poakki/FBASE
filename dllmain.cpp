#include "cheat/cheat.h"
#include "include.h"
#include <iostream>

// [CORREÇÃO] Aqui criamos as variáveis. É o ÚNICO lugar onde elas não têm 'extern'.
uintptr_t uworld_sig = 0;
int width = 1920;
int height = 1080;
bool cornerbox = false;
bool box = false;
bool draw_line = false;
bool aimbot = false;
int boneidselect = 1;
float aimfov = 60.0f;
float smooth_ = 2.0f;

ID3D11RenderTargetView* RenderTarget;
ID3D11DeviceContext* Context;
ID3D11Device* Device;
HWND hwnd;
uintptr_t origPresentPtr_content;
bool ShowMenu = true;
ImVec4 fovcol = ImColor(255, 255, 255);

void show_console() {
    AllocConsole();
    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);
    std::cout << "[DEBUG] Console Iniciado." << std::endl;
}

void menustyle() {
    ImVec4* colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 7;
}

void menu() {
    menustyle();
    if (ShowMenu)
    {
        ImGuiIO& io = ImGui::GetIO();
        POINT p;
        GetCursorPos(&p);
        io.MousePos.x = p.x;
        io.MousePos.y = p.y;
        if (steam_keybind(VK_LBUTTON)) {
            io.MouseDown[0] = true;
            io.MouseClicked[0] = true;
            io.MouseClickedPos[0].x = io.MousePos.x;
            io.MouseClickedPos[0].y = io.MousePos.y;
        }
        else {
            io.MouseDown[0] = false;
        }

        ImGui::SetNextWindowSize({ 350, 350 });
        ImGui::Begin("Refo Internal Cheat");
        ImGui::Checkbox("Corner Box", &cornerbox);
        ImGui::Checkbox("Full Box", &box);
        ImGui::Checkbox("Snaplines", &draw_line);
        ImGui::Separator();
        ImGui::Checkbox("Aimbot", &aimbot);
        ImGui::SliderInt("Aim Bone", &boneidselect, 1, 3);
        ImGui::SliderFloat("FOV", &aimfov, 10.f, 300.f);
        ImGui::SliderFloat("Smooth", &smooth_, 1, 10);
        ImGui::End();
    }
}

__int64 main_hook(IDXGISwapChain* swapchain, __int64 interval, __int64 flags)
{
    static bool first = false;
    if (!first)
    {
        if (SUCCEEDED(swapchain->GetDevice(__uuidof(ID3D11Device), (void**)&Device)))
        {
            Device->GetImmediateContext(&Context);
            DXGI_SWAP_CHAIN_DESC sd;
            swapchain->GetDesc(&sd);
            hwnd = sd.OutputWindow;

            ID3D11Texture2D* pBackBuffer = nullptr;
            if (SUCCEEDED(swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer)))
            {
                if (pBackBuffer) {
                    Device->CreateRenderTargetView(pBackBuffer, NULL, &RenderTarget);
                    pBackBuffer->Release();
                }
            }

            ImGui::CreateContext();
            ImGuiIO& io = ImGui::GetIO();
            io.Fonts->AddFontDefault();
            ImGui_ImplDX11_Init(hwnd, Device, Context);
            ImGui_ImplDX11_CreateDeviceObjects();

            first = true;
        }
        else return ((__int64 (*)(IDXGISwapChain*, __int64, __int64))(origPresentPtr_content))(swapchain, interval, flags);
    }

    if (Device || Context)
    {
        ID3D11Texture2D* renderTargetTexture = nullptr;
        if (!RenderTarget)
        {
            if (SUCCEEDED(swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<LPVOID*>(&renderTargetTexture))))
            {
                if (renderTargetTexture) {
                    Device->CreateRenderTargetView(renderTargetTexture, nullptr, &RenderTarget);
                    renderTargetTexture->Release();
                }
            }
        }
    }

    if (steam_keybind(VK_HOME) || steam_keybind(VK_INSERT))
    {
        Sleep(200);
        ShowMenu = !ShowMenu;
    }
    if (steam_keybind(VK_END))
    {
        exit(0);
    }

    ImGui_ImplDX11_NewFrame();
    menu();

    if (aimbot)
        ImGui::GetOverlayDrawList()->AddCircle(ImVec2(width / 2, height / 2), aimfov, ImGui::ColorConvertFloat4ToU32(fovcol), 64);

    if (aimbot || draw_line || cornerbox || box)
    {
        initcheat();
    }

    ImGui::EndFrame();
    Context->OMSetRenderTargets(1, &RenderTarget, NULL);
    ImGui::Render();

    if (RenderTarget)
    {
        RenderTarget->Release();
        RenderTarget = nullptr;
    }

    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    return ((__int64 (*)(IDXGISwapChain*, __int64, __int64))(origPresentPtr_content))(swapchain, interval, flags);
}

bool init() {
    auto module_ = find_base_address("GameOverlayRenderer64.dll");

    // Nota: Passamos 0 no lugar do nome do modulo para scanner de memória total se necessário,
    // mas idealmente passa module_ se tiveres certeza que a assinatura está lá.
    // Como a assinatura 48 8B 1D... é comum, pode estar no jogo principal.
    uintptr_t found_sig = sig_scan(0, "48 8B 1D ? ? ? ? 48 85 DB 74 3B 41 B0 01 33 D2 48 8B CB E8");

    if (found_sig) {
        auto offset = *(int*)(found_sig + 3);
        uworld_sig = found_sig + offset + 7;
        std::cout << "[INFO] UWorld Sig: " << std::hex << uworld_sig << std::endl;
    }
    else {
        std::cout << "[ERRO] UWorld Sig nao encontrada!" << std::endl;
        return false;
    }

    if (!module_) return false;
    auto steamPresentPtr_address = PatternScan("\x48\xFF\x25\x00\x00\x00\x00\x48\x89\x5C\x24\x30", "xxx????xxxxx", module_, 0xFFFFF);

    if (!steamPresentPtr_address) return false;

    steamPresentPtr_address = *(unsigned int*)(steamPresentPtr_address + 0x3) + steamPresentPtr_address + 0x7;
    origPresentPtr_content = *(uintptr_t*)(steamPresentPtr_address);
    *(uintptr_t*)(steamPresentPtr_address) = (uintptr_t)&main_hook;

    std::cout << "[INFO] Hook instalado." << std::endl;
    return true;
}

bool __stdcall DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
    if (dwReason == 1)
    {
        DisableThreadLibraryCalls(hModule);
        show_console();

        std::thread([&]() {
            Sleep(5000);
            init();
            }).detach();
    }
    return 1;
}