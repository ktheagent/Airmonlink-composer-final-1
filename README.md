# Airmonlink Composer Native

Clean native reconstruction. The former Electron renderer is not imported.

## Verified foundation contract

- One semantic `ScoreModel`
- Exact rational musical timing
- Stable semantic note identities through undo and redo
- One `CoordinateMapper`
- One `NoteInputController` for preview and commit
- Preview and committed note share the same mapped `PointerTarget`
- Voice-coloured, nonprinting ghost note
- Atomic conflict rejection
- Deterministic text persistence fixture
- Versioned plugin manifest and permission validation
- Qt 6 desktop surface when Qt is available

## Write workspace checkpoint

The current visible application checkpoint provides one compact `Write` workspace:

- SMuFL note-value symbols for whole through sixty-fourth notes
- V1–V4 voice controls using the specified editing colours
- Shared registered actions across the top workspace, docked keypad, and keyboard shortcuts
- Plain-language tooltips and accessible names
- A right/bottom dockable keypad that does not overlay the score
- Persistent active Write command and keypad position
- Ghost preview refresh when duration or voice changes
- UI tests for organization, accessibility, shared actions, keyboard equivalence, and non-overlap

Controls not backed by implemented semantic commands remain absent. Automatic harmonization and arranging are deferred to a future separately installed plugin and are not core features.

## Build

```sh
cmake -S . -B build -G Ninja
cmake --build build
ctest --test-dir build --output-on-failure
```

Qt 6.5+ is required for the desktop target. The native core is standard C++20 and can be tested without Qt.
