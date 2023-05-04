#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include<QSerialPort>
#include<QTime>
#include<QSimpleLed.h>
#include <QLCDNumber>
QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

    QSerialPort *serialport;

    //设置led灯
//    QSimpleLed *stepLedGreen = nullptr;
//    QSimpleLed *stepLedRed = nullptr;

    //延时函数
    void Sleep(int msec);

    //设置时钟对象
    QLCDNumber *m_pLCD;
private slots:
    void on_openBt_clicked();

    void on_closeBt_clicked();

    void on_sendBt_clicked();

    void on_cleartBt_clicked();

private:
    Ui::Widget *ui;
};
#endif // WIDGET_H
