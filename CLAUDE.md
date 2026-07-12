# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## What this is

`kwin_glass` is a KWin effect plugin for KDE Plasma 6 — a fork of the stock blur effect adding refraction ("liquid glass"), force-blur, rounded corners, tinting/glow/edge-lighting, and per-region (window content / decoration / dock) blur strengths. It loads as a shared module into the KWin compositor; there is no standalone executable.

## Build & install

```bash
# Default Wayland build
cmake -S . -B build -DCMAKE_INSTALL_PREFIX=/usr
cmake --build build -j$(nproc)
sudo cmake --install build

# X11 target (requires KWin X11 dev packages)
cmake -S . -B build -DGLASS_X11=ON -DGLASS_WAYLAND=OFF

# All-in-one: syncs `upstream` remote if present, then configure/build/sudo make install
./install.sh

# Nix
nix develop          # dev shell with deps
nix build .#default  # Wayland package (.#x11 for X11)
```

There is **no unit-test suite**; `ctest --test-dir build` is the only registered check and is essentially empty. The real test is loading the effect in a running compositor — use `./tools/plasma-nested.sh wayland` to launch a nested Plasma session for manual verification. After install, the effect appears in System Settings as "Glass (Custom)" (disabled by default).

Requires Qt ≥ 6.6, KF6 ≥ 5.240, and KWin ≥ 6.4 dev files. Build fails hard below 6.4.

## Architecture

### Multi-version / multi-platform conditional compilation
The single biggest source of complexity. The same sources (`blur.cpp`, `settings.cpp`) compile against several KWin ABIs via preprocessor guards — `blur.cpp` has ~90 of them. When editing `blur.cpp`/`blur.h`, **every code path must stay valid under all four guard combinations**:

- `GLASS_WAYLAND` vs `GLASS_X11` — chosen by CMake options; sets `GLASS_X11` define. X11 and Wayland use different KWin types (`using BlurOutput = Output` / `RenderView`, `BlurRegion = QRegion` / `Region`), link different libs (`KWin::kwin` vs `KWinX11::kwin`), and install to `kwin/` vs `kwin-x11/` plugin dirs.
- `GLASS_KWIN_67` — set by CMake when KWin ≥ 6.7. Gates API that changed at 6.7 (and the absence of the Blur/Contrast manager D-Bus interfaces).
- `PLASMA_VERSION >= 0x0604xx` — finer-grained checks (from `<Plasma/plasma_version.h>`) for point-release API shifts.

Supported Plasma versions are **6.6 and 6.7** (see README). X11 support is frozen at an old commit.

### Shaders — generated, not authored, in the build tree
Author shaders only in `src/shaders/*.glsl`. CMake (`src/CMakeLists.txt`) does the rest at configure time:
1. `#include "oklab.glsl"`, `"glass.glsl"`, `"snells-glass.glsl"` are textually expanded (snell's is pre-inlined into glass.glsl).
2. Each source is emitted twice into `src/generated/` — a `_core` variant prefixed with `compat_core.glsl` (modern GLSL) and a legacy variant prefixed with `compat_legacy.glsl` (the `#define TEXTURE texture2D` / `attribute` / `varying` shims).

The `compat_*.glsl` preambles let one shader body run on both core and legacy GL contexts. Resources are referenced through `src/blur.qrc`; reference shaders via the qrc, never the generated paths directly. Note: `src/generated/` files are checked into git but are build outputs — regenerate via cmake rather than hand-editing.

### Settings flow
`src/blur.kcfg` (KConfigXT schema, group `Effect-blurplus-custom`) → `blurconfig.kcfgc` generates `BlurConfig` → `settings.cpp` reads it into the plain structs in `settings.h` (`GeneralSettings`, `ForceBlurSettings`, `RoundedCornersSettings`, `RefractionSettings`, bundled in `BlurSettings`). `blur.cpp` consumes `BlurSettings`. Settings are organized around named **profiles** (default `LiquidGlass`, via `ActiveProfile`).

Adding a setting means touching: `blur.kcfg` (schema) → the matching struct + `read()` in `settings.{h,cpp}` → consumption in `blur.cpp` → the KCM UI.

### KCM (settings UI)
`src/kcm/` builds `kwin_glass_custom_config`, a separate KCMUtils module loaded by System Settings. `blur_config.ui` is the Qt Designer form; `blur_config.cpp` wires it and talks to KWin over the `kwineffects` D-Bus interface (generated from `KWIN_EFFECTS_INTERFACE`) to reconfigure the live effect. The same KCM serves both Wayland and X11 (assumes identical D-Bus interface).

## Conventions

C++20, KDE/Qt style: 4-space indent, braces on same line, `m_` members / `s_` statics, `QStringLiteral`/`QByteArrayLiteral` for literals, SPDX headers on every C++ source. Conventional Commit subjects (`feat(blur):`, `fix(oklab):`, `chore:`). For visual/KCM changes, attach screenshots and record session type + GPU/driver + Wayland-or-X11 in the PR.

## graphify

This project has a knowledge graph at graphify-out/ with god nodes, community structure, and cross-file relationships.

Rules:
- For codebase questions, first run `graphify query "<question>"` when graphify-out/graph.json exists. Use `graphify path "<A>" "<B>"` for relationships and `graphify explain "<concept>"` for focused concepts. These return a scoped subgraph, usually much smaller than GRAPH_REPORT.md or raw grep output.
- If graphify-out/wiki/index.md exists, use it for broad navigation instead of raw source browsing.
- Read graphify-out/GRAPH_REPORT.md only for broad architecture review or when query/path/explain do not surface enough context.
- After modifying code, run `graphify update .` to keep the graph current (AST-only, no API cost).
