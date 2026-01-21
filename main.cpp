/*
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "ble.h"
*/


#include <QtCore/QLoggingCategory>
#include <QQmlContext>
#include <QGuiApplication>
#include <QQuickView>
#include "ble.h"


int main(int argc, char *argv[])
{
    qputenv("QML_DISABLE_DISK_CACHE", "1");

    QGuiApplication app(argc, argv);

    BLE ble;

    QQuickView *view = new QQuickView;
    view->rootContext()->setContextProperty("ble", &ble);
    view->setSource(QUrl("qrc:/Start.qml"));
    view->setResizeMode(QQuickView::SizeRootObjectToView);
    //view->showMaximized();
    view->show();
    return app.exec();

}
