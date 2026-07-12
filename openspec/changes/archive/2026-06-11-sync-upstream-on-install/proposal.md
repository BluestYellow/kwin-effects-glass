## Why

Currently, the `install.sh` script only handles the build and installation process. Users who have forked the repository need to manually pull updates from the official upstream to stay up-to-date. Automating this step ensures that every installation or update uses the latest official code, reducing drift between the fork and the original project.

## What Changes

- Modified `install.sh` to include a synchronization step at the beginning of the execution.
- The script will fetch updates from the `upstream` remote.
- The script will merge the latest changes from `upstream/main` into the current branch.

## Capabilities

### New Capabilities
- `upstream-sync`: Automatically synchronize the local repository with the official upstream repository during the installation process.

### Modified Capabilities
<!-- No existing capabilities found in openspec/specs -->

## Impact

- `install.sh`: The main entry point for installation will have a new dependency on git connectivity and the presence of an `upstream` remote.
- Git configuration: Users will need to have an `upstream` remote configured for the synchronization to work (though the script can handle its absence gracefully).
