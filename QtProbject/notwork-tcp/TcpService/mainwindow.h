#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
//要使用这两个头文件时，必须在pro文件导入network
#include<QTcpServer>
#include<QTcpSocket>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QTcpServer * tcpService;
    QTcpSocket * tcpSocket;

private slots:
    void on_pushButton_clicked();

    void on_closeButton_clicked();

    void on_pushButton_3_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
