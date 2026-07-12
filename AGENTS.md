# Repository Guidelines

## Project Structure & Module Organization

This repository builds `kwin_glass`, a KDE Plasma 6 KWin effect. Core effect code lives in `src/` (`blur.cpp`, `settings.cpp`, headers, KConfig files, resources, and `metadata.json`). The settings module is in `src/kcm/`, and GLSL sources are in `src/shaders/`; CMake generates core and legacy shader variants into the build tree. User-facing screenshots and configuration notes are in `docs/`. Nix packaging lives in `flake.nix` and `nix/`. Translations are managed through `Messages.sh` and `po/`.

## Build, Test, and Development Commands

- `cmake -S . -B build -DCMAKE_INSTALL_PREFIX=/usr` configures the default Wayland build.
- `cmake -S . -B build -DGLASS_X11=ON -DGLASS_WAYLAND=OFF` configures the X11 target when compatible KWin X11 packages are installed.
- `cmake --build build -j$(nproc)` compiles the plugin and KCM module.
- `ctest --test-dir build --output-on-failure` runs CMake-registered checks when available.
- `./install.sh` syncs an `upstream` remote if present, configures, builds, and installs with `sudo make install`.
- `nix develop` enters a shell with package dependencies; `nix build .#default` builds the Wayland package.
- `./tools/plasma-nested.sh wayland` starts a nested Plasma session for manual verification.

## Coding Style & Naming Conventions

Use C++20 with KDE/Qt conventions already present in `src/`: 4-space indentation, braces on the same line, `m_` member prefixes, `s_` static prefixes, `QStringLiteral`/`QByteArrayLiteral` for literals, and Qt/KWin signal-slot patterns. Keep SPDX headers on C++ sources. Keep shader helpers in `src/shaders/` and add resources through `blur.qrc` rather than referencing generated files directly.

## Testing Guidelines

There is no dedicated unit-test tree in this checkout. Treat successful CMake configure, build, and `ctest` as the baseline. For rendering or settings changes, also test in a nested Plasma session and record the session type, GPU/driver if relevant, and whether Wayland or X11 was used.

## Commit & Pull Request Guidelines

Recent history uses concise imperative subjects, often Conventional Commit style such as `feat(blur): ...`, `fix(oklab): ...`, and `chore: ...`. Keep commits focused and mention the affected area. Pull requests should include a short behavior summary, build/test commands run, linked issues when applicable, and screenshots or short recordings for visual effect or KCM UI changes.

## Agent-Specific Instructions

When using shell commands in this workspace, prefix commands with `rtk` where practical. For codebase questions, use `graphify query "<question>"` first when `graphify-out/graph.json` exists, then update the graph after code changes with `graphify update .`.

## graphify

This project has a knowledge graph at graphify-out/ with god nodes, community structure, and cross-file relationships.

When the user types `/graphify`, invoke the `skill` tool with `skill: "graphify"` before doing anything else.

Rules:
- For codebase questions, first run `graphify query "<question>"` when graphify-out/graph.json exists. Use `graphify path "<A>" "<B>"` for relationships and `graphify explain "<concept>"` for focused concepts. These return a scoped subgraph, usually much smaller than GRAPH_REPORT.md or raw grep output.
- Dirty graphify-out/ files are expected after hooks or incremental updates; dirty graph files are not a reason to skip graphify. Only skip graphify if the task is about stale or incorrect graph output, or the user explicitly says not to use it.
- If graphify-out/wiki/index.md exists, use it for broad navigation instead of raw source browsing.
- Read graphify-out/GRAPH_REPORT.md only for broad architecture review or when query/path/explain do not surface enough context.
- After modifying code, run `graphify update .` to keep the graph current (AST-only, no API cost).
