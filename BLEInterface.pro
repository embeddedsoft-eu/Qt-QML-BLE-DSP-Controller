TEMPLATE = app

QT += qml quick bluetooth
CONFIG += c++11

SOURCES += main.cpp \
    deviceinfo.cpp \
    ble.cpp \

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
#include(deployment.pri)

ios {
    QMAKE_INFO_PLIST = Info.plist

#    ios_launch.files = $$files(ios/Images.xcassets/LaunchImage.launchimage/*.png)
#    QMAKE_BUNDLE_DATA += ios_launch

#    ios_icon.files = $$files(ios/Images.xcassets/AppIcon.appiconset/*.png)
#    QMAKE_BUNDLE_DATA += ios_icon

    QMAKE_BUNDLE_DATA += Storyboard.storyboard
}

DISTFILES += \
    LangSelect.qml \
    SelectCar.qml \
    android/AndroidManifest.xml \
    android/AndroidManifest.xml \
    android/build.gradle \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew \
    android/gradlew \
    android/gradlew.bat \
    android/gradlew.bat \
    android/res/values/libs.xml \
    android/res/values/libs.xml \
    images/Vector.png \
    android/AndroidManifest.xml \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradlew \
    android/res/values/libs.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew.bat \ \
    Slider4.qml

HEADERS += \
    deviceinfo.h \
    ble.h


ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
