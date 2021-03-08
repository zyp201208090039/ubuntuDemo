#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QLibrary>
#include <QPluginLoader>
#include "HpQRCodeInterface.h"
#include <QDebug>
#include <QVBoxLayout>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
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
        w->setParent(ui->widget);
       // ui->label
        //QVBoxLayout* mainLayout = new QVBoxLayout(this);
        //mainLayout->addWidget(w);
        //ui->horizontalLayout->addWidget(w);
         // ui->widget->addWidget(w);
        //std::function<void(int, QString)> cb = std::bind(&MainWindow::getCallBack, this);

         app->registerCallBack([this](int status, QString msg)
         {
                getCallBack(status, msg);
         });


//        QTimer::singleShot(5000, [app, this]{
//            app->refreshWidget(w);
//        });


    }
    else{
        qDebug() << "加载插件失败";
    }




}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::getCallBack(int status, QString msg)
{
    qDebug() << "status"<< status <<"  msg is "<<msg;
    ui->label->setText(msg);
}
