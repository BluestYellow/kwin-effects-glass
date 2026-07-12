## Context

Currently, the KWin glass and blur effects calculate local position and size coordinates for glass rendering using `uv` (normalized texture coordinates) and `blurSize`. Since the background texture (`texUnit`) is larger than the window frame (due to blur expansion and margin allocations), `uv` does not scale correctly to the window frame. This causes visual glass features (such as refraction, edge highlights, and glow) to be distorted and shifted, while the window clipping is done correctly via `sdfRoundedBox` using `vertex` and `box`. This misalignment generates visual artifacts in the corners of transparent or decorated windows. Furthermore, the refraction offset is not scaled to normalized coordinate space, resulting in massive, out-of-bounds sampling displacements.

## Goals / Non-Goals

**Goals:**
- Correct coordinate mapping for glass rendering inside `glass.glsl` and `onscreen_rounded.glsl` to align glass highlights and borders with the window frame boundaries.
- Correct the scaling of refraction offsets to translate pixel distances into normalized texture coordinates using the `halfpixel` uniform.

**Non-Goals:**
- Modify the Dual Kawase desampling/upsampling passes.
- Modify KWin settings or user presets configuration.

## Decisions

### 1. Derive local window coordinates from `box` and `vertex` in the shaders
- **Choice**: Instead of calculating local position as `uv * blurSize - halfBlurSize`, use:
  ```glsl
  vec2 halfBlurSize = box.zw;
  vec2 position = vertex - box.xy;
  ```
- **Rationale**: `box.xy` holds the center of the window frame and `box.zw` holds the half-size of the window frame. `vertex` contains the fragment coordinate in the same coordinate space. Calculating `vertex - box.xy` yields the exact offset of the fragment from the center in physical pixels.
- **Alternatives Considered**: Modifying the VBO coordinates. However, VBO vertex coordinates are shared across multiple rendering paths (including downsampling passes), so modifying them would break downsampling/upsampling.

### 2. Correct `roundedRectangle` coordinate input
- **Choice**: Call the `roundedRectangle` function inside `glass` as:
  ```glsl
  roundedRectangle(vertex - (box.xy - box.zw), tinted, cornerRadius)
  ```
- **Rationale**: `roundedRectangle` expects coordinates ranging from `0` to `blurSize` relative to the top-left of the window frame. `vertex - (box.xy - box.zw)` translates the fragment coordinate to exactly this space.

### 3. Scale refraction offset by `2.0 * halfpixel`
- **Choice**: Multiply the calculated refraction offset by `2.0 * halfpixel` before subtracting it from `uv`:
  ```glsl
  vec2 refractOffset = -normal.xy * finalStrength * 2.0 * halfpixel;
  ```
- **Rationale**: `halfpixel` represents the size of half a pixel in normalized coordinate space (`0.5 / width`). Therefore, `2.0 * halfpixel` represents the size of exactly one pixel in normalized coordinates. This translates pixel-based refraction strength configurations into correct UV coordinates.

## Risks / Trade-offs

- **[Risk]** Shader syntax errors or compatibility failures with legacy/core profile variants.
  - **Mitigation**: The changes only use variables (`box`, `vertex`, `halfpixel`) that are already declared and bound in both core and legacy fragment shaders.
- **[Risk]** Slightly different visual behavior of refraction and edge effects.
  - **Mitigation**: This change fixes a regression where refraction was essentially zeroed-out or extremely clamped due to coordinate misalignment. The new behavior represents the intended visual rendering of "Liquid Glass".
