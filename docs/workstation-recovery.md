# Workstation recovery

This repository targets CachyOS/Arch Linux with KDE Plasma 6. Restore it on a
new machine as follows.

## 1. Clone every preserved ref

```bash
git clone https://github.com/BluestYellow/kwin-effects-glass.git
cd kwin-effects-glass
git fetch --all --tags --prune
git branch -a
```

Read `AGENTS.md`, `CLAUDE.md`, and the main `README.md` before changing code.
The recovery branch created before the 2026-07 workstation reinstall is
`docs/workstation-recovery-20260712`.

The former local stash is preserved as
`origin/backup/preformat-20260712-stash`. Inspect it without applying it:

```bash
git log --graph --oneline origin/backup/preformat-20260712-stash -n 5
git diff origin/backup/preformat-20260712-stash^1 origin/backup/preformat-20260712-stash
```

If that work is needed, create a new branch from its first parent and apply the
merge commit explicitly:

```bash
git switch -c feature/recover-preformat-stash origin/backup/preformat-20260712-stash^1
git cherry-pick -m 1 origin/backup/preformat-20260712-stash
```

## 2. Install build dependencies

Use the current dependency list in the main README because CachyOS is a rolling
release. Install packages through Homebrew only where appropriate; KDE/Qt/KWin
development libraries must match the system and normally come from pacman.

## 3. Configure and verify

```bash
cmake -S . -B build -DCMAKE_INSTALL_PREFIX=/usr
cmake --build build -j"$(nproc)"
ctest --test-dir build --output-on-failure
```

For rendering changes, verify in a nested Wayland Plasma session:

```bash
./tools/plasma-nested.sh wayland
```

Only after a successful build should the operator install it:

```bash
sudo cmake --install build
```

The `build/`, local agent directories, and `graphify-out/` are intentionally not
versioned. Regenerate them locally. OpenSpec source artifacts under `openspec/`
are versioned and are the durable planning/specification record.
