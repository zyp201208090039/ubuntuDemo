include(../HpQRCodeInterface.pri)
include(./qrencode/qrencode.pri)

QT += gui widgets
QT +=network

TEMPLATE = lib
CONFIG += plugin

TARGET = $$qtLibraryTarget(hp-qrcode-plugin)
DESTDIR = ./
target.path = $${PLUGIN_INSTALL_DIRS}
INSTALLS += target

message($${PLUGIN_INSTALL_DIRS})

INCLUDEPATH += \
                $$PROJECT_COMPONENTSOURCE

HEADERS += \
    QRCodePlugin.h \
    hpqrcodewidget.h

SOURCES += \
    QRCodePlugin.cpp \
    hpqrcodewidget.cpp

# Default rules for deployment.
unix {
    target.path = $$[QT_INSTALL_LIBS]/about-extensions/
}
!isEmpty(target.path): INSTALLS += target

FORMS += \
    hpqrcodewidget.ui

RESOURCES += \
    HpQRCodePlugin.qrc
