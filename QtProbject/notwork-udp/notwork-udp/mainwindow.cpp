#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QUdpSocket>
#include<QMessageBox>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    udpSocket = new QUdpSocket(this);


}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_openBt_clicked()
{
    //将本地端口设置进去,连接成功会返回一个true

    if(udpSocket->bind(ui->LocalPort->text().toUInt())== true){
        QMessageBox::information(this,"提示","连接成功");
    }else {
        QMessageBox::information(this,"提示","连接失败");
    }
    // 打开读取数据的信号，即收到数据就触发信号
    connect(udpSocket,&QUdpSocket::readyRead,[=](){
       //将接收到的数据进行处理，显示在textEdit上
        //如果没有将数据读取完则返回true,判断是否读完了
        while (udpSocket->hasPendingDatagrams()) {
            QByteArray array;
            //将array设置为读取的数据的长度，调整大小
            array.resize(udpSocket->pendingDatagramSize());
            //固定写法 ，array.data()返回一个指向存储在字节数组中的数据的指针
            //因为发送来的数据需要用qbytearray 接收
            udpSocket->readDatagram(array.data(),array.size());

            QString buf;
            buf=array.data();

            ui->RecvEdit->appendPlainText(buf);

        }


    });
}


void MainWindow::on_SendBt_clicked()
{
    //目标端口
    quint16 port;
    //发送数据
    QString sendBuf;
    //目标ip
    QHostAddress address;

    //将目标ip转为 QHostAddress类型
    address.setAddress(ui->AimIp->text());

    sendBuf =ui->SendEdit->text();
    // 端口必须转为int类型数据
    port=ui->AimPort->text().toUInt();

    //因为第一个参数是const qbytearray 类型数据，所以必须转，
    //  而接收别人发来的数据也是
    udpSocket->writeDatagram(sendBuf.toLocal8Bit().data(),sendBuf.length(),address,port);


}


void MainWindow::on_CloseBt_clicked()
{
    udpSocket->close();
    QMessageBox::information(this,"提示","关闭成功");
}

