#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${ROOT_DIR}/build"
ARTIFACTS_DIR="${BUILD_DIR}/RTool_artefacts"
PACKAGE_DIR="${BUILD_DIR}/packages"
PAYLOAD_DIR="${BUILD_DIR}/pkg-payload"
IDENTIFIER="com.ranze.rtool.pkg"
VERSION="${RTool_VERSION:-0.1.0}"
MACOS_APP_SIGN_IDENTITY="${MACOS_APP_SIGN_IDENTITY:-}"
MACOS_INSTALLER_SIGN_IDENTITY="${MACOS_INSTALLER_SIGN_IDENTITY:-}"
MACOS_NOTARY_APPLE_ID="${MACOS_NOTARY_APPLE_ID:-}"
MACOS_NOTARY_PASSWORD="${MACOS_NOTARY_PASSWORD:-}"
MACOS_NOTARY_TEAM_ID="${MACOS_NOTARY_TEAM_ID:-}"

PKG_OUTPUT="${PACKAGE_DIR}/R-Tool-${VERSION}-macOS.pkg"

VST3_SOURCE="$(find "${ARTIFACTS_DIR}" -type d -name "R-Tool.vst3" | head -n 1)"
AU_SOURCE="$(find "${ARTIFACTS_DIR}" -type d -name "R-Tool.component" | head -n 1)"

if [[ ! -d "${VST3_SOURCE}" ]]; then
    echo "Missing VST3 bundle under: ${ARTIFACTS_DIR}" >&2
    echo "Run: cmake --build build --config Release" >&2
    exit 1
fi

if [[ ! -d "${AU_SOURCE}" ]]; then
    echo "Missing AU bundle under: ${ARTIFACTS_DIR}" >&2
    echo "Run: cmake --build build --config Release" >&2
    exit 1
fi

rm -rf "${PAYLOAD_DIR}" "${PACKAGE_DIR}"
mkdir -p "${PAYLOAD_DIR}/Library/Audio/Plug-Ins/VST3"
mkdir -p "${PAYLOAD_DIR}/Library/Audio/Plug-Ins/Components"
mkdir -p "${PACKAGE_DIR}"

ditto "${VST3_SOURCE}" "${PAYLOAD_DIR}/Library/Audio/Plug-Ins/VST3/R-Tool.vst3"
ditto "${AU_SOURCE}"   "${PAYLOAD_DIR}/Library/Audio/Plug-Ins/Components/R-Tool.component"

app_sign_identity="${MACOS_APP_SIGN_IDENTITY:-"-"}"
codesign_options=(--force --deep --sign "${app_sign_identity}")

if [[ "${app_sign_identity}" != "-" ]]; then
    codesign_options+=(--options runtime --timestamp)
fi

codesign "${codesign_options[@]}" "${PAYLOAD_DIR}/Library/Audio/Plug-Ins/VST3/R-Tool.vst3"
codesign "${codesign_options[@]}" "${PAYLOAD_DIR}/Library/Audio/Plug-Ins/Components/R-Tool.component"

pkgbuild_args=(
    --root "${PAYLOAD_DIR}"
    --identifier "${IDENTIFIER}"
    --version "${VERSION}"
    --install-location "/"
    --ownership recommended
)

if [[ -n "${MACOS_INSTALLER_SIGN_IDENTITY}" ]]; then
    pkgbuild_args+=(--sign "${MACOS_INSTALLER_SIGN_IDENTITY}")
fi

pkgbuild "${pkgbuild_args[@]}" "${PKG_OUTPUT}"

if [[ -n "${MACOS_NOTARY_APPLE_ID}" && -n "${MACOS_NOTARY_PASSWORD}" && -n "${MACOS_NOTARY_TEAM_ID}" ]]; then
    xcrun notarytool submit "${PKG_OUTPUT}" \
        --apple-id "${MACOS_NOTARY_APPLE_ID}" \
        --password "${MACOS_NOTARY_PASSWORD}" \
        --team-id "${MACOS_NOTARY_TEAM_ID}" \
        --wait

    xcrun stapler staple "${PKG_OUTPUT}"
fi

echo "Created ${PKG_OUTPUT}"
