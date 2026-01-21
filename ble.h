/***************************************************************************
**
** Copyright (C) 2014 BlackBerry Limited. All rights reserved.
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the examples of the QtBluetooth module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef BLE_H
#define BLE_H

#include "deviceinfo.h"

#include <QString>
#include <QDebug>
#include <QDateTime>
#include <QVector>
#include <QTimer>
#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothDeviceInfo>
#include <QLowEnergyController>
#include <QLowEnergyService>

#include <QLowEnergyConnectionParameters>



QT_USE_NAMESPACE
class BLE: public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString message READ message NOTIFY messageChanged)
    Q_PROPERTY(QString busy_message READ busy_message NOTIFY busy_messageChanged)
    Q_PROPERTY(QString serial_num READ serial_num NOTIFY serial_numChanged)
    Q_PROPERTY(QVariant name READ name NOTIFY nameChanged)
    Q_PROPERTY(QString fw_num READ fw_num NOTIFY fw_numChanged)

    Q_PROPERTY(int data_on_off READ DataOnOff WRITE change_data_on_off NOTIFY on_off_Changed)
    Q_PROPERTY(int data_volume READ DataVolume WRITE change_data_volume NOTIFY volume_Changed)
    Q_PROPERTY(int data_bass READ DataBass WRITE change_data_bass NOTIFY bass_Changed)
    Q_PROPERTY(int data_middle READ DataMiddle WRITE change_data_middle NOTIFY middle_Changed)
    Q_PROPERTY(int data_treble READ DataTreble WRITE change_data_treble NOTIFY treble_Changed)
    Q_PROPERTY(int current_style READ CurrentStyleOn WRITE change_sound_style NOTIFY sound_style_Changed)

    Q_PROPERTY(bool con_enable READ ConEnable NOTIFY conEnableChanged)
    Q_PROPERTY(int waiting READ Waiting NOTIFY waitingChanged)


Q_SIGNALS:
    void carsChanged();
    void instructionChanged();
    void langChanged();
    void cur_langChanged();
    void nameChanged();

private:
    int cur_state = 0;

public:
    int CurState();
    int DataOnOff();
    int DataVolume();
    int DataBass();
    int DataMiddle();
    int DataTreble();
    int CurrentStyleOn();

    bool ConEnable();

    int Waiting();


public:
    BLE();
    ~BLE();

    void setMessage(QString message);
    QString message() const;

    void setBusyMessage(QString message);
    QString busy_message() const;
    QString bsy_message = "Progress";

    void setSerialNum(QByteArray num);
    QString serial_num() const;
    QString serial_number = "Zero";

    void setFWNum(QByteArray num);
    QString fw_num() const;
    QString fw_number = "";

    QVariant name();

    int data_on_off;
    int data_volume;
    int data_bass;
    int data_middle;
    int data_treble;

    int current_style;

    bool con_enable;

    int send_flag;
    int answer_timeout_flag;

    int waiting;


public slots:
    void deviceSearch();
    void connectToService(const QString &address);
    void disconnectService();

    QString deviceAddress() const;
    int numDevices() const;

public slots:

    QString sendNewSettings();
    QString sendNewStyle();

    void change_data_on_off(int val);
    void change_data_volume(int val);
    void change_data_bass(int val);
    void change_data_middle(int val);
    void change_data_treble(int val);
    void change_sound_style(int val);
    void changeFWNum(QString num);
    void change_aux(int val);

signals:
    void on_off_Changed();
    void volume_Changed();
    void bass_Changed();
    void middle_Changed();
    void treble_Changed();
    void sound_style_Changed();
    void conEnableChanged();
    void waitingChanged();

public:
    void sendModeReq();
    int GetSerialNumber();




public:
    QString WriteCustomDataToBle(QByteArray arr);

    void ParseIncomeData(const quint8 *data, int size);

private slots:
    //  QBluetothDeviceDiscoveryAgent
    void addDevice(const QBluetoothDeviceInfo&);
    void scanFinished();
    void deviceScanError(QBluetoothDeviceDiscoveryAgent::Error);

    //QLowEnergyController
    void serviceDiscovered(const QBluetoothUuid &);
    void serviceScanDone();
    void controllerError(QLowEnergyController::Error);
    void deviceConnected();
    void deviceDisconnected();

    void connupp(const QLowEnergyConnectionParameters &);



    //QLowEnergyService
    void serviceStateChanged(QLowEnergyService::ServiceState s);
    void updateBLEValue(const QLowEnergyCharacteristic &c,
                              const QByteArray &value);
    void confirmedDescriptorWrite(const QLowEnergyDescriptor &d,
                              const QByteArray &value);
    void serviceError(QLowEnergyService::ServiceError e);

Q_SIGNALS:
    void messageChanged();
    void busy_messageChanged();
    void serial_numChanged();
    void fw_numChanged();

    void stateChanged();
    void new_data();

private:
    QTimer *write_timer;
    QTimer *disconnect_timer;

private slots:
    void writeDelay();
    void disconnectDelay();

private:
    DeviceInfo m_currentDevice;
    QBluetoothDeviceDiscoveryAgent *m_deviceDiscoveryAgent;
    QLowEnergyDescriptor m_notificationDesc;
    QList<QObject*> m_devices;
    QString m_info;
    bool foundBLEService;

    QLowEnergyController *m_control;
    QLowEnergyService *m_service;

private:
    QTimer connetion_check_timer;

};

#endif // BLE_H
