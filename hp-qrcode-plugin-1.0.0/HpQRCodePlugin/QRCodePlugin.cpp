#include "QRCodePlugin.h"
#include<QDebug>
#include "hpqrcodewidget.h"

using namespace hp;

QRCodePlugin::QRCodePlugin(QWidget *parent) :
    QObject(parent)
{
    qDebug() << "Construct function";
    m_height = 145;
    m_width = 144;
}

const QString QRCodePlugin::name()
{
    return "HP QRCode plugin";
}

const QString QRCodePlugin::description()
{
    return "HP QRCode plugin";
}

uint32_t QRCodePlugin::getPluginHints()
{
    return 0;
}

QWidget *QRCodePlugin::createWidget(QWidget *parent)
{
    qDebug() << "Create widget function";
    HpQRCodeWidget *mainWidget = new HpQRCodeWidget(parent);
    connect(mainWidget,&HpQRCodeWidget::sendQRcodeMeg,this,&QRCodePlugin::getMsgFromQRcode);
    return mainWidget;
}

void QRCodePlugin::refreshWidget(QWidget* wQRCode)
{
    HpQRCodeWidget* qrcode = qobject_cast<HpQRCodeWidget*>(wQRCode);
    if(qrcode != nullptr)
    {
         qrcode->onRefresh();
    }
}
void QRCodePlugin::registerCallBack(std::function<void(int, QString)> cbfun)
{
    m_callBackFun=cbfun;
}
void QRCodePlugin::invokCallBackFun(int status, QString msg)
{
    if(m_callBackFun)
    {
        m_callBackFun(status,msg);
    }
}
 void  QRCodePlugin::getMsgFromQRcode(int status, QString msg)
 {
     qDebug() << "call back "<<msg;
     invokCallBackFun(status,msg);
 }
