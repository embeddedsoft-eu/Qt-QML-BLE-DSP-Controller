import QtQuick 2.5
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.5
import QtQuick.Particles 2.0
import QtGraphicalEffects 1.12

import "Classic"

Item {
    id: start

    width: 400
    height: 1200


    LinearGradient {
            anchors.fill: parent
            start: Qt.point(width, 0)
            end: Qt.point(0, height/2)
            gradient: Gradient {
                GradientStop { position: 0.0; color: "#121212" }
                GradientStop { position: 1.0; color: "steelblue" }
            }
    }


    Label {
        id: logo_text

        anchors.horizontalCenter: parent.horizontalCenter
        y: parent.height/3

        font.pixelSize: 35
        font.bold: true
        color: "#FFFFFF"

        text: "BLE FIRMWARE UPDATER"
    }


    Menu {
        id: settings
    }


    CurrentSound {
        id: cur_sound
    }

    Label {
        id: connect_text

        anchors.horizontalCenter: parent.horizontalCenter
        y: parent.height - height - 50
        color: "#FFFFFF"

        text: ble.message

        onTextChanged:
        {
            if (text == 'Connected')
            {
                print('Connected_text')
                ble.change_aux(2)
                cur_sound.visible = true
            }
        }
    }

    BusyIndicator {
        width: parent.width/3
        height:parent.width/3
        anchors.horizontalCenter: parent.horizontalCenter
        y: parent.height/2
        opacity: 0.2
    }


    Button {
        width: 50
        height: 50
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom:  connect_text.top

        opacity: 0.2

        rotation: 180
        background: BorderImage {
             source: "images/Vector.png"
        }

        onClicked: {
            cur_sound.visible = true
        }
    }


    Loader {
        id: pageLoader_st
        anchors.fill: parent
    }
}

