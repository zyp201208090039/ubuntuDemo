#ifndef HPQRCODEWIDGET_H
#define HPQRCODEWIDGET_H

#include <QWidget>
#include <QFrame>
#include <QMovie>
#include <QLabel>
#include <QTimer>
#include <QPixmap>
#include <QPointer>
#include <memory>
#include <QProcess>


class QNetworkReply;
class QNetworkAccessManager;

namespace Ui {
class HpQRCodeWidget;
}

namespace hp {
class HpQRCodeWidget : public QFrame
{
    Q_OBJECT

public:
    explicit HpQRCodeWidget(QWidget *parent = nullptr);
    ~HpQRCodeWidget();
signals:
    void sendQRcodeMeg(int status, QString msg);

private:
    void generateQRCode(const QString& url);
    static QString getSupportUrl(const QString& serialNumber, const QString& skuNumber, const QString& productName);
    void initialNetwork();
    void parseConfigFile();
    bool isNetWorkOnline();
    void  netWorkTimeOut();
private slots:
    void responseFromUrl(QNetworkReply *reply);


public slots:
    void onInvalid();
    void onRefresh(QString path = "");
    void onLoaded(const QString& url);

private slots:
    void onSysinfoProcessStarted();

private:
    static constexpr int QRCodeValidDuration = 180; // 10s
    static constexpr int CheckNetWorkDuration = 3;

private:
    Ui::HpQRCodeWidget *ui;
    QPointer<QMovie> m_loadingMovie;
    QLabel* m_lQRCode;
    //QLabel* m_lStatus;
    QPointer<QTimer> m_qrcodeTimer;
    QPointer<QTimer> m_checkNetWorkTimer;
    std::unique_ptr<QPixmap> m_pmQRCode;
    QString m_serialNum;
    QString m_skuNum;
    QString m_productName;
    QPointer<QProcess> m_pSysinfo;

    QString m_requestUrl;
    QString m_parseFileUrl;
    QNetworkAccessManager *manager;

    // Only for test
    //QPointer<QTimer> m_LoadingTimer;
};
}

#endif // HPQRCODEWIDGET_H
