#!/usr/bin/env bash

# SPDX-License-Identifier: BSD-3-Clause
# Copyright 2024-2025 <Nitrux Latinoamericana S.C. <hello@nxos.org>>


# -- Exit on errors.

set -euo pipefail


# -- Prepare source.

SRC_DIR="$(pwd)"

BUILD_WORK_DIR="$(mktemp -d)"

cp -r "$SRC_DIR"/* "$BUILD_WORK_DIR/"

cd "$BUILD_WORK_DIR"


# -- Configure build.

DEB_ARCH="$(dpkg --print-architecture)"
DEB_HOST_MULTIARCH="$(dpkg-architecture -qDEB_HOST_MULTIARCH)"
# MauiKit software MUST be built with CMake and ECM; NEVER use Meson.
CMAKE_ARGS=(
    -DCMAKE_INSTALL_PREFIX=/usr
    -DKDE_INSTALL_LIBDIR="lib/$DEB_HOST_MULTIARCH"
    -DCMAKE_BUILD_TYPE=Release
)

if [ "$DEB_ARCH" = "amd64" ]; then
    CMAKE_ARGS+=(-DCMAKE_CXX_FLAGS=-march=x86-64-v3)
fi

cmake -S . -B .build "${CMAKE_ARGS[@]}"


# -- Compile source.

cmake --build .build --parallel "$(nproc)"


# -- Create a temporary DESTDIR for packaging.

DESTDIR="$(pwd)/pkg"
rm -rf "$DESTDIR"


# -- Install binary to DESTDIR.

DESTDIR="$DESTDIR" cmake --install .build


# -- Create DEBIAN control file.

mkdir -p "$DESTDIR/DEBIAN"

PKGNAME="nudge-osd"
VERSION="${PACKAGE_VERSION:?PACKAGE_VERSION must be supplied by CI}"
MAINTAINER="uri_herrera@nxos.org"
ARCHITECTURE="$DEB_ARCH"
DESCRIPTION="QML-based on-screen display. Built with MauiKit and LayerShell-Qt."

cat > "$DESTDIR/DEBIAN/control" <<EOF
Package: $PKGNAME
Version: $VERSION
Section: utils
Priority: optional
Architecture: $ARCHITECTURE
Maintainer: $MAINTAINER
Description: $DESCRIPTION
Depends: libkf6i18n6,libqt6core6t64, libqt6gui6, libqt6qml6, libqt6quick6, libqt6dbus6, libqt6quickcontrols2-6, qml6-module-qtquick, qml6-module-qtquick-controls, qml6-module-qtquick-window, libkf6coreaddons6, mauikit (>= 4.0.5), qml6-module-qtquick-effects, liblayershellqtinterface6, fonts-nitrux-desktop, wireplumber, brightnessctl
EOF


# -- Create .deb.

cd "$(dirname "$DESTDIR")"

dpkg-deb --build "$(basename "$DESTDIR")" "${PKGNAME}_${VERSION}_${ARCHITECTURE}.deb"


# -- Move .deb to ./build/ for CI consistency.

TARGET_DIR="${GITHUB_WORKSPACE:-$SRC_DIR}/build"

mkdir -p "$TARGET_DIR"

mv "${PKGNAME}_${VERSION}_${ARCHITECTURE}.deb" "$TARGET_DIR/"

echo "Debian package created: $(pwd)/build/${PKGNAME}_${VERSION}_${ARCHITECTURE}.deb"
