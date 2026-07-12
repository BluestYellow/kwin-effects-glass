## 1. Shader Updates

- [x] 1.1 Update `src/shaders/glass.glsl` to compute `halfBlurSize` and `position` using `box` and `vertex`.
- [x] 1.2 Update `src/shaders/glass.glsl` to scale the refraction offset using `2.0 * halfpixel`.
- [x] 1.3 Update `src/shaders/glass.glsl` to call `roundedRectangle` with coordinates local to the window (`vertex - (box.xy - box.zw)`).
- [x] 1.4 Update `src/shaders/onscreen_rounded.glsl` to align the blur sampling condition `dist <= 0.0` with the corrected window bounds.

## 2. Build and Verification

- [x] 2.1 Rebuild the project using CMake to generate the core and legacy shader variants.
- [x] 2.2 Verify that the shader changes compile correctly and eliminate the alignment/refraction artifacts.
