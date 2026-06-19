# AssaultCubeMenu

## Repository Status

This repository is preserved as an archival C++ / Win32 experiment. It is not a
recommended portfolio highlight in its current form because it combines an ImGui
overlay with process memory editing logic for a game.

Do not use this project to gain an unfair advantage in a game or to modify
software without permission. The useful learning value is limited to the
transferable engineering pieces: Win32 window creation, Direct3D 9 rendering,
ImGui integration, Visual Studio project structure, and repository hygiene.

## Overview

`AssaultCubeMenu` is a compact Visual Studio C++ project that demonstrates a
single-file Windows overlay application using Direct3D 9 and Dear ImGui. The
main source file contains the application state, Win32 process lookup helpers,
window procedure, Direct3D initialization, ImGui frame lifecycle, and menu UI.

The repository is best treated as a historical learning snapshot rather than a
maintained application. For a professional portfolio, the UI/rendering approach
should be rebuilt as a harmless desktop tool or graphics demo that does not read
or write another process.

## Technical Highlights

- Win32 overlay window setup.
- Direct3D 9 device initialization and render loop.
- Dear ImGui context setup and tabbed UI surface.
- Small application-state struct for runtime flags and connection state.
- Visual Studio project files targeting C++20.
- Vendored ImGui source files for a self-contained experiment.
- Repository cleanup for local ImGui state and generated build files.

## What This Project Demonstrates

From a safe engineering perspective, this repository demonstrates:

- how to create a Win32 window and route messages through a window procedure;
- how to initialize a Direct3D 9 device for UI rendering;
- how to integrate ImGui into a Windows desktop loop;
- how to organize a small prototype around a central `AppState` struct;
- why local UI state such as `imgui.ini` should be ignored instead of committed;
- why prototype code should be separated from portfolio-ready projects.

It should not be presented as a polished application, a production-quality
architecture, or a professional gameplay tool.

## Tech Stack

| Area | Technology |
| --- | --- |
| Language | C++20 |
| Platform | Windows |
| IDE / build files | Visual Studio solution and project files |
| Graphics | Direct3D 9 |
| UI | Dear ImGui |
| System APIs | Win32 API, Tool Help snapshots |
| Dependency style | Vendored ImGui source files |

## Project Structure

```text
.
|-- AssaultCubeMenu.slnx
|-- AssaultCubeMenu/
|   |-- AssaultCubeMenu.cpp            # Main application, Win32 loop, D3D9, ImGui UI
|   |-- AssaultCubeMenu.vcxproj        # Visual Studio C++ project
|   |-- AssaultCubeMenu.vcxproj.filters
|   `-- imgui/                         # Vendored Dear ImGui sources/backends
|-- .gitattributes
|-- .gitignore
`-- README.md
```

## Source Map

| File or directory | Purpose |
| --- | --- |
| `AssaultCubeMenu/AssaultCubeMenu.cpp` | Contains the prototype application state, process lookup helpers, window procedure, Direct3D 9 setup, ImGui frame loop, and UI layout. |
| `AssaultCubeMenu/imgui/` | Vendored Dear ImGui implementation and Win32/DX9 backends. |
| `AssaultCubeMenu/AssaultCubeMenu.vcxproj` | Visual Studio C++ project definition with Debug/Release and Win32/x64 configurations. |
| `AssaultCubeMenu.slnx` | Solution file referencing the project. |
| `.gitignore` | Visual Studio, build output, and local UI state ignore rules. |

## Installation

This repository does not provide supported installation instructions.

The current project includes game-specific process memory editing behavior, so
operational setup instructions are intentionally not included. To showcase the
same technical skills professionally, rebuild the project as a standalone ImGui
desktop utility with mock data and no external process interaction.

## Usage

No supported usage workflow is provided.

For portfolio purposes, use this repository only as a record of early Windows
graphics/UI experimentation. A public-facing version should keep the Direct3D 9
and ImGui learning goals while removing game-specific memory editing behavior.

## Screenshots / Demo

No screenshots or demos are included. A demo would be appropriate only after the
project is converted into a harmless standalone UI or graphics sample.

## Repository Hygiene

The repository includes a Visual Studio `.gitignore` and now ignores local ImGui
window state:

- `.vs/` IDE cache files;
- `Debug/`, `Release/`, `x64/`, and `x86/` build outputs;
- compiler and linker artifacts such as `.obj`, `.pdb`, `.ipdb`, and `.log`;
- local UI state such as `imgui.ini`.

The previously tracked `AssaultCubeMenu/imgui.ini` file was removed because it
stores local window positions and should not be part of source control.

## Quality Notes

- The project is a compact prototype, not a layered application.
- Most custom logic lives in one source file.
- The code uses global Direct3D device pointers.
- Direct3D and Win32 API calls need stronger failure handling.
- ImGui is vendored without a documented version.
- There are no automated tests, linters, formatters, or CI workflows.
- No license file is present.

## Known Issues

- The source is tightly coupled to a game-specific process name and memory
  layout.
- Hard-coded values make the prototype brittle and difficult to maintain.
- The render loop and application state are not separated into reusable modules.
- Error recovery is minimal if device creation or process access fails.
- The repository is not suitable as a highlighted public portfolio project.

## Future Improvements

Recommended direction if the goal is a professional portfolio:

- Archive or make this repository private.
- Build a new standalone ImGui desktop sample with harmless mock data.
- Split window creation, rendering, UI, and application state into separate
  files.
- Document the ImGui version and update process.
- Add a formatting configuration.
- Add a small CI workflow that verifies the project file can be parsed or built
  in a supported environment.
- Add screenshots only after the project becomes a benign UI demo.
- Add an explicit license if the code is intended for reuse.

## Portfolio Recommendation

Do not feature this repository as one of the main projects on a resume or GitHub
profile. If it stays public, keep the archival README and avoid presenting it as
a usable tool.

Better portfolio alternatives:

- a standalone ImGui settings panel;
- a Direct3D 9 rendering sandbox with synthetic data;
- a small Win32 utility with documented architecture;
- a C++ desktop app with clean separation between platform, rendering, and UI.

## License

No license file is currently present. Without a license, the code should be
treated as all-rights-reserved by default. Add an explicit license before reuse,
redistribution, or collaboration.
