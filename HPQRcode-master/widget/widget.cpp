#include "widget.h"
#include "ui_widget.h"
#include <QLibrary>
#include <QPluginLoader>
#include "HpQRCodeInterface.h"
#include <QDebug>
#include <QVBoxLayout>
#include <QTimer>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
#ifdef WIN32
    QPluginLoader loader("../HpQRCodePlugin/hp-qrcode-plugind.dll");
#else
    QPluginLoader loader("../HpQRCodePlugin/libhp-qrcode-plugin.so");
#endif
    QObject *plugin = loader.instance();
    if (plugin) {
        hp::QRCodeInterface *app = qobject_cast<hp::QRCodeInterface*>(plugin);
        w = app->createWidget(this);
        QVBoxLayout* mainLayout = new QVBoxLayout(this);
        mainLayout->addWidget(w);

        QTimer::singleShot(5000, [app, this]{
            app->refreshWidget(w);
        });
    }
    else{
        qDebug() << "加载插件失败";
    }
}

Widget::~Widget()
{
    if(w)
        delete w;
    delete ui;
}

