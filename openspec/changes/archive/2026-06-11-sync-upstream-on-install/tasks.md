## 1. Implement Upstream Synchronization

- [x] 1.1 Add `sync_upstream` function to `install.sh` that checks for the `upstream` remote.
- [x] 1.2 Implement `git fetch upstream` with error handling for network issues within the function.
- [x] 1.3 Implement `git merge upstream/main` with error handling for merge conflicts within the function.
- [x] 1.4 Add a call to `sync_upstream` at the beginning of the `install.sh` script, after setting up logging and directories.

## 2. Verification

- [x] 2.1 Verify that the script continues normally if the `upstream` remote is missing.
- [x] 2.2 Verify that the script successfully fetches and merges when `upstream` is present and clean.
- [x] 2.3 Verify that the script stops and reports an error if a merge conflict occurs.
- [x] 2.4 Verify that the script continues with a warning if the remote is unreachable.
