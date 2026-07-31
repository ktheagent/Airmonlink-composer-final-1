# Milestone 0 Gate Status

**Gate status: IN PROGRESS — NOT ACCEPTED**

## Verified in the current foundation

- Repository source matches the uploaded Native Foundation 0.1.0 source archive.
- The source archive itself is excluded from the repository; only its SHA-256 checksum is stored.
- The standard C++20 core configures and compiles locally with GCC 14.
- The current core test executable passes.
- The project has one `ScoreModel`, one `CoordinateMapper`, and one `NoteInputController`.
- Preview and commit use the same mapped pointer target in the current vertical-slice implementation.

## Added by the first Milestone 0 checkpoint

- Windows GitHub Actions workflow for the real Qt application and tests.
- Mandatory Qt discovery when the desktop target is enabled.
- Embedded application version and source commit identity.
- Windows deployment staging and portable ZIP artifact creation.
- A packaged executable launch smoke check.
- Dependency/licence inventory and generated-file exclusions.

## Still missing or unverified

- A completed Windows workflow result for the checkpoint commit.
- Installer output and installer test.
- Qt packaged-GUI interaction tests.
- Crash logging and recovery.
- Static analysis and formatting enforcement.
- Application icon/resource system.
- SMuFL font integration and licence.
- Clean-machine launch test outside the GitHub-hosted runner.
- Physical MIDI, audio, printer, accessibility, multiple-monitor, and high-DPI testing.

A green build will validate compilation, tests, deployment staging, identity output, and a short offscreen launch smoke check only. It will not establish that Milestone 0 or the full product is complete.
