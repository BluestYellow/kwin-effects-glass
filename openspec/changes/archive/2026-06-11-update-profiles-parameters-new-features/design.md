## Context

The KWin Glass Effect provides preset profiles (e.g., Liquid Glass, macOS Light, macOS Dark, Windows Acrylic, Frosty Glass) implemented in the KCM configuration utility. Recent upstream updates introduced two new settings:
1. `OklabSaturation` (`Apply saturation perceptually (OKLab)`)
2. `PhysicallyBasedRefraction` (`Physically-based refraction`)

Currently, these settings are not defined in the `GlassPreset` structure or its static presets map `s_presets` in `src/kcm/blur_config.cpp`. Consequently, switching profiles does not reset or configure these new checkboxes.

## Goals / Non-Goals

**Goals:**
- Extend the `GlassPreset` struct to support the new features.
- Update the static `s_presets` mapping to specify settings for each profile.
- Synchronize preset applications to toggle the new UI checkboxes appropriately.
- Connect the new checkbox state-change signals to trigger the "Custom / Manual" profile selection when modified manually.

**Non-Goals:**
- Creating new preset profiles.
- Modifying the underlying shaders or configuration storage format.

## Decisions

### 1. Extend the `GlassPreset` Struct
We will add two fields to the `GlassPreset` structure:
```cpp
struct GlassPreset {
    ...
    bool oklabSaturation;
    bool physicallyBasedRefraction;
};
```
*Rationale*: This is the simplest way to extend the preset schema.

### 2. Update Default Preset Parameters
We will define values for the new settings for each preset:
- **Liquid Glass**: `oklabSaturation = true`, `physicallyBasedRefraction = true`.
- **macOS Light**: `oklabSaturation = false`, `physicallyBasedRefraction = false`.
- **macOS Dark**: `oklabSaturation = false`, `physicallyBasedRefraction = false`.
- **Windows Acrylic**: `oklabSaturation = false`, `physicallyBasedRefraction = false`.
- **Frosty Glass**: `oklabSaturation = false`, `physicallyBasedRefraction = false`.

*Rationale*: "Liquid Glass" is designed to show off the most premium, high-fidelity glass-like refraction features, so enabling physically-based Snell's refraction and perceptual OKLab saturation fits its design language. The other presets mimic existing operating system designs that do not use these features, so we keep them disabled to preserve their visual styles.

### 3. Connect UI Control Signals
We will map the new checkbox toggled signals:
```cpp
connectControl(ui.kcfg_OklabSaturation, &QCheckBox::toggled);
connectControl(ui.kcfg_PhysicallyBasedRefraction, &QCheckBox::toggled);
```
*Rationale*: Ensures that manually changing these values correctly moves the profile combo box to "Custom / Manual".

## Risks / Trade-offs

- **Risk**: Adding fields to `GlassPreset` requires modifying all list initializers for `s_presets`.
  - *Mitigation*: Ensure all initializers are updated correctly in C++.
