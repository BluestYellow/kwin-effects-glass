# Glass Rendering Alignment

## Purpose
Ensure glass rendering effects align precisely with window boundaries and that refraction offsets scale correctly.

## Requirements

### Requirement: Correct Local Coordinates Calculation in Shaders
The glass rendering shaders SHALL calculate local coordinates (`position`, `halfBlurSize`, `blurSize`) relative to the actual window boundaries (`box`) and fragment screen position (`vertex`), instead of utilizing global normalized texture coordinates (`uv`).

#### Scenario: Alignment with Window Frame
- **WHEN** rendering a window frame with decoration or custom region size
- **THEN** the local glass position and size match the actual window boundaries exactly

### Requirement: Scaled Refraction Offset
The glass refraction shader SHALL scale the refraction offset (which is defined in pixels) into normalized texture coordinates by multiplying it by twice the `halfpixel` uniform before applying it to the texture coordinates.

#### Scenario: Refraction Scale Correction
- **WHEN** the refraction effect is active
- **THEN** the texture offset is scaled down to normalized coordinates and shifts sampling by the configured number of physical pixels
