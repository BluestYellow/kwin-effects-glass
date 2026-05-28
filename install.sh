#!/usr/bin/env bash

# Exit immediately if a command exits with a non-zero status,
# if an uninitialized variable is used, or if a pipeline fails.
set -euo pipefail

# ANSI color codes for status messages
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Helper functions for logging
log_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

log_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

log_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1" >&2
}

# Ensure script is run from the directory containing it
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "${SCRIPT_DIR}"

log_info "Starting kwin-effects-glass installation/update script..."

# 1. Check for required build tools (cmake, make)
log_info "Checking for required build tools..."

if ! command -v cmake &> /dev/null; then
    log_error "cmake is required but could not be found. Please install cmake and try again."
    exit 1
fi
log_success "cmake is installed."

if ! command -v make &> /dev/null; then
    log_error "make is required but could not be found. Please install make and try again."
    exit 1
fi
log_success "make is installed."

# 2. Create the 'build' directory if it doesn't exist, or reuse it safely.
BUILD_DIR="${SCRIPT_DIR}/build"
if [ ! -d "${BUILD_DIR}" ]; then
    log_info "Creating build directory: ${BUILD_DIR}"
    mkdir -p "${BUILD_DIR}"
else
    log_info "Reusing existing build directory: ${BUILD_DIR}"
fi

# 3. Configure the build using 'cmake .. -DCMAKE_INSTALL_PREFIX=/usr'
log_info "Configuring build using CMake..."
cd "${BUILD_DIR}"

# Run cmake to generate build files
if ! cmake .. -DCMAKE_INSTALL_PREFIX=/usr; then
    log_warning "CMake configuration failed. Attempting a clean configuration by removing CMakeCache.txt..."
    rm -f CMakeCache.txt
    if ! cmake .. -DCMAKE_INSTALL_PREFIX=/usr; then
        log_error "CMake configuration failed even after cleaning cache."
        exit 1
    fi
fi
log_success "CMake configuration completed successfully."

# 4. Compile the project using 'make -j$(nproc)'
log_info "Compiling the project..."
JOBS=$(nproc 2>/dev/null || echo 4)
if ! make -j"${JOBS}"; then
    log_error "Compilation failed."
    exit 1
fi
log_success "Compilation completed successfully."

# 5. Install using 'sudo make install'
log_info "Installing the project (requires sudo privileges)..."
if ! sudo make install; then
    log_error "Installation failed."
    exit 1
fi

log_success "kwin-effects-glass has been successfully installed/updated!"
