/******************************************************************************
 * BLE Communication Flow (HM-10 compatible)
 *
 * 1) Replace UUIDs with your own (HM-10 compatible UUIDs)
 *
 * 2) When the service reaches the state:
 *      QLowEnergyService::ServiceDiscovered
 *
 *    This means the device is connected. At this point you must:
 *    - Enable notifications (to receive data from the device)
 *    - Send the initial service message (to transmit data to the device)
 *
 * ---------------------------------------------------------------------------
 * Sending data example:
 *
 *     QByteArray arr;
 *     arr.resize(4);
 *     arr[0] = 0x20;
 *     arr[1] = 0x00;
 *     arr[2] = 0x00;
 *     arr[3] = 0x00;
 *
 *     // m_service->writeDescriptor(m_notificationDesc, arr);
 *
 * ---------------------------------------------------------------------------
 * Enabling notifications:
 *
 *     m_notificationDesc = hrChar.descriptor(
 *         QBluetoothUuid::ClientCharacteristicConfiguration
 *     );
 *
 *     if (m_notificationDesc.isValid()) {
 *         // Enable notifications
 *         m_service->writeDescriptor(
 *             m_notificationDesc,
 *             QByteArray::fromHex("0100")
 *         );
 *     }
 *
 * ---------------------------------------------------------------------------
 * 3) Connect the signal that is emitted when a notification is received
 *    (i.e. when the characteristic value changes). In the slot, update
 *    the application state using the received data.
 *
 *     connect(
 *         m_service,
 *         SIGNAL(characteristicChanged(
 *             QLowEnergyCharacteristic,
 *             QByteArray
 *         )),
 *         this,
 *         SLOT(updateBLEValue(
 *             QLowEnergyCharacteristic,
 *             QByteArray
 *         ))
 *     );
 *
 *     void BLE::updateBLEValue(
 *         const QLowEnergyCharacteristic &characteristic,
 *         const QByteArray &value
 *     );
 *
 * ---------------------------------------------------------------------------
 * 4) confirmedDescriptorWrite()
 *
 *    This slot confirms that the descriptor (or data) has been successfully
 *    written. No changes were made to this logic.
 *
 * ---------------------------------------------------------------------------
 * 5) serviceScanDone()
 *
 *    Called after service discovery is complete. Attempts to connect to
 *    the desired BLE service.
 *
 * ---------------------------------------------------------------------------
 * BLE Data Flow Summary:
 *
 * - When the central device wants to send data to the module:
 *     → it WRITES to the characteristic.
 *
 * - When the module wants to send data to the central device:
 *     → it sends a NOTIFICATION.
 *
 *****************************************************************************/


#include "ble.h"

#include <QLowEnergyCharacteristic>

#include <QtEndian>
#include <QString>
#include <QFile>
#include <QStandardPaths>

#define CON_PARAMS 1

#define DEVICE_NAME "HM-10"


BLE::BLE():
    m_currentDevice(QBluetoothDeviceInfo()), foundBLEService(false), m_control(NULL),
    m_service(NULL)
{
    //! [devicediscovery-1]
    m_deviceDiscoveryAgent = new QBluetoothDeviceDiscoveryAgent(this);

    connect(m_deviceDiscoveryAgent, SIGNAL(deviceDiscovered(const QBluetoothDeviceInfo&)),
                              this, SLOT(addDevice(const QBluetoothDeviceInfo&)));

    connect(m_deviceDiscoveryAgent, SIGNAL(error(QBluetoothDeviceDiscoveryAgent::Error)),
                              this, SLOT(deviceScanError(QBluetoothDeviceDiscoveryAgent::Error)));

    connect(m_deviceDiscoveryAgent, SIGNAL(finished()), this, SLOT(scanFinished()));
    //! [devicediscovery-1]

    cur_state = 0;

    if (!cur_state) {
        qDebug() << "Error string: " << m_deviceDiscoveryAgent->errorString();
        m_deviceDiscoveryAgent->start();
    }

    write_timer = new QTimer(this);
    connect(write_timer, SIGNAL(timeout()), this, SLOT(writeDelay()));

    disconnect_timer = new QTimer(this);
    connect(disconnect_timer, SIGNAL(timeout()), this, SLOT(disconnectDelay()));

    con_enable = false;
    Q_EMIT conEnableChanged();

    data_on_off = 1;
    data_volume = 0;
    data_bass = 0;
    data_middle = 0;
    data_treble = 0;

    waiting = 0;
    emit waitingChanged();

    answer_timeout_flag = 0;
    send_flag = 0;
}

BLE::~BLE()
{
    qDeleteAll(m_devices);
    m_devices.clear();
}


void BLE::deviceSearch()
{
    qDeleteAll(m_devices);
    m_devices.clear();
    m_deviceDiscoveryAgent->start();
    setMessage("Scanning for devs...");
}


void BLE::addDevice(const QBluetoothDeviceInfo &device)
{
    if (device.coreConfigurations() & QBluetoothDeviceInfo::LowEnergyCoreConfiguration)
    {
        #ifdef Q_OS_MAC
            // workaround for Core Bluetooth:
            qWarning() << "Discovered LE Device name: " << device.name() << " Address: "
                       << device.deviceUuid().toString();
        #else
            qWarning() << "Discovered LE Device name: " << device.name();
        #endif

        DeviceInfo *dev = new DeviceInfo(device);
        m_devices.append(dev);
        setMessage("BLE dev found. Scanning for more...");

        if (dev->getName().contains(DEVICE_NAME,  Qt::CaseInsensitive) )
        {
            m_deviceDiscoveryAgent->stop();

            #ifdef Q_OS_MAC
                qWarning() << "BLE found, waiting for connection with " << device.deviceUuid().toString();
                connectToService(device.deviceUuid().toString());
            #else
                qWarning() << "BLE found, waiting for connection with " << device.address().toString();
                connectToService(device.address().toString());
            #endif

        }
    }
}

void BLE::scanFinished()
{
    if (m_devices.size() == 0)
        setMessage("No Low Energy devices found");

    if (!cur_state)
         m_deviceDiscoveryAgent->start();

    Q_EMIT nameChanged();
}

QVariant BLE::name()
{
    return QVariant::fromValue(m_devices);
}

void BLE::deviceScanError(QBluetoothDeviceDiscoveryAgent::Error error)
{
    if (error == QBluetoothDeviceDiscoveryAgent::PoweredOffError)
        setMessage("Turn on bluetooth.");
    else if (error == QBluetoothDeviceDiscoveryAgent::InputOutputError)
        setMessage("IO from the device error.");
    else
        setMessage("An unknown error has occurred.");
}

void BLE::setMessage(QString message)
{
    m_info = message;
    Q_EMIT messageChanged();
    qDebug() << "message changed: " << message;
}

QString BLE::message() const
{
    return m_info;
}

QString BLE::busy_message() const
{
    return bsy_message;
}

void BLE::setBusyMessage(QString message)
{
    bsy_message = message;
    Q_EMIT busy_messageChanged();
    qDebug() << "busy message changed: " << message;
}

QString BLE::serial_num() const
{
    return serial_number;
}

void BLE::setSerialNum(QByteArray num)
{
    for (int i = 0; i < num.size(); i++)
        serial_number += num[i];

    Q_EMIT serial_numChanged();

    qDebug() << "serial num Changed: " << serial_number;
}

void BLE::setFWNum(QByteArray num)
{
    for (int i = 0; i < num.size(); i++)
        fw_number += num[i];

    Q_EMIT fw_numChanged();

    qDebug() << "FW num Changed: " << fw_number;
}

QString BLE::fw_num() const
{
    return fw_number;
}


void BLE::connectToService(const QString &address)
{
    for (int i = 0; i < m_devices.size(); i++)
    {
        qWarning() << "name 1: " <<((DeviceInfo*)m_devices.at(i))->getAddress();
        qWarning() << "name 2: " <<address;

        if (((DeviceInfo*)m_devices.at(i))->getAddress() == address )
        {
            m_currentDevice.setDevice(((DeviceInfo*)m_devices.at(i))->getDevice());
            setMessage("Connecting to device...");
            // deviceFound = true;
            break;
        }
    }

    if (m_control) {
        m_control->disconnectFromDevice();
        delete m_control;
        m_control = 0;
    }


    //! [Connect signals]
    m_control = new QLowEnergyController(m_currentDevice.getDevice(), this);

    connect(m_control, SIGNAL(serviceDiscovered(QBluetoothUuid)), this, SLOT(serviceDiscovered(QBluetoothUuid)));
    connect(m_control, SIGNAL(discoveryFinished()), this, SLOT(serviceScanDone()));
    connect(m_control, SIGNAL(error(QLowEnergyController::Error)), this, SLOT(controllerError(QLowEnergyController::Error)));
    connect(m_control, SIGNAL(connected()), this, SLOT(deviceConnected()));
    connect(m_control, SIGNAL(disconnected()), this, SLOT(deviceDisconnected()));

    connect(m_control, SIGNAL(connectionUpdated(QLowEnergyConnectionParameters)), this, SLOT(connupp(QLowEnergyConnectionParameters)));
    connect(m_control, &QLowEnergyController::connected, [](){qDebug() << "CONNECTED OK";});

    m_control->connectToDevice();
    //! [Connect signals]

    qWarning() << "con2";
}


void BLE::connupp(const QLowEnergyConnectionParameters &params)
{
    qDebug() << "conup(QLowEnergyConnectionParameters)): " << params.maximumInterval();
}


//! [Connecting to service]

void BLE::deviceConnected()
    {
    #ifdef CON_PARAMS
        QLowEnergyConnectionParameters params;

        params.setIntervalRange(7.5, 10);
        params.setSupervisionTimeout(30);

        m_control->requestConnectionUpdate( params );

        qDebug() <<  "maxInterval: " << params.maximumInterval() << "min interval: "<< params.minimumInterval() << "supervision timeout" << params.supervisionTimeout();
    #endif

    m_control->discoverServices();
}

void BLE::deviceDisconnected()
{
    setMessage("Ble service disconnected");
    qWarning() << "Remote device disconnected";

    connetion_check_timer.stop();

    cur_state = 0;
    Q_EMIT stateChanged();

    con_enable = false;
    Q_EMIT conEnableChanged();

    m_deviceDiscoveryAgent->start();
}

void BLE::serviceDiscovered(const QBluetoothUuid &gatt)
{
    if (gatt == QBluetoothUuid((quint16) 0xffe0) )
    {
        setMessage("Ble service discovered. Waiting for service scan to be done...");
        foundBLEService = true;
    }
}

void BLE::serviceScanDone()
{
    delete m_service;
    m_service = NULL;

    if (foundBLEService)
    {
        setMessage("Connecting to service...");
        //        m_service = ->createServiceObject( QBluetoothUuid(QBluetoothUuid::HM_10_CHARACTERISTIC), this);
        m_service = m_control->createServiceObject( QBluetoothUuid((quint16)0xffe0), this);
    }

    if (!m_service)
    {
        setMessage("Service not found: 11.");
        return;
    }

    connect(m_service, SIGNAL(stateChanged(QLowEnergyService::ServiceState)), this, SLOT(serviceStateChanged(QLowEnergyService::ServiceState)));

    connect(m_service, SIGNAL(characteristicChanged(QLowEnergyCharacteristic,QByteArray)), this, SLOT(updateBLEValue(QLowEnergyCharacteristic,QByteArray)));

    connect(m_service, SIGNAL(descriptorWritten(QLowEnergyDescriptor,QByteArray)), this, SLOT(confirmedDescriptorWrite(QLowEnergyDescriptor,QByteArray)));

    m_service->discoverDetails();

    setMessage(QString::fromLocal8Bit("Connected"));


    #ifdef CON_PARAMS
        QLowEnergyConnectionParameters params;

        params.setIntervalRange(7.5, 10);
        params.setSupervisionTimeout(20);

        m_control->requestConnectionUpdate( params );


        qWarning() << "conn2, maxInterval: " << params.maximumInterval() << "min interval: "<< params.minimumInterval() << "supervision timeout" << params.supervisionTimeout();
    #endif
}


void BLE::disconnectService()
{
    foundBLEService = false;

    if (m_devices.isEmpty())
    {
        return;
    }

    //disable notifications before disconnecting
    if (m_notificationDesc.isValid() && m_service && m_notificationDesc.value() == QByteArray::fromHex("0100"))
    {
        m_service->writeDescriptor(m_notificationDesc, QByteArray::fromHex("0000"));
    }
    else
    {
        m_control->disconnectFromDevice();
        delete m_service;
        m_service = NULL;
    }

    con_enable = false;
    Q_EMIT conEnableChanged();

    setMessage(QString::fromLocal8Bit("Disconnected"));
}

void BLE::controllerError(QLowEnergyController::Error error)
{
    setMessage("Cannot connect to remote device.");
    qWarning() << "Controller Error:" << error;
}


void BLE::serviceStateChanged(QLowEnergyService::ServiceState s)
{
    setMessage("Connected");

    switch (s) {
    case QLowEnergyService::ServiceDiscovered:
    {
        sendModeReq();

        const QLowEnergyCharacteristic hrChar = m_service->characteristic( QBluetoothUuid((quint16)0xffe1) );
        m_notificationDesc = hrChar.descriptor(QBluetoothUuid::ClientCharacteristicConfiguration);

        if (m_notificationDesc.isValid())
        {
            // enable notifications
            m_service->writeDescriptor(m_notificationDesc, QByteArray::fromHex("0100"));

            qWarning() << "conn8";
            connetion_check_timer.start(1000);
        }

        break;
    }
    default:
        //nothing for now
        break;
    }
}

void BLE::serviceError(QLowEnergyService::ServiceError e)
{
    switch (e) {
    case QLowEnergyService::DescriptorWriteError:
        setMessage("Cannot obtain BLE notifications");
        break;
    default:
        qWarning() << "BLE service error:" << e;
    }
}

void BLE::updateBLEValue(const QLowEnergyCharacteristic &c, const QByteArray &value)
{
    // ignore any other characteristic change -> shouldn't really happen though
    if (c.uuid() != QBluetoothUuid(QBluetoothUuid((quint16)0xffe1)))
        return;

    const quint8 *data = reinterpret_cast<const quint8 *>(value.constData());
    // quint8 flags = data[0];

    ParseIncomeData(data, value.size());
}

void BLE::confirmedDescriptorWrite(const QLowEnergyDescriptor &d,
                                         const QByteArray &value)
{
    if (d.isValid() && d == m_notificationDesc && value == QByteArray::fromHex("0000"))
    {
        //disabled notifications -> assume disconnect intent
        m_control->disconnectFromDevice();
        delete m_service;
        m_service = 0;
    }
}


QString BLE::deviceAddress() const
{
    return m_currentDevice.getAddress();
}


int BLE::numDevices() const
{
    return m_devices.size();
}


//---------------------------------------------------//


void BLE::sendModeReq()
{
    if (!foundBLEService)
    {
        setMessage(QString::fromLocal8Bit("NO BLE"));
        return;
    }

    const QLowEnergyCharacteristic hrChar = m_service->characteristic( QBluetoothUuid((quint16)0xffe1) );

    if (!hrChar.isValid())
    {
        setMessage(QString::fromLocal8Bit("BLE Data not found."));
        return;
    }

    QByteArray arr;
    arr.resize(3);
    arr[0] = 0x01;
    arr[1] = 0x02;
    arr[2] = 0x0;

    m_service->writeCharacteristic(hrChar, arr, QLowEnergyService::WriteWithoutResponse);
    disconnect_timer->start(500);
}



//---------------------------------------------------//


QString BLE::WriteCustomDataToBle(QByteArray arr)
{
    qWarning() << "connn sending user data";

    if (!foundBLEService)
    {
        setMessage(QString::fromLocal8Bit("NO BLE"));
        return QString::fromLocal8Bit("ERROR");
    }

    //    setMessage(QString::fromLocal8Bit("Out msg: ") + QString::fromLocal8Bit(arr));

    const QLowEnergyCharacteristic hrChar = m_service->characteristic( QBluetoothUuid((quint16)0xffe1) );

    if (!hrChar.isValid())
    {
        setMessage(QString::fromLocal8Bit("BLE Data not found."));
        return QString::fromLocal8Bit("ERROR");
    }

    //   m_service->writeDescriptor(m_notificationDesc, arr);
    m_service->writeCharacteristic(hrChar, arr, QLowEnergyService::WriteWithoutResponse);

    return QString::fromLocal8Bit("OK");
}


//---------------------------------------------------//

int BLE::CurState()
{
    return cur_state;
}

int BLE::DataOnOff()
{
    return data_on_off;
}

int BLE::DataVolume()
{
    return data_volume;
}

int BLE::DataBass()
{
    return data_bass;
}

int BLE::DataMiddle()
{
    return data_middle;
}

int BLE::DataTreble()
{
    return data_treble;
}

int BLE::CurrentStyleOn()
{
    return current_style;
}

bool BLE::ConEnable()
{
    return con_enable;
}

int BLE::Waiting()
{
    return waiting;
}

void BLE::change_data_on_off(int val)
{
    qWarning() << "change data on off " << val;
    data_on_off = val;
    sendNewSettings();
}

void BLE::change_data_volume(int val)
{
    qWarning() << "change data volume " << val;
    data_volume = val;
    sendNewSettings();
}

void BLE::change_data_bass(int val)
{
    qWarning() << "change data |bass| " << val;
    data_bass = val;
    sendNewSettings();
}

void BLE::change_data_middle(int val)
{
    qWarning() << "change data |middle| " << val;
    data_middle = val;
    sendNewSettings();
}

void BLE::change_data_treble(int val)
{
    qWarning() << "change data |treble| " << val;
    data_treble = val;
    sendNewSettings();
}

void BLE::change_sound_style(int val)
{
    if (current_style == val)
        return;

    current_style = val;
    emit sound_style_Changed();
    sendNewStyle();
}


void BLE::changeFWNum(QString num)
{
    fw_number = num;
    qDebug() << "fw_number changed: " << fw_number;
}


QString BLE::sendNewSettings()
{
    write_timer->start(20);

    send_flag = 1;
    return QString("OK");
}

QString BLE::sendNewStyle()
{
    write_timer->start(20);

    send_flag = 3;
    return QString("OK");
}


void BLE::disconnectDelay()
{
    disconnect_timer->stop();
//    con_enable = false;
}

//------------------------------------------------------------//

void BLE::ParseIncomeData(const quint8 *data, int size)
{
    if (size < 2)
        return;

    const quint8 type = data[0];
    const quint8 cmd  = data[1];

    const bool isRealTimeData = (cmd == 0x13) && (type == 0x01 || type == 0x02 || type == 0x03);
    const bool isFwVer  = (type == 0xAB && cmd == 0xDC);


    if ( isFwVer )
    {
        serial_number.clear();
        serial_number += "V";

        quint16 numss =  (quint16)((quint16)data[2] << 8) | (quint16)data[3];

        serial_number += QString::number((double)numss/10);

        Q_EMIT serial_numChanged();

        qWarning() << "Serial number read OK: " << serial_number;
    }


    if ( isRealTimeData )   // real time data
    {
        qWarning() << "cur state recieved";

        quint8 data_on_off1 = data[2];
        quint16 data_volume1 = (quint16)((quint16)data[3] << 8) | (quint16)data[4];
        quint16 data_bass1 = (quint16)((quint16)data[5] << 8) | (quint16)data[6];
        quint16 data_middle1 = (quint16)((quint16)data[7] << 8) | (quint16)data[8];
        quint16 data_treble1 = (quint16)((quint16)data[9] << 8) | (quint16)data[10];
        quint8 current_style1 = data[11];

        data_on_off = data_on_off1;
        data_volume = data_volume1;
        data_bass = data_bass1;
        data_treble = data_treble1;
        data_middle = data_middle1;

        if (data[0] == 1)
            current_style = current_style1;

        Q_EMIT on_off_Changed();
        Q_EMIT volume_Changed();
        Q_EMIT bass_Changed();
        Q_EMIT treble_Changed();
        Q_EMIT middle_Changed();
        Q_EMIT sound_style_Changed();

        if (con_enable == false)
        {
            con_enable = true;
            Q_EMIT conEnableChanged();
        }

        qWarning() << "";
        qWarning() << "Current sound style" << current_style;

        qWarning() << "data_on_off" << data_on_off;
        qWarning() << "data_volume" << data_volume;
        qWarning() << "data_bass" << data_bass;
        qWarning() << "data_middle" << data_middle;
        qWarning() << "data_treble" << data_treble;
    }
}


void BLE::writeDelay()
{
    if (send_flag == 1)     // presets
    {
        QByteArray arr;
        arr.resize(12);
        arr[0] = 0x02;
        arr[1] = 0x13;
        arr[2] = 0xFF&data_on_off;
        arr[3] = 0xFF&(data_volume >> 8);
        arr[4] = 0xFF&(data_volume);

        arr[5] = 0xFF&(data_bass >> 8);
        arr[6] = 0xFF&(data_bass);

        arr[7] = 0xFF&(data_middle >> 8);
        arr[8] = 0xFF&(data_middle);

        arr[9] = 0xFF&(data_treble >> 8);
        arr[10] = 0xFF&(data_treble);

        WriteCustomDataToBle(arr);

        write_timer->start(30);
        send_flag = 2;
    }

    if (send_flag == 3)     // Getting current style
    {
        QByteArray arr;
        arr.resize(3);
        arr[0] = 0x03;
        arr[1] = 2;
        arr[2] = (char)current_style;
        WriteCustomDataToBle(arr);

        qWarning() << "set current style: " << current_style;

        write_timer->start(30);
        send_flag = 2;
    }


    if (send_flag == 10)     // Getting serial number
    {
        QByteArray arr;
        arr.resize(3);
        arr[0] = 0xAB;
        arr[1] = 0xCD;
        arr[2] = 0x0A;
        WriteCustomDataToBle(arr);

        qWarning() << "Getting serial number sended";

        write_timer->start(50);
        send_flag = 2;
    }

    if (send_flag == 2)
    {
        write_timer->stop();
        send_flag = 0;
    }
}


void BLE::change_aux(int val)
{
    qDebug() << "";
    qDebug() << "change aux " << val;
    qDebug() << "";

    if (val == 1)
    {
        qDebug () << "UpdateFW pressed in QML";
    }

    if (val == 2)
    {
        qDebug() << "Getting serial number from QML";
        GetSerialNumber();
    }
}

int BLE::GetSerialNumber()
{
    send_flag = 10;
    write_timer->start(10);
    return 0;
}
