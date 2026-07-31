# Airmonlink Composer Native

Clean native reconstruction. The former Electron renderer is not imported.

## Foundation contract

- One semantic `ScoreModel`
- One `CoordinateMapper`
- One `NoteInputController` for preview and commit
- Preview and committed note share the same mapped `PointerTarget`
- Voice-coloured, nonprinting ghost note
- Atomic conflict rejection
- Undo/redo
- Deterministic text persistence fixture
- Qt 6 desktop surface when Qt is available

## Build

```sh
cmake -S . -B build -G Ninja
cmake --build build
ctest --test-dir build --output-on-failure
```

Qt 6.5+ is required for the desktop target. The native core is standard C++20 and can be tested without Qt.
