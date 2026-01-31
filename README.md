# NudgeOSD | ![License](https://img.shields.io/badge/License-BSD--3--Clause-blue)

A QML-based on-screen display for keyboard shortcuts and system notifications designed for Wayland compositors. Built with **[MauiKit](https://mauikit.org/)** and LayerShell-Qt.

The application will start in the background and listen for DBus commands.


## Supported Notifications

| Type | Icon Mapping | Description |
|------|-------------|-------------|
| `volume` | audio-volume-{muted,low,medium,high} | System volume |
| `brightness` | brightness-{low,medium,high} | Display brightness |
| `keyboard-brightness` | keyboard-brightness-{off,low,high} | Keyboard backlight |
| `microphone` | microphone-sensitivity-{muted,low,high} | Microphone level |
| `battery` | battery-{caution,low,good,full} | Battery status |
| `media-play` | media-playback-start | Play button |
| `media-pause` | media-playback-pause | Pause button |
| `media-stop` | media-playback-stop | Stop button |
| `media-next` | media-skip-forward | Next track |
| `media-previous` | media-skip-backward | Previous track |

# Licensing

The license for this repository and its contents is **BSD-3-Clause**.

# Issues

If you find problems with the contents of this repository, please create an issue and use the **🐞 Bug report** template.

## Submitting a bug report

Before submitting a bug, you should look at the [existing bug reports](https://github.com/Nitrux/vxm/issues) to verify that no one has reported the bug already.

©2026 Nitrux Latinoamericana S.C.
