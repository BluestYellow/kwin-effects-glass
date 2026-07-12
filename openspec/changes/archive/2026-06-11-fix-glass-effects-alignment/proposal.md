## Why

Currently, the glass and blur shaders calculate local window coordinates and distance fields using the global texture coordinates (`uv`). Because the background texture (`texUnit`) is larger than the window frame (including blur expansion and margins), the glass visual effects (refraction, edge highlights, glow) are misaligned and scaled incorrectly relative to the window geometry. Furthermore, the refraction offset is specified in pixels but subtracted directly from normalized texture coordinates without scaling, causing massive shifts that clamp to the texture boundaries and create smeared transparent artifacts in the screen/window corners.

This change corrects the coordinate calculations to align all glass effects perfectly with the window boundaries and scales the refraction offset to normalized texture coordinates.

## What Changes

- **Fix Glass Coordinate Alignment**: Recompute local coordinates (`position`, `halfBlurSize`, `blurSize`) inside `glass.glsl` and `onscreen_rounded.glsl` using the uniform `box` and the varying `vertex` instead of `uv` and the uncorrected `blurSize`.
- **Fix Refraction Offset Scaling**: Multiply the pixel-based refraction offset by `2.0 * halfpixel` inside `glass.glsl` before offsetting `uv` for sampling.
- **Align Blur Region Sampling**: Use the corrected `dist` calculation in `onscreen_rounded.glsl` to sample the blurred texture only within the bounds of the window frame.

## Capabilities

### New Capabilities

- `glass-rendering-alignment`: Ensures that glass shaders calculate local coordinates, rounded rectangle distances, edge lighting, and refraction offsets relative to the window's physical boundaries rather than the global background texture bounds.

### Modified Capabilities

None.

## Impact

- `src/shaders/glass.glsl`: Coordinates mapping, rounded rectangle distance calculations, refraction offset computation, and glow highlights.
- `src/shaders/onscreen_rounded.glsl`: Fragment shader main loop, blur sampling boundaries.
