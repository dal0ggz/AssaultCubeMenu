# AssaultCubeMenu

Small C++ Win32 overlay menu built with Direct3D 9 and Dear ImGui.

This started as a compact Visual Studio project for experimenting with a custom
menu, a D3D9 render loop, and basic Windows process/module lookup code. The
whole custom app is intentionally kept small, with most of the logic in
`AssaultCubeMenu.cpp`.

## Features

- Win32 overlay window.
- Direct3D 9 rendering.
- Dear ImGui menu with tabs and checkboxes.
- Simple `AppState` struct for runtime state.
- Process and module lookup helpers.
- Vendored ImGui source files.
- Visual Studio solution and project files.

## Tech Stack

- C++20
- Win32 API
- Direct3D 9
- Dear ImGui
- Visual Studio

## Project Structure

```text
.
|-- AssaultCubeMenu.slnx
|-- AssaultCubeMenu/
|   |-- AssaultCubeMenu.cpp
|   |-- AssaultCubeMenu.vcxproj
|   |-- AssaultCubeMenu.vcxproj.filters
|   `-- imgui/
|       |-- imgui.cpp
|       |-- imgui_draw.cpp
|       |-- imgui_tables.cpp
|       |-- imgui_widgets.cpp
|       |-- imgui_impl_dx9.cpp
|       |-- imgui_impl_win32.cpp
|       `-- ...
|-- .gitattributes
|-- .gitignore
`-- README.md
```

## Main Files

| Path | Description |
| --- | --- |
| `AssaultCubeMenu/AssaultCubeMenu.cpp` | Main app file with Win32 setup, Direct3D 9 setup, ImGui frame loop, and menu UI. |
| `AssaultCubeMenu/imgui/` | Vendored Dear ImGui source and Win32/DX9 backends. |
| `AssaultCubeMenu/AssaultCubeMenu.vcxproj` | Visual Studio project configuration. |
| `AssaultCubeMenu.slnx` | Visual Studio solution file. |
| `.gitignore` | Ignores Visual Studio output, cache files, and local ImGui state. |

## Build Notes

This is a Windows-only Visual Studio C++ project.

General build setup:

1. Open `AssaultCubeMenu.slnx` in Visual Studio.
2. Install the Desktop development with C++ workload if it is missing.
3. Use an x64 or Win32 configuration depending on the target setup.
4. Build from Visual Studio.

The project links against `d3d9.lib` and uses the vendored ImGui Win32/DX9
backend files already included in the repository.

## Repository Notes

- `imgui.ini` is ignored because it only stores local UI/window state.
- Build folders such as `Debug/`, `Release/`, `x64/`, and `x86/` are ignored.
- Compiler outputs such as `.obj`, `.pdb`, `.ipdb`, and `.log` are ignored.
- ImGui is vendored directly in the repository.

## Screenshots

No screenshots are included yet.

## TODO

- Add the exact ImGui version used by the project.
- Split the app into smaller files if the menu grows.
- Add basic error logging for Direct3D initialization.
- Add a consistent formatting style for the C++ code.
- Add a license if the project is meant to be reused by other people.
