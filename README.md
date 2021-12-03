## Setup

## Project config
- Consists of 2 projects, loader & dll.
- Manifest for Loader: require admin (`Properties -> Linker -> Manifest File -> UAC Execution Level`)
- DLL will be automatically copied over next to loader EXE

## DLL Debugging
- Make sure debugger is attached to Game (`Properties -> Debugging`)
	- Attach: Yes
	- Command: Select game EXE
- `Linker -> Debugging -> Debuggable Assembly`: Yes
- Attach debugger after DLL is injected:
	- Right click "Trainer" project, "Debug" and "Step into new instance"
- https://docs.microsoft.com/en-us/visualstudio/debugger/debugging-dll-projects?view=vs-2022
- https://docs.microsoft.com/en-us/visualstudio/debugger/debug-multiple-processes?view=vs-2022

## Issues
- Special characters support - making sure to use wstring
- Console works fine for proof of concept, however, closing console closes game thread
- https://stackoverflow.com/questions/11959643/why-does-closing-a-console-that-was-started-with-allocconsole-cause-my-whole-app
- ImGui should work, copy/paste.
	- https://www.reedbeta.com/made/git-partial-submodule/
	- Properties -> C / C++ -> General -> Additional Include Directories -> add "imgui"
	- Disable precompiled headers! https://github.com/ocornut/imgui/issues/2781
	- Add d3d11.lib https://stackoverflow.com/questions/16182175/d3d11createdeviceandswapchain-unresolved-external/18089518
- Hooking keyboard events
	- Imgui is not receiving keyboard events when in game, and if it does - it also passes those events to game
	- Trainer should prevent game from recieving keyboard events
	- https://stackoverflow.com/questions/33024712/call-setwindowshookex-with-method-defined-in-header-file/33028459#33028459
	- https://docs.microsoft.com/en-us/windows/win32/inputdev/using-raw-input