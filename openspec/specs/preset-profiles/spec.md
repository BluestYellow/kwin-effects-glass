# Preset Profiles

## Purpose
Manage preset configuration profiles within the KCM (KDE Control Module) for the glass effect, enabling users to easily switch between predefined settings or customize them.

## Requirements

### Requirement: Preset profile synchronization for new features
The configuration module (KCM) SHALL update and synchronize the settings for OKLab saturation (`OklabSaturation`) and physically-based refraction (`PhysicallyBasedRefraction`) when preset profiles are changed.

#### Scenario: Selecting Liquid Glass preset
- **WHEN** the user selects the "Liquid Glass" profile in the active profile dropdown
- **THEN** the system SHALL enable both "OklabSaturation" and "PhysicallyBasedRefraction" settings

#### Scenario: Selecting macOS or Windows presets
- **WHEN** the user selects "macOS Light", "macOS Dark", "Windows Acrylic", or "Frosty Glass" in the active profile dropdown
- **THEN** the system SHALL disable the "PhysicallyBasedRefraction" setting, and set the "OklabSaturation" setting to false

#### Scenario: Modifying settings manually
- **WHEN** the active profile is a preset, and the user manually toggles either the "Apply saturation perceptually (OKLab)" or "Physically-based refraction" checkbox
- **THEN** the system SHALL change the active profile selection to "Custom / Manual"
