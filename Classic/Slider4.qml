import QtQuick 2.5

Item {
    id: slider
    width: 320
    height: 64

    // ===== API =====
    property string title: "MASTER VOLUME"
    property int minimum: 0
    property int maximum: 100
    property int value: 50
    property color accent: "#FF9800"
    property bool enabled: true

    signal valueChangedByUser(int v)

    // ===== helpers =====
    function clamp(v, min, max) {
        return Math.min(Math.max(v, min), max)
    }

    function valueToX(v) {
        if (maximum === minimum) return 0
        return (v - minimum) / (maximum - minimum) * (track.width - knob.width)
    }

    function xToValue(x) {
        return minimum + x / (track.width - knob.width) * (maximum - minimum)
    }

    // ===== title =====
    Text {
        text: slider.title
        color: "#FFFFFF"
        font.pixelSize: 24
        font.bold: true
        anchors.left: parent.left
        anchors.top: parent.top
    }

    // ===== percent =====
    Text {
        text: Math.round(
                  (slider.value - minimum) * 100 / (maximum - minimum)
              ) + "%"
        color: "#FFFFFF"
        anchors.right: parent.right
        anchors.top: parent.top
    }

    // ===== track =====
    Rectangle {
        id: track
        width: parent.width
        height: knob.height/5
        radius: height/2
        y: knob.height*1.2
        color: "#3A3A3A"
        opacity: slider.enabled ? 1.0 : 0.4

        MouseArea {
            anchors.fill: parent
            enabled: slider.enabled
            onPressed: {
                var x = clamp(mouseX - knob.width/2,
                              0,
                              track.width - knob.width)
                slider.value = clamp(
                            Math.round(xToValue(x)),
                            minimum,
                            maximum)
                slider.valueChangedByUser(slider.value)
            }
        }
    }

    // ===== progress =====
    Rectangle {
        height: track.height
        radius: track.radius
        y: track.y
        width: knob.x + knob.width/2
        color: slider.accent
        opacity: slider.enabled ? 1.0 : 0.4
    }

    // ===== knob =====
    Rectangle {
        id: knob
        width: parent.width/20
        height: width*2
        radius: 3
        color: "transparent"
        y: track.y + track.height / 2 - height / 2
        x: valueToX(slider.value)
        opacity: slider.enabled ? 1.0 : 0.4

        border.color: "#FFFFFF20"
        border.width: 3

        Rectangle {
            anchors.fill: parent
            radius: parent.radius
            gradient: Gradient {
                GradientStop { position: 1.0; color: "darkGray" }
                GradientStop { position: 1.0; color: "#FFFFFF" }
            }
        }

        MouseArea {
            id: knobMouse
            anchors.fill: parent
            enabled: slider.enabled
            drag.target: parent
            drag.axis: Drag.XAxis
            drag.minimumX: 0
            drag.maximumX: track.width - knob.width

            onPositionChanged: {
                slider.value = clamp(
                            Math.round(xToValue(knob.x)),
                            minimum,
                            maximum)
                slider.valueChangedByUser(slider.value)
            }
        }
    }
}
