## Why

The local repository was recently updated and merged with the official upstream repository. This merge introduced new features: perceptual color saturation using OKLab space, and physically-based refraction (using Snell's law). Currently, the preset profiles (e.g., Liquid Glass, macOS Light, macOS Dark, Windows Acrylic, Frosty Glass) do not support or set parameters for these new features. When a user switches profiles, these new settings are not updated, leading to an inconsistent configuration state.

## What Changes

- Update the `GlassPreset` struct and `s_presets` map in the KControl Module (KCM) to include options for the new features:
  - `oklabSaturation`: Whether to apply saturation in the OKLab color space (bool).
  - `physicallyBasedRefraction`: Whether to use physically-based refraction (bool).
- Update the default presets with appropriate settings for the new features (e.g., enabling physically-based refraction and OKLab saturation on the "Liquid Glass" profile).
- Update the KCM config initialization and control mapping to handle changes for `OklabSaturation` and `PhysicallyBasedRefraction`.
- Ensure manually toggling the new checkboxes switches the active profile back to "Custom".

## Capabilities

### New Capabilities
- `preset-profiles`: Support configuring OKLab saturation and physically-based refraction when preset profiles are changed.

### Modified Capabilities
<!-- No requirement changes to existing capabilities -->

## Impact

- `src/kcm/blur_config.cpp`: Update `GlassPreset` struct, `s_presets` values, `applyPreset` implementation, and control change signal connections.
- User Experience: Profiles will now correctly configure all features, preventing inconsistencies when switching presets.
