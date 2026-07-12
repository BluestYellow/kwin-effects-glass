## Context

The `install.sh` script is the primary way for users to build and install the `kwin-effects-glass` plugin. Many users work from forks and need to keep their local `main` branch synchronized with the official repository to receive updates and fixes.

## Goals / Non-Goals

**Goals:**
- Automate the process of fetching and merging updates from the `upstream` remote.
- Maintain compatibility with the existing logging and error handling style of `install.sh`.
- Ensure the script remains usable even if the user has not configured an `upstream` remote.

**Non-Goals:**
- Automatically configuring the `upstream` remote if it's missing (this requires user-specific information).
- Handling complex merge conflicts automatically.
- Supporting multiple main branch names (will assume `main`).

## Decisions

### 1. Verification of Upstream Remote
**Decision:** Use `git remote | grep -q '^upstream$'` to check for the existence of the remote.
**Rationale:** This is a lightweight way to verify if the expected remote is configured before attempting network operations.
**Alternatives:** `git remote get-url upstream` (fails with error if missing, requiring redirection and exit code check).

### 2. Synchronization Strategy
**Decision:** Use `git fetch upstream` followed by `git merge upstream/main`.
**Rationale:** Standard way to bring in changes. `merge` is preferred over `rebase` in this context to keep the history simple for users who might have local changes they want to preserve alongside upstream updates.
**Alternatives:** `git pull upstream main` (shorthand, but less control over individual steps).

### 3. Error Handling for Network Issues
**Decision:** If `git fetch` fails, log a warning and continue.
**Rationale:** Network issues should not block a user from installing the version of the code they already have locally.

### 4. Error Handling for Merge Conflicts
**Decision:** If `git merge` fails (exit code != 0), the script will exit.
**Rationale:** A merge conflict puts the repository in an unstable state. Continuing the build/install process would likely fail or result in a broken installation.

## Risks / Trade-offs

- **[Risk]** Merge conflicts during `install.sh` execution. → **[Mitigation]** Provide clear instructions to the user on how to resolve the conflict manually.
- **[Risk]** Slow installation due to network overhead. → **[Mitigation]** Synchronization happens only once at the start, and `git fetch` is generally fast.
- **[Risk]** `upstream/main` might not be the correct branch name for all forks. → **[Mitigation]** Standardize on `main` as it is the project's default.
