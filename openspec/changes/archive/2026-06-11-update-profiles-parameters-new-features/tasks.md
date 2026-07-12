## 1. Extend GlassPreset and Presets Definition

- [x] 1.1 Add `oklabSaturation` and `physicallyBasedRefraction` boolean fields to the `GlassPreset` struct in `src/kcm/blur_config.cpp`
- [x] 1.2 Update all presets in the `s_presets` map in `src/kcm/blur_config.cpp` to include values for the new fields (enabling both on `LiquidGlass`, and disabling both on `MacosLight`, `MacosDark`, `WindowsAcrylic`, and `FrostyGlass`)

## 2. Update Preset UI Logic and Signal Connections

- [x] 2.1 Update `applyPreset` in `src/kcm/blur_config.cpp` to set the checked state of `ui.kcfg_OklabSaturation` and `ui.kcfg_PhysicallyBasedRefraction` from the selected preset properties
- [x] 2.2 Add `connectControl` connections for `ui.kcfg_OklabSaturation` and `ui.kcfg_PhysicallyBasedRefraction` in `BlurEffectConfig` constructor to reset the active profile to "Custom" on manual toggles

## 3. Build and Verification

- [x] 3.1 Run `install.sh` to compile the changes and verify that the plugin builds successfully without compiler warnings or errors
- [ ] 3.2 Open KCM settings and verify that changing profiles correctly updates the new settings, and manual adjustments correctly change the profile selection to "Custom / Manual"
