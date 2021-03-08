#ifndef MYWIDGET_H
#define MYWIDGET_H

#include <QObject>
#include <QWidget>
#include <QPushButton>

#include "HpQRCodeInterface.h"

namespace hp {
    class QRCodePlugin : public QObject, public QRCodeInterface
    {
        Q_OBJECT
        Q_PLUGIN_METADATA(IID "com.hp.QRCodeInterface")
        Q_INTERFACES(hp::QRCodeInterface)
    public:
        explicit QRCodePlugin(QWidget *parent = nullptr);
        virtual const QString name();
        virtual const QString description();
        virtual uint32_t getPluginHints();
        virtual QWidget *createWidget(QWidget *parent);
        void refreshWidget(QWidget* wQRCode);
        void registerCallBack(std::function<void(int, QString)>);
    private:
        void invokCallBackFun(int status, QString msg);
    private slots:
        void getMsgFromQRcode(int status, QString msg);
    signals:
    };
}

#endif // MYWIDGET_H
