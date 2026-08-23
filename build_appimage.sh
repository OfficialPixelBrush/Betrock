#!/usr/bin/env bash
# build_appimage.sh – Build LegacyWorldViewer and package it as an AppImage.
#
# Usage:
#   ./build_appimage.sh            # release build (no ASAN)
#   ./build_appimage.sh --debug    # keeps debug symbols, enables ASAN
#
# Output: LegacyWorldViewer-0.4.2-Linux.AppImage  (in the repo root)
#
# Requirements (install once):
#   sudo apt install cmake build-essential libfmt-dev libglfw3-dev \
#        libdeflate-dev libgl-dev
#
# linuxdeploy and its AppImage plugin are downloaded automatically on first
# run and cached in tools/ so you don't need them pre-installed.

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

VERSION="0.4.3"
APP_NAME="LegacyWorldViewer"
OUTPUT="${APP_NAME}-${VERSION}-Linux.AppImage"
TOOLS_DIR="${SCRIPT_DIR}/tools"
BUILD_DIR="${SCRIPT_DIR}/build"
APPDIR="${BUILD_DIR}/AppDir"

# ── Parse arguments ───────────────────────────────────────────────────────────
BUILD_TYPE="Release"
USE_ASAN="OFF"
for arg in "$@"; do
    case "$arg" in
        --debug) BUILD_TYPE="RelWithDebInfo"; USE_ASAN="ON" ;;
    esac
done

# ── Download linuxdeploy if needed ────────────────────────────────────────────
mkdir -p "$TOOLS_DIR"

LINUXDEPLOY="${TOOLS_DIR}/linuxdeploy-x86_64.AppImage"
LINUXDEPLOY_PLUGIN="${TOOLS_DIR}/linuxdeploy-plugin-appimage-x86_64.AppImage"

download_tool() {
    local url="$1"
    local dest="$2"
    if [[ ! -x "$dest" ]]; then
        echo "Downloading $(basename "$dest") ..."
        curl -L --fail --progress-bar -o "$dest" "$url"
        chmod +x "$dest"
    fi
}

download_tool \
    "https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage" \
    "$LINUXDEPLOY"

download_tool \
    "https://github.com/linuxdeploy/linuxdeploy-plugin-appimage/releases/download/continuous/linuxdeploy-plugin-appimage-x86_64.AppImage" \
    "$LINUXDEPLOY_PLUGIN"

# ── Build ─────────────────────────────────────────────────────────────────────
echo ""
echo "==> Configuring (${BUILD_TYPE}, ASAN=${USE_ASAN}) ..."
cmake -S "$SCRIPT_DIR" -B "$BUILD_DIR" \
    -DCMAKE_BUILD_TYPE="${BUILD_TYPE}" \
    -DUSE_ASAN="${USE_ASAN}" \
    -DCMAKE_INSTALL_PREFIX="${APPDIR}/usr"

echo ""
echo "==> Building ..."
cmake --build "$BUILD_DIR" --parallel "$(nproc)"

# ── Install into AppDir ───────────────────────────────────────────────────────
echo ""
echo "==> Installing into AppDir ..."
cmake --install "$BUILD_DIR" --prefix "${APPDIR}/usr"
mkdir -p "${APPDIR}/usr/share/icons/hicolor/256x256/apps"
cp "${SCRIPT_DIR}/LegacyWorldViewer.png" "${APPDIR}/usr/share/icons/hicolor/256x256/apps/LegacyWorldViewer.png"
mkdir -p "${APPDIR}/usr/share/applications"
cp "${SCRIPT_DIR}/LegacyWorldViewer.desktop" "${APPDIR}/usr/share/applications/LegacyWorldViewer.desktop"

# ── Package with linuxdeploy ──────────────────────────────────────────────────
# linuxdeploy bundles shared libraries, creates the AppRun wrapper, and calls
# the appimage plugin to produce the final .AppImage file.
echo ""
echo "==> Packaging AppImage ..."

# Tell linuxdeploy-plugin-appimage where to write the output file
export OUTPUT="${SCRIPT_DIR}/${OUTPUT}"

# ARCH must be set for the appimage plugin
export ARCH="x86_64"

cp "${SCRIPT_DIR}/LegacyWorldViewer.png" "${APPDIR}/LegacyWorldViewer.png"

# linuxdeploy validates the Icon= entry in the .desktop file by looking for a
# .DirIcon file in the AppDir root. Create it so the icon check passes.
ln -sf "usr/share/icons/hicolor/256x256/apps/LegacyWorldViewer.png" "${APPDIR}/.DirIcon"

# Run linuxdeploy; the --appimage-extract-and-run flag avoids FUSE issues in
# containers / build servers.
"${LINUXDEPLOY}" \
    --appimage-extract-and-run \
    --appdir "${APPDIR}" \
    --executable "${APPDIR}/usr/bin/LegacyWorldViewer" \
    --desktop-file "${APPDIR}/usr/share/applications/${APP_NAME}.desktop" \
    --icon-file "${SCRIPT_DIR}/LegacyWorldViewer.png" \
    --output appimage

echo ""
echo "==> Done!"
echo "    Output: ${SCRIPT_DIR}/${APP_NAME}-${VERSION}-Linux.AppImage"