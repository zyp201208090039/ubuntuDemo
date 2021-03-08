TEMPLATE = subdirs

SUBDIRS += \
    widget \
    HpQRCodePlugin \
    TestPlugin

#widget.depends = HpQRCodePlugin
TestPlugin.depends = HpQRCodePlugin
