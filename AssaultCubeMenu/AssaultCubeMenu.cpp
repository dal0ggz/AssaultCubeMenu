#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <iostream>
#include <tchar.h>
#include <d3d9.h>
#include <TlHelp32.h>
#include <vector>

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx9.h"

bool SbloccaPrivilegi(LPCWSTR perms, bool attiva) {
    TOKEN_PRIVILEGES tp; HANDLE hToken; LUID luid;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) return 0;
    LookupPrivilegeValue(0, perms, &luid);
    tp.PrivilegeCount = 1; tp.Privileges[0].Luid = luid;
    tp.Privileges[0].Attributes = (attiva) ? SE_PRIVILEGE_ENABLED : 0;
    AdjustTokenPrivileges(hToken, 0, &tp, sizeof(TOKEN_PRIVILEGES), 0, 0);
    return 1;
}

DWORD getpid(const wchar_t* exe) {
    DWORD id = 0;
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snap != INVALID_HANDLE_VALUE) {
        PROCESSENTRY32W e; e.dwSize = sizeof(e);
        if (Process32FirstW(snap, &e)) {
            do {
                if (!_wcsicmp(e.szExeFile, exe)) { id = e.th32ProcessID; break; }
            } while (Process32NextW(snap, &e));
        }
    }
    CloseHandle(snap);
    return id;
}

uintptr_t getmod(DWORD pid, const wchar_t* mod) {
    uintptr_t b = 0;
    HANDLE s = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid);
    if (s != INVALID_HANDLE_VALUE) {
        MODULEENTRY32W m; m.dwSize = sizeof(m);
        if (Module32FirstW(s, &m)) {
            do {
                if (!_wcsicmp(m.szModule, mod)) { b = (uintptr_t)m.modBaseAddr; break; }
            } while (Module32NextW(s, &m));
        }
    }
    CloseHandle(s);
    return b;
}

static LPDIRECT3DDEVICE9 g_pDevice = 0;
static LPDIRECT3D9 g_pD3D = 0;
bool init_d3d(HWND hWnd);
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

int main() {
    AllocConsole();
    FILE* dummy; freopen_s(&dummy, "CONOUT$", "w", stdout);
    SbloccaPrivilegi(SE_DEBUG_NAME, 1);

    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0, 0, GetModuleHandle(0), 0, 0, 0, 0, _T("internal_class"), 0 };
    RegisterClassEx(&wc);
    HWND main_wnd = CreateWindow(wc.lpszClassName, _T("AssaultCube Trainer"), WS_OVERLAPPEDWINDOW, 100, 100, 450, 400, 0, 0, wc.hInstance, 0);

    if (!init_d3d(main_wnd)) return 1;
    ShowWindow(main_wnd, SW_SHOWDEFAULT);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplWin32_Init(main_wnd);
    ImGui_ImplDX9_Init(g_pDevice);

    HANDLE hProc = 0;
    uintptr_t modBase = 0;
    DWORD dwPid = 0;

    float pos[3] = { 0,0,0 };
    int bullets = 999;

    while (1) {
        MSG m;
        while (PeekMessage(&m, 0, 0U, 0U, PM_REMOVE)) {
            TranslateMessage(&m); DispatchMessage(&m);
            if (m.message == WM_QUIT) return 0;
        }

        if (GetAsyncKeyState(VK_END) & 1) break;

        if (!hProc) {
            dwPid = getpid(L"ac_client.exe");
            if (dwPid) {
                modBase = getmod(dwPid, L"ac_client.exe");
                hProc = OpenProcess(PROCESS_ALL_ACCESS, 0, dwPid);
            }
        }

        Sleep(2);

        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
        ImGui::Begin("TrainerMenu", 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);

        if (hProc) {
            uintptr_t local = 0;
            ReadProcessMemory(hProc, (LPCVOID)(modBase + 0x0017E0A8), &local, 4, 0);

            if (local) {
                ReadProcessMemory(hProc, (LPCVOID)(local + 0x2C), &pos[0], 4, 0);
                ReadProcessMemory(hProc, (LPCVOID)(local + 0x30), &pos[1], 4, 0);
                ReadProcessMemory(hProc, (LPCVOID)(local + 0x28), &pos[2], 4, 0);

                if (GetAsyncKeyState(VK_F1) & 1) {
                    std::cout << "Coords: " << pos[0] << " " << pos[1] << " " << pos[2] << std::endl;
                }

                ImGui::Text("Position: %.1f %.1f %.1f", pos[0], pos[1], pos[2]);
                ImGui::Separator();

                if (ImGui::Button("ENABLE GODMODE", ImVec2(160, 50))) {
                    int vita = 999;
                    WriteProcessMemory(hProc, (LPVOID)(local + 0xEC), &vita, 4, 0);
                }

                ImGui::SameLine();

                if (ImGui::Button("SET ARMOR", ImVec2(140, 50))) {
                    int armor = 999;
                    WriteProcessMemory(hProc, (LPVOID)(local + 0xF0), &armor, 4, 0);
                }

                if (ImGui::Button("GIVE MAX AMMO", ImVec2(-1, 40))) {
                    WriteProcessMemory(hProc, (LPVOID)(local + 0x140), &bullets, 4, 0);
                }

                if (GetAsyncKeyState(VK_F4) & 1) {
                    int r = 100;
                    WriteProcessMemory(hProc, (LPVOID)(local + 0xEC), &r, 4, 0);
                }

                if (ImGui::Button("GIVE GRENADES", ImVec2(-1, 40))) {
                    int n = 99;
                    WriteProcessMemory(hProc, (LPVOID)(local + 0x144), &n, 4, 0);
                }
            }
        }
        else {
            ImGui::TextColored(ImVec4(1, 1, 0, 1), "Waiting for ac_client.exe...");
        }

        ImGui::End();
        ImGui::EndFrame();

        g_pDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA(15, 15, 15, 255), 1.0f, 0);
        if (g_pDevice->BeginScene() >= 0) {
            ImGui::Render();
            ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
            g_pDevice->EndScene();
        }
        g_pDevice->Present(0, 0, 0, 0);
    }
    return 0;
}

bool init_d3d(HWND hWnd) {
    if ((g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == 0) return 0;
    D3DPRESENT_PARAMETERS p = { 0 };
    p.Windowed = 1; p.SwapEffect = D3DSWAPEFFECT_DISCARD; p.BackBufferFormat = D3DFMT_UNKNOWN;
    if (g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &p, &g_pDevice) < 0) return 0;
    return 1;
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam)) return 1;
    if (msg == WM_DESTROY) { PostQuitMessage(0); return 0; }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}