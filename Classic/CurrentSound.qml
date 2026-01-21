import QtQuick 2.5
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.5
import QtQuick.Particles 2.0
import QtGraphicalEffects 1.12
import QtQuick.Dialogs 1.2

import "."


Drawer {
        id: main_win
        width: start.width
        height: start.height
        interactive: false

        edge: Qt.RightEdge

        // background simple fill one color
        Rectangle {
            anchors.fill: parent
            color: "#121212"
        }


        // background
        Rectangle {
            width: parent.width * 2
            height: parent.height * 2
            color: "transparent"

            opacity: 0.9

            LinearGradient {
                    anchors.fill: parent
                    start: Qt.point(width, 0)
                    end: Qt.point(0, height/2)
                    gradient: Gradient {
                        GradientStop { position: 0.0; color: "steelblue" }
                        GradientStop { position: 1.0; color: "#121212" }
                    }
            }
        }


        // main window column
        Column {
            spacing: 40
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter

            // Preset Text
            Text {
                text: "Preset"
                anchors.horizontalCenter: parent.horizontalCenter
                font.pixelSize: 58
                font.bold: true
                color: "#FFB74D"
            }

            // SpinBox - style choose
            Spin {
                // value: ble.current_style
                onValueChangedByUser: {
                    if (ble.current_style !== value)
                        ble.current_style = value
                }
            }

            //  On/Off button
            RoundBtnOnOff {
                id: powerButton
            }


            //---------------  Sliders  ---------------//
            Slider4 {
                id: slide_volume
                title: "MASTER VOLUME"
                minimum: 0
                maximum: 100
                width: main_win.width * 0.85
                height: main_win.height / 10
                anchors.horizontalCenter: parent.horizontalCenter

                value: ble.data_volume

                onValueChangedByUser: ble.data_volume = value
            }

            Slider4 {
                id: slider_bass
                title: "BASS"
                minimum: 0
                maximum: 100
                width: slide_volume.width
                height: slide_volume.height
                anchors.horizontalCenter: parent.horizontalCenter

                value: ble.data_bass/40

                onValueChangedByUser: ble.data_bass = value*40

            }

            Slider4 {
                title: "MIDDLE"
                minimum: 0
                maximum: 100
                width: slide_volume.width
                height: slide_volume.height
                anchors.horizontalCenter: parent.horizontalCenter

                value: ble.data_middle

                onValueChangedByUser: {
                    ble.data_middle = value
                }
            }

            Slider4 {
                title: "TREBLE"
                minimum: 0
                maximum: 100
                width: slide_volume.width
                height: slide_volume.height
                anchors.horizontalCenter: parent.horizontalCenter


                value: ble.data_treble/40

                onValueChangedByUser: {
                    ble.data_treble = value*40
                }
            }
        }
}

