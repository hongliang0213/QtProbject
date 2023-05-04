#include "widget.h"
#include "ui_widget.h"

#include<QSerialPortInfo>
#include<QMessageBox>
# include<QDebug>
#include<QString>
//#include<QSimpleLed.h>
#include<QTimer>
#include <QApplication>
#include <QLabel>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    //设置窗口ui和name
    setWindowTitle("COM");
    setWindowIcon(QIcon(":/icon/COM001.png"));

    //设置led灯***************************
//    stepLedGreen = new QSimpleLed(this, QSimpleLed::GREEN);
//    stepLedRed = new QSimpleLed(this, QSimpleLed::RED);
//    //定义开关大小
//    stepLedGreen->setFixedSize(180,60);
//    stepLedRed->setFixedSize(180,60);
//    //定义位置
//    stepLedGreen->move(100,342);
//    stepLedRed->move(200,342);
//    //开关状态
//    stepLedGreen->setStates(QSimpleLed::OFF);
//    stepLedRed->setStates(QSimpleLed::ON);
//    stepLedGreen->show();
//    stepLedRed->show();

    //************************************时钟
    // 创建一个标签对象，用于显示当前日期和时间
        QLabel *label = new QLabel(this);
        label->setStyleSheet("font-size: 30px; color: grey;");

        label->setFixedSize(300,50);
        label->move(295,348);
        // 创建一个定时器，每隔1秒钟更新一次时间
        QTimer *timer = new QTimer();
        timer->setInterval(1000);

        QObject::connect(timer, &QTimer::timeout, [=]() {
            QDateTime currentDateTime = QDateTime::currentDateTime();
            QString dateTimeString = currentDateTime.toString("yyyy-MM-dd hh:mm:ss");
            label->setText(dateTimeString);
        });
        // 设置窗口透明度
//        label->setWindowOpacity(0.5);
        // 显示标签
        label->show();
        // 启动定时器
        timer->start();


    //**************************************
    /*
     * 自动读取串口硬件名字，并用const修饰，<<写入到字符list中，再用additems显示
     */
    //用来接收每个可用串口，然后写入到serial中
    QStringList serialNamePort;
    //获取当前全部的可用串口
    QList<QSerialPortInfo> list =QSerialPortInfo::availablePorts();
    //使用foeach循环将每个可用串口写到qstringlist中,使用引用传递
    foreach (const QSerialPortInfo &port, list) {
        //将每个可用串口的name写入到list中
        serialNamePort<<port.portName();
    }
    //将得到的每个可用串口写入到ui中显示
    ui->serialCb->addItems(serialNamePort);

    //串口对象
    serialport =new QSerialPort(this);
    //接收数据
    connect(serialport,&QSerialPort::readyRead,[=](){
        //只要一读到数据就让他显示在大框中
        QString buf;
        //接收读到的所有数据并转为qstring
        buf=QString(serialport->readAll());
        //写入读到的数据到大框中
        ui->reviceTextEdit->appendPlainText(buf);
    });

}

Widget::~Widget()
{
    delete ui;
}

//初始化串口
void Widget::on_openBt_clicked()
{
    qDebug()<<"按下链接按钮";
    //定义一些初始化的值
    QSerialPort::BaudRate buadRate;
    QSerialPort::DataBits dataBits;
    QSerialPort::StopBits stopBits;
    QSerialPort::Parity checkBits;

    //判断ui界面选择的参数
    if(ui->bauntRateCb->currentText() =="4800"){
        //QSerialPort::Baud4800是枚举值，buadRate只能接受QSerialPort枚举值,不能自己赋值
        //按键选择的是字符串数据，但我们要把定义的变量设置为规定的格式，所以需要进行判断
        buadRate=QSerialPort::Baud4800;
    }else if(ui->bauntRateCb->currentText() =="9600"){
        buadRate=QSerialPort::Baud9600;
    }else if(ui->bauntRateCb->currentText() =="115200"){
        buadRate=QSerialPort::Baud115200;
    }
    //写入数据位参数
    if(ui->dataCb->currentText()=="5"){
        dataBits=QSerialPort::Data5;
    }else if(ui->dataCb->currentText()=="6"){
        dataBits=QSerialPort::Data6;
    }else if(ui->dataCb->currentText()=="7"){
        dataBits=QSerialPort::Data7;
    }else if(ui->dataCb->currentText()=="8"){
        dataBits=QSerialPort::Data8;
    }
    //写入 停止位数据
    if(ui->stopCb->currentText()=="1"){
        stopBits=QSerialPort::OneStop;
    }else if(ui->stopCb->currentText()=="1.5"){
        stopBits=QSerialPort::OneAndHalfStop;
    }else if(ui->stopCb->currentText()=="2"){
        stopBits=QSerialPort::TwoStop;
    }
    //写入 check数据
    checkBits=QSerialPort::NoParity;

    //设置端口号,以及其他的属性
    serialport->setPortName(ui->serialCb->currentText());
    serialport->setBaudRate(buadRate);
    serialport->setDataBits(dataBits);
    serialport->setStopBits(stopBits);
    serialport->setParity(checkBits);

    //判断是否打开成功
    if(serialport->open(QIODevice::ReadWrite)== true)
    {
        //连接成功
        QMessageBox::information(this,"提示","连接成功");
        //先灭红
//        stepLedRed->setStates(QSimpleLed::OFF);
//        //绿闪
//        stepLedRed->setStates(QSimpleLed::BLINK);
//        Sleep(1200);
//        //绿亮
//        stepLedGreen->setStates(QSimpleLed::ON);


    }else
    {
        //连接失败
        QMessageBox::warning(this,"提示","连接失败");
//        stepLedRed->setStates(QSimpleLed::BLINK);
//        Sleep(1200);
//        stepLedRed->setStates(QSimpleLed::ON);
    }

}
//延时函数
void Widget::Sleep(int msec)
{
    QTime dieTime = QTime::currentTime().addMSecs(msec);
    while( QTime::currentTime() < dieTime )
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}



//关闭按钮
void Widget::on_closeBt_clicked()
{
    //  关闭连接
    serialport->close();
}

//发送数据
void Widget::on_sendBt_clicked()
{
    //write里面接收的数据为const char *data，必须将qstring数据转为const char *data
    //  const char* testChar = str.toLocal8Bit().data(); 能将str转为
    serialport->write(ui->sendEdit->text().toLocal8Bit().data());
}

//清空数据
void Widget::on_cleartBt_clicked()
{
    //清空显示的数据
    ui->reviceTextEdit->clear();
}

