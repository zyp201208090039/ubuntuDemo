#include "hpqrcodewidget.h"
#include "ui_hpqrcodewidget.h"
#include "qrencode/qrencode.h"

#include <QDebug>
#include <QPainter>
#include <QPointF>
#include <QRectF>
#include <QByteArray>
#include <QRegularExpression>
#include <QtNetwork>

#include "HpQRCodeInterface.h"

using namespace hp;

HpQRCodeWidget::HpQRCodeWidget(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::HpQRCodeWidget),
    m_loadingMovie(new QMovie(":/HpQRCodePlugin/images/loading.gif", QByteArray(), this)),
    m_qrcodeTimer(new QTimer(this)),
    m_checkNetWorkTimer(new QTimer(this))
{
    ui->setupUi(this);
    m_lQRCode = this->findChild<QLabel*>("lableQRCode");


    m_qrcodeTimer->setSingleShot(true);
    m_qrcodeTimer->setInterval(QRCodeValidDuration * 1000);
    connect(m_qrcodeTimer, &QTimer::timeout, this, &HpQRCodeWidget::onInvalid);


    m_checkNetWorkTimer->setSingleShot(true);
    m_checkNetWorkTimer->setInterval(CheckNetWorkDuration * 1000);
    connect(m_checkNetWorkTimer, &QTimer::timeout, this, &HpQRCodeWidget::netWorkTimeOut);

    //    m_LoadingTimer = new QTimer(this);
    //    m_LoadingTimer->setSingleShot(true);

    /////////////////////
    /// \brief onRefresh

    parseConfigFile();
    initialNetwork();
    onRefresh();


}

HpQRCodeWidget::~HpQRCodeWidget()
{
    delete ui;
}

void HpQRCodeWidget::onRefresh(QString _)
{
    if(m_qrcodeTimer->isActive())
    {
        m_qrcodeTimer->stop();
    }

    m_lQRCode->setStyleSheet("QLabel { border-radius: 4px; background-color: rgba(0, 0, 0, 0); border: 1px solid #EBEBEB; opacity: 0.4; }");
    m_lQRCode->setMovie(m_loadingMovie);
    m_loadingMovie->start();


     emit sendQRcodeMeg(Loading, "二维码加载中");


    if(m_skuNum.isEmpty() || m_serialNum.isEmpty() || m_productName.isEmpty())
    {
#ifdef WIN32
        onSysinfoProcessStarted();
#else
        m_pSysinfo = new QProcess;
        connect(m_pSysinfo, &QProcess::started, this, &HpQRCodeWidget::onSysinfoProcessStarted);
        m_pSysinfo->start("sysinfo", QStringList());
#endif
    }
    else
    {
        // Call back-end service to get support URL.
        // Call onLoaded function when this process is done.

        //for get response from network
        if(!this->isNetWorkOnline())
        {
            qDebug()<<"can't connect the network";
            //onInvalid();
            m_lQRCode->setStyleSheet("QLabel { border-radius: 4px; background-color: rgba(0, 0, 0, 102); border: 0px solid #EBEBEB; opacity: 0.4; }");
            QPixmap pmError(":/HpQRCodePlugin/images/error.png");
            m_lQRCode->setPixmap(pmError);

             emit sendQRcodeMeg(NetWorkError, "网络好像出现了问题请检查");
            return;
        }

        qDebug()<<"the refresh request url is "<<this->m_requestUrl;
        manager->get(QNetworkRequest(QUrl(this->m_requestUrl)));
        m_checkNetWorkTimer->start();
    }
}

void HpQRCodeWidget::onLoaded(const QString& url)
{
    m_loadingMovie->stop();

    generateQRCode(url);
    if(m_pmQRCode != nullptr)
    {
        m_lQRCode->setStyleSheet("QLabel { border-radius: 4px; background-color: rgba(0, 0, 0, 0); }");
        m_lQRCode->setPixmap(*m_pmQRCode);
        m_qrcodeTimer->start();
    }
}

void HpQRCodeWidget::generateQRCode(const QString& url)
{
    QRcode *qrcode = QRcode_encodeString(url.toUtf8().data (), 2, QR_ECLEVEL_L, QR_MODE_8, 1);
    if(qrcode != nullptr)
    {
        m_pmQRCode.reset(new QPixmap(100, 100));
        QPainter p(m_pmQRCode.get());
        unsigned char *point = qrcode->data;
        p.setPen(Qt::NoPen);
        p.setBrush(QColor(255, 255, 255));
        p.drawRect(0, 0, m_pmQRCode->width(), m_pmQRCode->height());
        double scale = m_pmQRCode->width() * 1.0 / qrcode->width;
        p.setBrush(QColor(0, 0, 0));
        for (int y = 0; y < qrcode->width; y ++) {
            for (int x = 0; x < qrcode->width; x ++) {
                if (*point & 1) {
                    QRectF r(x * scale, y * scale, scale, scale);
                    p.drawRects(&r, 1);
                }
                point++;
            }
        }

        point = nullptr;
        QRcode_free(qrcode);
    }
}

void HpQRCodeWidget::onInvalid()
{
    m_lQRCode->setStyleSheet("QLabel { border-radius: 4px; background-color: rgba(0, 0, 0, 102); border: 0px solid #EBEBEB; opacity: 0.4; }");
    if(m_pmQRCode)
    {
        QPainter p(m_pmQRCode.get());
        p.setBrush(QColor(0, 0, 0, 102));
        p.drawRect(0, 0, m_pmQRCode->width(), m_pmQRCode->height());
        QPixmap pmError(":/HpQRCodePlugin/images/error.png");
        if(!pmError.isNull())
        {
            QPointF pt((m_pmQRCode->width() - pmError.width()) / 2.0, (m_pmQRCode->height() - pmError.height()) / 2.0);
            QRectF src(0, 0, pmError.width(), pmError.height());
            p.drawPixmap(pt, pmError, src);
        }

        m_lQRCode->setPixmap(*m_pmQRCode);
         emit sendQRcodeMeg(InvalidTime, "二维码已失效");
    }
    else
    {
        QPixmap pmError(":/HpQRCodePlugin/images/error.png");
        m_lQRCode->setPixmap(pmError);
        //emit sendQRcodeMeg(ReplyUrlErr, "二维码已失效");
    }


}

void HpQRCodeWidget::onSysinfoProcessStarted()
{
#ifdef WIN32
    m_skuNum = "SKU 1";
    m_serialNum = "Serial numner 1";
    m_productName = "HP product";
#else
    QProcess* p = qobject_cast<QProcess*>(this->sender());
    QByteArray baLine;
    QRegularExpression reSerialNum(R"(^\s*Serial Number:\s*(\w+)\s*\n$)");
    QRegularExpression reSkuNum(R"(^\s*SKU Number:\s*(\w+)\s*\n$)");
    QRegularExpression reProductName(R"(^\s*Product Name:\s*(\w+)\s*\n$)");
    while ((baLine = p->readLine()).size() != 0) {
        QString sLine(baLine);
        //qDebug() << sLine;
        QRegularExpressionMatch mSerialNum = reSerialNum.match(sLine);
        QRegularExpressionMatch mSku = reSkuNum.match(sLine);
        QRegularExpressionMatch mProductName = reProductName.match(sLine);
        if(m_serialNum.isEmpty() && mSerialNum.hasMatch())
        {
            qDebug() << "Serial number: " << mSerialNum.captured(1);
            m_serialNum = mSerialNum.captured(1);
        }
        else if(m_skuNum.isEmpty() && mSku.hasMatch())
        {
            m_skuNum = mSku.captured(1);
            qDebug() << "SKU number: " << mSku.captured(1);

        }
        else if(m_productName.isEmpty() && mProductName.hasMatch())
        {
            m_productName = mProductName.captured(1);
            qDebug() << "Product Name: " << mProductName.captured(1);
        }
    }
#endif

    if(m_skuNum.isEmpty())
    {
        m_skuNum="4810GB01C006";
    }
    if(m_serialNum.isEmpty())
    {
        m_serialNum="ABC92100CC";
    }
    if(m_productName.isEmpty())
    {
        m_productName="86B1";
    }

    if(!m_skuNum.isEmpty() && !m_serialNum.isEmpty() && !m_productName.isEmpty())
    {
        // Call back-end service to get support URL.
        // Call onLoaded function when this process is done.


        qDebug()<<" penSSL版本 QSslSocket="<<QSslSocket::sslLibraryBuildVersionString();
        qDebug() << "OpenSSL支持情况:" << QSslSocket::supportsSsl();

        if(this->m_parseFileUrl.isEmpty())
        {
            QString surl=QString("https://xiaowei.ext.hp.com/xiaowei/client/v1/qilin/getqrcodeurl?pn=%1&sn=%2&ssid=%3").arg(m_skuNum).arg(m_serialNum).arg(m_productName);
            qDebug()<<"the request url is "<< surl;
            this->m_requestUrl=surl;
        }
        else
        {
              QString argFormat=QString("?pn=%1&sn=%2&ssid=%3").arg(m_skuNum).arg(m_serialNum).arg(m_productName);
              this->m_requestUrl=this->m_parseFileUrl+argFormat;
        }

        qDebug()<<"the request url is "<< this->m_requestUrl;
        //connect(manager, &QNetworkAccessManager::finished,this, &HpQRCodeWidget::responseFromUrl);
        manager->get(QNetworkRequest(QUrl(this->m_requestUrl)));

    }
    else
    {

        onInvalid();
        emit sendQRcodeMeg(ParameterErr, "未能正确的获取机器参数");


        if(!m_checkNetWorkTimer->isActive())
        {
            m_checkNetWorkTimer->start();
        }
    }
}

QString HpQRCodeWidget::getSupportUrl(const QString& serialNumber, const QString& skuNumber, const QString& productName)
{
    // Call backend service to get the support URL.

    return  QString("serrialnumber=")+serialNumber+"skunumeber="+skuNumber+"productNmame="+productName;


    return "www.baidu.com";
}

void HpQRCodeWidget::initialNetwork()
{

    manager = new QNetworkAccessManager(this);
    connect(manager, &QNetworkAccessManager::finished,this, &HpQRCodeWidget::responseFromUrl);

}

void HpQRCodeWidget::parseConfigFile()
{
    this->m_parseFileUrl="https://xiaoweitest.ext.hp.com:8521/xiaowei/client/v1/qilin/getqrcodeurl";
//    QDir(dir);
//    QFile fp;
//    QString path("./"),filename("config.txt");
//    QDebug qdebug(QtWarningMsg);
//    fp.setFileName(path+filename);
//    qdebug<<"the config directory is "<< dir.current();
//    if(fp.exists())
//    {
//        QString(text);
//        QByteArray content;

//        fp.open(QIODevice::ReadOnly);
//        while(!fp.atEnd())
//        {
//            content = fp.readAll();
//        }
//        qdebug<<"file content is :"<<endl<<content<<endl;


//        QJsonParseError err_rpt;
//        QJsonDocument  json = QJsonDocument::fromJson(content, &err_rpt);

//        if(err_rpt.error != QJsonParseError::NoError)
//        {
//            qDebug() << "config file JSON fromat error";
//        }
//        else
//        {
//            if(json.isObject())
//            {
//                QJsonObject rootObj = json.object();

//                if(rootObj.contains(("url")))
//                {
//                    QJsonValue urlObj= rootObj.value("url");

//                    QString url=urlObj.toString();
//                    this->m_parseFileUrl=urlObj.toString();
//                }
//            }
//        }

//    }
//    else
//    {
//        qdebug<<"config file not exsit";
//    }

}

bool HpQRCodeWidget::isNetWorkOnline()
{
    QNetworkConfigurationManager mgr;
    return mgr.isOnline();
}

void HpQRCodeWidget::netWorkTimeOut()
{

  onInvalid();

  emit sendQRcodeMeg(NetWorkError, "网络好像出现了问题请检查");

}

void HpQRCodeWidget::responseFromUrl(QNetworkReply *reply)
{

    // emit sendQRcodeMeg("test");

    if(m_checkNetWorkTimer->isActive())
    {
        m_checkNetWorkTimer->stop();
    }

    qDebug()<<"from response data ";
    QByteArray data= reply->readAll();
    QString responseData =data;
    qDebug()<<responseData;

    int status=reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).value<int>();
    qDebug()<<"the response status is "<<status;

    if(status==0||responseData.isEmpty())
    {
        m_loadingMovie->stop();
        onInvalid();  
        emit sendQRcodeMeg(NetWorkError, "网络好像出现了问题请检查");
        return;
    }
    QJsonParseError err_rpt;
    QJsonDocument  json = QJsonDocument::fromJson(data, &err_rpt);

    if(err_rpt.error != QJsonParseError::NoError)
    {
        qDebug() << "JSON fromat error";
    }
    else
    {
        if(json.isObject())
        {
            QJsonObject root_Obj = json.object();

            if(root_Obj.contains(("code")))
            {

                QJsonValue code = root_Obj.value("code");

                if(code.isDouble())
                {
                    int codeNum=code.toInt();

                    if(codeNum==200)
                    {
                        QJsonValue message = root_Obj.value("message");
                        QString url=message.toString();
                        this->onLoaded(url);
                        qDebug() << "the response url is "<<url;
                         emit sendQRcodeMeg( Successful, "成功");
                    }
                    else
                    {
                        onInvalid();  
                        emit sendQRcodeMeg(ParameterErr, "URL 的状态码返回值非200 ");

                    }

                }

            }

        }

    }

}
