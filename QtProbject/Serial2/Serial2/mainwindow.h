#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<QSerialPort>
#include <QLCDNumber>
#include<QTimer>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QSerialPort *serialport;
    //延时函数
    void Sleep(int msec);

    //设置时钟对象
    QLCDNumber *m_pLCD;
private:
    Ui::MainWindow *ui;
    void on_openBt_clicked();

    void on_closeBt_clicked();

    void on_sendBt_clicked();

    void on_cleartBt_clicked();

};
#endif // MAINWINDOW_H
