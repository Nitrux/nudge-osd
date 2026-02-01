import QtQuick
import QtQuick.Controls
import QtQuick.Window
import org.mauikit.controls as Maui

// Access to Screen properties

Window {
    id: root
    width: 300
    height: 60
    visible: false
    color: "#00000000"
    flags: Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint | Qt.Tool

    // Center horizontally, position in bottom quarter vertically
    x: (Screen.width - width) / 2
    y: Screen.height * 0.75

    // System palette for KDE color scheme integration
    SystemPalette {
        id: palette
        colorGroup: SystemPalette.Active
    }

    // Use KDE/Qt color scheme
    readonly property color backgroundColor: palette.window
    readonly property color highlightColor: palette.highlight
    readonly property color textColor: palette.windowText
    readonly property color dimTextColor: palette.mid

    // Auto-hide timer
    Timer {
        id: hideTimer
        interval: 2000
        repeat: false
        onTriggered: hideAnimation.start()
    }

    // Show animation
    NumberAnimation {
        id: showAnimation
        target: container
        property: "opacity"
        from: 0.0
        to: 1.0
        duration: 200
        easing.type: Easing.OutQuad
        onStarted: root.visible = true
    }

    // Hide animation
    NumberAnimation {
        id: hideAnimation
        target: container
        property: "opacity"
        from: 1.0
        to: 0.0
        duration: 200
        easing.type: Easing.InQuad
        onFinished: root.visible = false
    }

    // Connect to controller signals
    Connections {
        target: controller
        function onNudgeTriggered() {
            hideTimer.stop()
            hideAnimation.stop()
            showAnimation.start()
            hideTimer.start()
        }
    }

    // Main container - pill shaped
    Rectangle {
        id: container
        anchors.centerIn: parent
        width: parent.width - 10
        height: parent.height - 10
        radius: height / 2  // Perfect pill shape
        color: root.backgroundColor
        opacity: 0.0

        border.width: 1
        border.color: Qt.lighter(root.backgroundColor, 1.3)

        layer.enabled: false

        Item {
            anchors.fill: parent
            anchors.margins: 10

            Rectangle {
                id: iconCircle
                width: 36
                height: 36
                radius: 18
                color: controller.muted ? palette.mid : root.highlightColor
                anchors.left: parent.left
                anchors.leftMargin: 5
                anchors.verticalCenter: parent.verticalCenter

                // System theme icons
                Maui.Icon {
                    anchors.centerIn: parent
                    width: 20
                    height: 20
                    source: controller.icon
                    color: Maui.Theme.textColor
                    isMask: true
                    visible: !controller.useNerdFont
                }

                // Nerd Font glyphs
                Label {
                    anchors.centerIn: parent
                    font.family: "Symbols Nerd Font"
                    font.pixelSize: 18
                    color: Maui.Theme.textColor
                    visible: controller.useNerdFont
                    text: {
                        var iconName = controller.icon
                        // Nerd Font glyphs (Font Awesome icons)
                        if (iconName.indexOf("audio-volume-muted") >= 0) return "\uf026"  //
                        if (iconName.indexOf("audio-volume-low") >= 0) return "\uf027"   //
                        if (iconName.indexOf("audio-volume-medium") >= 0) return "\uf027" //
                        if (iconName.indexOf("audio-volume-high") >= 0) return "\uf028"  //
                        if (iconName.indexOf("brightness-low") >= 0) return "\uf185"     //
                        if (iconName.indexOf("brightness-medium") >= 0) return "\uf185"  //
                        if (iconName.indexOf("brightness-high") >= 0) return "\uf185"    //
                        if (iconName.indexOf("keyboard-brightness") >= 0) return "\uf11c" //
                        if (iconName.indexOf("microphone") >= 0) return "\uf130"         //
                        if (iconName.indexOf("battery") >= 0) return "\uf240"            //
                        if (iconName.indexOf("media-playback-start") >= 0) return "\uf04b" //
                        if (iconName.indexOf("media-playback-pause") >= 0) return "\uf04c" //
                        if (iconName.indexOf("media-playback-stop") >= 0) return "\uf04d"  //
                        if (iconName.indexOf("media-skip-forward") >= 0) return "\uf051"   //
                        if (iconName.indexOf("media-skip-backward") >= 0) return "\uf048"  //
                        return "\uf028" // Default volume icon
                    }
                }
            }

            // Percentage text or "Muted" label
            Label {
                id: percentLabel
                width: controller.muted ? 60 : 32
                anchors.right: parent.right
                anchors.rightMargin: 5
                anchors.verticalCenter: parent.verticalCenter
                horizontalAlignment: Text.AlignRight
                font.pixelSize: 13
                font.weight: Font.Bold
                color: Maui.Theme.textColor
                text: controller.muted ? qsTr("Muted") : Math.round(controller.value) + "%"
            }

            // Progress bar container
            Item {
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.leftMargin: iconCircle.width + iconCircle.anchors.leftMargin + 12
                anchors.rightMargin: percentLabel.width + percentLabel.anchors.rightMargin + 12
                height: parent.height
                anchors.verticalCenter: parent.verticalCenter

                // Progress bar background
                Rectangle {
                    id: progressBg
                    anchors.verticalCenter: parent.verticalCenter
                    width: parent.width
                    height: 6
                    radius: 3
                    color: Qt.darker(root.backgroundColor, 1.3)

                    // Progress bar fill
                    Rectangle {
                        width: parent.width * (Math.min(controller.value, 100.0) / 100.0)
                        height: parent.height
                        radius: parent.radius
                        color: controller.muted ? palette.mid : root.highlightColor

                        Behavior on width {
                            NumberAnimation {
                                duration: 150
                                easing.type: Easing.OutQuad
                            }
                        }
                    }
                }
            }
        }
    }
}
