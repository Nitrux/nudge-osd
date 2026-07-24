# NudgeOSD | ![License](https://img.shields.io/badge/License-BSD--3--Clause-blue)

A QML-based on-screen display for keyboard shortcuts and system notifications designed for Wayland compositors.

![NudgeOSD](https://nxos.org/wp-content/uploads/2026/05/nudge-osd-002.png)
> NudgeOSD, a QML-based on-screen display. Built with **[MauiKit](https://mauikit.org/)** and LayerShell-Qt.

## Features

- Supports using either the system icon theme or Nerd Fonts to display the icons.
- Supports standard `.colors` schemes (KDE style).
- Direct D-Bus integration. The application will start in the background and listen for DBus commands.
- Built with `x86-64-v3` optimizations for modern hardware.

## Requirements

- Nitrux 6.1.0 and newer.

### Runtime Requirements

```
mauikit (>= 4.0.3)
qt6 (>= 6.9)
wayland
kf6-coreaddons (>= 6.13.0)
kf6-i18n (>= 6.13.0)
wireplumber
brightnessctl
layershellqt / layershellqtinterface
```

### Optional Runtime Requirements

```
nerd-font-symbols (only when `iconMode=emoji`)
```

## Usage

```
nudge-osd                           Daemon mode. Uses the configured icon mode.

nudge-osd --volume-down             Adjust volume (up or down) in steps.
          --volume-up
          --volume-mute             Mutes volume.
          --brightness-up           Adjust brightness (up or down) in steps.
          --brightness-down
```

The configured step is used when no amount is supplied. An optional positional
amount, such as `nudge-osd --volume-up 3`, overrides it for that invocation.

## Configuration

NudgeOSD watches `~/.config/nudge-osd/nudge-osd.conf` and applies changes while it is running.

```ini
[Appearance]
iconMode=system
width=292
height=66

[Position]
bottomOffset=114

[Behavior]
hideTimeout=2000
showAnimationDuration=200
hideAnimationDuration=200

[Controls]
volumeStep=5
brightnessStep=10
```

> [!NOTE]
> `iconMode` accepts `system` or `emoji`. Normal saves and atomic file replacements are
> detected and debounced before the new values are applied.

# Licensing

The license for this repository and its contents is **BSD-3-Clause**.

# Issues

If you find problems with the contents of this repository, please create an issue and use the **🐞 Bug report** template.

## Submitting a bug report

Before submitting a bug, you should look at the [existing bug reports](https://github.com/Nitrux/vxm/issues) to verify that no one has reported the bug already.

©2026 Nitrux Latinoamericana S.C.
