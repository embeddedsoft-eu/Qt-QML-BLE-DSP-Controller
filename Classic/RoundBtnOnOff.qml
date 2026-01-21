import QtQuick 2.5
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.5
import QtQuick.Particles 2.0
import QtGraphicalEffects 1.12
import QtQuick.Dialogs 1.2


RoundButton {

    width: parent.width / 3
    height: width
    radius: width / 2
    anchors.horizontalCenter: parent.horizontalCenter


    text: ble.data_on_off ? "ON" : "OFF"
    font.pixelSize: 48
    font.bold: true
    //foreground: "white"

    background: Rectangle {
        radius: width / 2
        gradient: Gradient {
            GradientStop {
                position: 0.0
                color: ble.data_on_off ? "#FFB74D" : "#4E342E"
            }
            GradientStop {
                position: 1.0
                color: ble.data_on_off ? "#F57C00" : "#3E2723"
            }
        }

        // shadow
        layer.enabled: true
        layer.effect: DropShadow {
            horizontalOffset: 0
            verticalOffset: 6
            radius: 14
            samples: 16
            color: "#80000000"
        }
    }

    scale: pressed ? 0.96 : 1.0
    Behavior on scale {
        NumberAnimation { duration: 100 }
    }

    onClicked: ble.data_on_off = !ble.data_on_off
}
