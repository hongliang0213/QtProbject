#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QUdpSocket>

#include <QMainWindow>
#include<QHostAddress>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QUdpSocket * udpSocket;

private slots:
    void on_openBt_clicked();

    void on_SendBt_clicked();

    void on_CloseBt_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
