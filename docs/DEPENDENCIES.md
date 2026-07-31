# Dependency and Licence Inventory

Status: **Milestone 0 foundation inventory — packaging gate not yet complete**

## Direct build/runtime dependencies

| Dependency | Minimum / pinned version | Purpose | Licence / distribution note |
|---|---:|---|---|
| C++ compiler | C++20; MSVC 2022 on Windows CI | Native application and core library | Toolchain component; not redistributed as application source |
| CMake | 3.24 minimum | Configure, build, test and package | BSD 3-Clause |
| Qt | 6.5 minimum; Windows CI pins 6.8.3 | Widgets, GUI and native Windows application | Available under LGPLv3/GPLv3/commercial terms. Final packaging must satisfy the selected Qt licence and include required notices/source-offer material where applicable. |
| GitHub Actions checkout | v4 | Source checkout in CI | MIT |
| GitHub Actions upload-artifact | v4 | Test and portable artifact upload | MIT |
| install-qt-action | v4 | Install Qt in Windows CI | MIT; wraps Qt acquisition and does not change Qt's licence obligations |

## Bundled assets

No music font, audio library, MIDI library, installer runtime, or third-party artwork is bundled in Foundation 0.1.0.

## Release blockers

A professional SMuFL music font and its licence are not yet integrated. Installer technology, runtime redistribution, complete licence notices, and a dependency lock/verification policy remain required before a release candidate may be described as distributable.
