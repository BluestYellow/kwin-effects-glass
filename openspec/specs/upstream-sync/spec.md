# Upstream Synchronization

## Purpose
Ensure that local installations and updates of `kwin-effects-glass` are synchronized with the official upstream repository to reduce drift and provide the latest improvements automatically.

## Requirements

### Requirement: Upstream remote synchronization
The `install.sh` script SHALL attempt to synchronize the local repository with the official upstream repository before starting the build process.

#### Scenario: Successful synchronization
- **WHEN** the `upstream` remote exists and there are new changes on `upstream/main` that can be fast-forwarded or merged cleanly
- **THEN** the script fetches from `upstream`, merges `upstream/main` into the current branch, and continues with the installation

#### Scenario: Missing upstream remote
- **WHEN** the `upstream` remote does not exist in the git configuration
- **THEN** the script logs an informative message stating that synchronization is skipped and continues with the installation

#### Scenario: Merge conflict during synchronization
- **WHEN** a merge conflict occurs while merging `upstream/main`
- **THEN** the script stops execution, logs an error message explaining the conflict, and instructs the user to resolve it manually before running the script again

#### Scenario: Offline or unreachable remote
- **WHEN** the script is unable to fetch from `upstream` due to network issues or remote unavailability
- **THEN** the script logs a warning message and continues with the installation using the local code
