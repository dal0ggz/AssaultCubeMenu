#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <d3d9.h>
#include <TlHelp32.h>
#include <cstdint>
#include <vector>
#include <string>
#include <cmath> // Per l'effetto arcobaleno

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx9.h"

#pragma comment(lib, "d3d9.lib")

// --- STATO DELL'APPLICAZIONE ---
struct AppState {
    bool running = true;
    bool show_menu = true;
    bool connected = false;

    HANDLE hProc = nullptr;
    uintptr_t modBase = 0;
    uintptr_t localPlayer = 0;
    float position[3] = { 0, 0, 0 };

    bool t_godmode = false;
    bool t_maxammo = false;
    bool t_grenades = false;
};

// --- UTILITY ---
DWORD GetProcId(const wchar_t* exe) {
    DWORD id = 0;
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snap != INVALID_HANDLE_VALUE) {
        PROCESSENTRY32W e; e.dwSize = sizeof(e);
        if (Process32FirstW(snap, &e)) {
            do { if (!_wcsicmp(e.szExeFile, exe)) { id = e.th32ProcessID; break; } } while (Process32NextW(snap, &e));
        }
        CloseHandle(snap);
    }
    return id;
}

uintptr_t GetModuleBase(DWORD pid, const wchar_t* mod) {
    uintptr_t b = 0;
    HANDLE s = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid);
    if (s != INVALID_HANDLE_VALUE) {
        MODULEENTRY32W m; m.dwSize = sizeof(m);
        if (Module32FirstW(s, &m)) {
            do { if (!_wcsicmp(m.szModule, mod)) { b = (uintptr_t)m.modBaseAddr; break; } } while (Module32NextW(s, &m));
        }
        CloseHandle(s);
    }
    return b;
}

static LPDIRECT3DDEVICE9 g_pd3dDevice = nullptr;
static D3DPRESENT_PARAMETERS g_d3dpp = {};

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam)) return 1;
    if (msg == WM_DESTROY) { PostQuitMessage(0); return 0; }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

int main() {
    // 1. FINESTRA OVERLAY
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, L"OverlayClass", NULL };
    RegisterClassEx(&wc);

    HWND hwnd = CreateWindowEx(
        WS_EX_TOPMOST | WS_EX_LAYERED,
        wc.lpszClassName, L"AssaultCube Overlay", WS_POPUP,
        0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN),
        NULL, NULL, wc.hInstance, NULL
    );

    SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), 0, LWA_COLORKEY);

    // 2. DX9
    LPDIRECT3D9 pD3D = Direct3DCreate9(D3D_SDK_VERSION);
    ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
    g_d3dpp.Windowed = TRUE;
    g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    g_d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
    pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pd3dDevice);

    ShowWindow(hwnd, SW_SHOWDEFAULT);

    // 3. IMGUI
    ImGui::CreateContext();
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX9_Init(g_pd3dDevice);

    AppState s;

    while (s.running) {
        MSG msg;
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg); DispatchMessage(&msg);
            if (msg.message == WM_QUIT) s.running = false;
        }

        // TASTI RAPIDI
        if (GetAsyncKeyState(VK_INSERT) & 1) s.show_menu = !s.show_menu;
        if (GetAsyncKeyState(VK_END) & 1) s.running = false;

        // --- FIX INTERAZIONE MOUSE ---
        // Se il menu è aperto, rendiamo la finestra cliccabile. Se chiuso, trasparente ai click.
        LONG_PTR exStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
        if (s.show_menu) {
            SetWindowLongPtr(hwnd, GWL_EXSTYLE, exStyle & ~WS_EX_TRANSPARENT);
        }
        else {
            SetWindowLongPtr(hwnd, GWL_EXSTYLE, exStyle | WS_EX_TRANSPARENT);
        }

        // --- BACKEND MEMORIA ---
        if (!s.connected) {
            DWORD pid = GetProcId(L"ac_client.exe");
            if (pid) {
                s.modBase = GetModuleBase(pid, L"ac_client.exe");
                s.hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
                if (s.hProc) s.connected = true;
            }
        }
        else {
            if (ReadProcessMemory(s.hProc, (LPCVOID)(s.modBase + 0x17E0A8), &s.localPlayer, sizeof(uintptr_t), NULL)) {
                if (s.localPlayer) {
                    ReadProcessMemory(s.hProc, (LPCVOID)(s.localPlayer + 0x2C), &s.position[0], 12, NULL);
                    if (s.t_godmode) { int val = 999; WriteProcessMemory(s.hProc, (LPVOID)(s.localPlayer + 0xEC), &val, 4, NULL); }
                    if (s.t_maxammo) { int val = 999; WriteProcessMemory(s.hProc, (LPVOID)(s.localPlayer + 0x140), &val, 4, NULL); }
                    if (s.t_grenades) { int val = 99; WriteProcessMemory(s.hProc, (LPVOID)(s.localPlayer + 0x144), &val, 4, NULL); }
                }
            }
            else { s.connected = false; CloseHandle(s.hProc); s.hProc = nullptr; }
        }

        // --- RENDERING ---
        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        if (s.show_menu) {
            ImGui::SetNextWindowSize(ImVec2(400, 320), ImGuiCond_FirstUseEver);
            if (ImGui::Begin("AssaultCube Premium Overlay", &s.show_menu)) {

                // EFFETTO ARCOBALENO FIRMA
                float t = (float)ImGui::GetTime();
                ImGui::TextColored(ImVec4(sin(t) * 0.5f + 0.5f, cos(t) * 0.5f + 0.5f, sin(t + 1.5f) * 0.5f + 0.5f, 1.0f), "Made by daloggz");
                ImGui::Separator();

                if (ImGui::BeginTabBar("Tabs")) {
                    if (ImGui::BeginTabItem("Trainer")) {
                        if (!s.connected) ImGui::TextColored(ImVec4(1, 0, 0, 1), "Game not found!");
                        else {
                            ImGui::Text("X: %.1f Y: %.1f Z: %.1f", s.position[0], s.position[1], s.position[2]);
                            ImGui::Checkbox("God Mode", &s.t_godmode);
                            ImGui::Checkbox("Infinite Ammo", &s.t_maxammo);
                            ImGui::Checkbox("Infinite Grenades", &s.t_grenades);
                            if (ImGui::Button("Reset All", ImVec2(-1, 30))) {
                                int h = 100, a = 100, g = 1;
                                WriteProcessMemory(s.hProc, (LPVOID)(s.localPlayer + 0xEC), &h, 4, NULL);
                                WriteProcessMemory(s.hProc, (LPVOID)(s.localPlayer + 0x140), &a, 4, NULL);
                                WriteProcessMemory(s.hProc, (LPVOID)(s.localPlayer + 0x144), &g, 4, NULL);
                                s.t_godmode = s.t_maxammo = s.t_grenades = false;
                            }
                        }
                        ImGui::EndTabItem();
                    }
                    ImGui::EndTabBar();
                }
                ImGui::SetCursorPosY(ImGui::GetWindowHeight() - 25);
                ImGui::TextDisabled("v1.1 | daloggz-edition");
            }
            ImGui::End();
        }

        ImGui::EndFrame();
        g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);
        if (g_pd3dDevice->BeginScene() >= 0) {
            ImGui::Render();
            ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
            g_pd3dDevice->EndScene();
        }
        g_pd3dDevice->Present(NULL, NULL, NULL, NULL);
        Sleep(10);
    }

    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    return 0;
}