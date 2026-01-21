import QtQuick 2.5
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.5
import QtQuick.Particles 2.0
import QtGraphicalEffects 1.12
import QtQuick.Dialogs 1.2


SpinBox {
    id: orangeSpin
    width: parent.width / 3
    height: width
    anchors.horizontalCenter: parent.horizontalCenter

    from: 0
    to: 10
    stepSize: 1

    signal valueChangedByUser(int v)

    onValueModified: {
        valueChangedByUser(value)
    }

    leftPadding: width / 3
    rightPadding: width / 3

    background: Rectangle {
        radius: height / 2
        color: "#3E2723"
        border.color: "#FF9800"
        border.width: 2
    }

    // Number in center
    contentItem: Text {
        text: orangeSpin.value
        font.pixelSize: 38
        font.bold: true
        color: "#FFB74D"
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }

    // button +
    up.indicator: Rectangle {
        width: orangeSpin.width / 3
        height: width
        anchors.right: parent.right
        radius: height / 2
        anchors.verticalCenter: parent.verticalCenter

        gradient: Gradient {
            GradientStop { position: 0.0; color: "#FFD180" }
            GradientStop { position: 1.0; color: "#F57C00" }
        }

        Text {
            anchors.centerIn: parent
            text: "+"
            font.pixelSize: 28
            font.bold: true
            color: "#3E2723"
        }
    }

    // button −
    down.indicator: Rectangle {
        width: orangeSpin.width / 3
        height: width
        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter
        radius: height / 2

        gradient: Gradient {
            GradientStop { position: 0.0; color: "#FFD180" }
            GradientStop { position: 1.0; color: "#F57C00" }
        }

        Text {
            anchors.centerIn: parent
            text: "−"
            font.pixelSize: 28
            font.bold: true
            color: "#3E2723"
        }
    }
}
