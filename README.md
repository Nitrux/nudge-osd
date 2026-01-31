# NudgeOSD | ![License](https://img.shields.io/badge/License-BSD--3--Clause-blue)

A QML-based on-screen display for keyboard shortcuts and system notifications designed for Wayland compositors.

![NudgeOSD](https://nxos.org/wp-content/uploads/2026/01/screenshot-20260131-120609.png)
> NudgeOSD, a QML-based on-screen display. Built with **[MauiKit](https://mauikit.org/)** and LayerShell-Qt.

## Features

- Supports using either the system icon theme or Nerd Fonts to display the icons.
- Supports standard `.colors` schemes (KDE style).
- Direct D-Bus integration. The application will start in the background and listen for DBus commands.
- Built with `x86-64-v3` optimizations for modern hardware.

## Requirements

- Nitrux 5.0.0 and newer.

### Runtime Requirements

```
mauikit (>= 4.0.2)
qt6 (>= 6.9.2)
wayland
kf6-coreaddons (>= 6.20.0)
wireplumber
brightnessctl
```

## Usage

```
nudge-osd                           Daemon mode. Uses system icons.
          --emoji                   Daemon mode. Uses Nerd Font glyphs.

nudge-osd --volume-down             Adjust volume (up, down or mute) in steps.
          --volume-up
          --volume-mute
          --brightness-up           Adjust brightness (up or down) in steps.
          --brightness-down
```

# Licensing

The license for this repository and its contents is **BSD-3-Clause**.

# Issues

If you find problems with the contents of this repository, please create an issue and use the **🐞 Bug report** template.

## Submitting a bug report

Before submitting a bug, you should look at the [existing bug reports](https://github.com/Nitrux/vxm/issues) to verify that no one has reported the bug already.

©2026 Nitrux Latinoamericana S.C.
