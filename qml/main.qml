import QtQuick
import QtQuick.Controls
import QtQuick.Window
import org.mauikit.controls as Maui

// Access to Screen properties

Window {
    id: root
    readonly property int osdWidth: controller.osdWidth
    readonly property int osdHeight: controller.osdHeight

    width: osdWidth
    height: osdHeight
    visible: false
    color: "#00000000"
    flags: Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint | Qt.Tool

    // Center horizontally, position in bottom quarter vertically
    x: (Screen.width - width) / 2
    y: Screen.height * 0.75

    // Use KDE/Qt color scheme
    readonly property color backgroundColor: Maui.Theme.alternateBackgroundColor
    readonly property color highlightColor: Maui.Theme.highlightColor
    readonly property color textColor: Maui.Theme.textColor
    readonly property color dimTextColor: Maui.Theme.disabledTextColor
    readonly property color iconContrastColor: Maui.Theme.highlightedTextColor
    // Auto-hide timer
    Timer {
        id: hideTimer
        interval: controller.hideTimeout
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
        duration: controller.showAnimationDuration
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
        duration: controller.hideAnimationDuration
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
        width: root.osdWidth
        height: root.osdHeight
        radius: height / 2  // Perfect pill shape
        color: Qt.alpha(root.backgroundColor, 0.76)
        opacity: 0.0

        border.width: 1
        border.color: Qt.alpha(Maui.Theme.textColor, 0.10)

        layer.enabled: false
        Item {
            anchors.fill: parent
            anchors.margins: 10

            Rectangle {
                id: iconCircle
                width: 36
                height: 36
                radius: 18
                color: controller.muted ? Maui.Theme.backgroundColor : root.highlightColor
                anchors.left: parent.left
                anchors.leftMargin: 5
                anchors.verticalCenter: parent.verticalCenter

                // System theme icons
                Maui.Icon {
                    anchors.centerIn: parent
                    width: 20
                    height: 20
                    source: controller.icon
                    color: controller.muted ? root.dimTextColor : root.iconContrastColor
                    visible: !controller.useNerdFont
                }

                // Nerd Font glyphs
                Label {
                    anchors.centerIn: parent
                    font.family: "Symbols Nerd Font"
                    font.pixelSize: 18
                    color: controller.muted ? root.dimTextColor : root.iconContrastColor
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
                        return "\uf028" // Default volume icon
                    }
                }
            }

            // Volume badge (Valenz WorkspaceBadge-style chip)
            ToolButton {
                id: percentBadge
                anchors.right: parent.right
                anchors.rightMargin: 5
                width: Maui.Style.units.gridUnit * 4
                anchors.verticalCenter: parent.verticalCenter

                text: controller.muted ? qsTr("Muted") : Math.round(controller.value) + "%"
                display: ToolButton.TextOnly
                font.bold: true
                padding: Maui.Style.space.tiny

                onClicked: {}

                background: Rectangle {
                    color: Maui.Theme.backgroundColor
                    radius: Maui.Style.radiusV
                }
            }

            // Progress bar container
            Item {
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.leftMargin: iconCircle.width + iconCircle.anchors.leftMargin + 12
                anchors.rightMargin: percentBadge.width + percentBadge.anchors.rightMargin + 12
                height: parent.height
                anchors.verticalCenter: parent.verticalCenter
                ProgressBar {
                    id: volumeProgress
                    anchors.verticalCenter: parent.verticalCenter
                    width: parent.width
                    from: 0
                    to: 100
                    value: Math.min(controller.value, 100.0)
                    background: Rectangle
                    {
                        implicitHeight: 10
                        radius: height / 2
                        color: Maui.Theme.backgroundColor
                    }
                    contentItem: Item
                    {
                        implicitHeight: 10
                        Rectangle
                        {
                            width: volumeProgress.position * parent.width
                            height: parent.height
                            radius: height / 2
                            color: controller.muted ? root.dimTextColor : root.highlightColor
                        }
                    }

                    Behavior on value {
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
