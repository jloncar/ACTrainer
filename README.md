## Project config
- Consists of 2 projects, loader & dll.
- Manifest for Loader: require admin (Properties -> Linker -> Manifest File -> UAC Execution Level)
- DLL will be automatically copied over next to loader EXE

## DLL Debugging
- Make sure debugger is attached to Game (Properties -> Debugging)
-- Attach: Yes
-- Command: Select game EXE
- Linker -> Debugging -> Debuggable Assembly: Yes
- Attach debugger after DLL is injected:
-- Right click "Trainer" project, "Debug" and "Step into new instance"

## Issues
- Special characters support - making sure to use wstring
- Console works fine for proof of concept, however, closing console closes game thread
- https://stackoverflow.com/questions/11959643/why-does-closing-a-console-that-was-started-with-allocconsole-cause-my-whole-app
- ImGui should work, but copy/pasting won't. Too large repo for submodule.
-- https://www.reedbeta.com/made/git-partial-submodule/