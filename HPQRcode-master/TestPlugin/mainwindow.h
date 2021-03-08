#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    void getCallBack(int status, QString msg);
    Ui::MainWindow *ui;
     QWidget *w;
};

#endif // MAINWINDOW_H
