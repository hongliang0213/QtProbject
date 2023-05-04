#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QMessageBox>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //  建立连接用 tcpserver   ，发送和接收数据用tcp socket
    tcpService=new QTcpServer(this);
    tcpSocket =new QTcpSocket(this);
    this->setWindowTitle("Service");

    //设置led灯***************************
//    stepLedGreen = new QSimpleLed(this, QSimpleLed::GREEN);

    //定义开关大小
//    stepLedGreen->setFixedSize(60,20);

//    //定义位置
//    stepLedGreen->move(100,342);
//    stepLedRed->move(200,342);
//    //开关状态
//    stepLedGreen->setStates(QSimpleLed::OFF);
//    stepLedRed->setStates(QSimpleLed::ON);
//    stepLedGreen->show();
//    stepLedRed->show();


    connect(tcpService,&QTcpServer::newConnection,[=](){
        //newConnection,获取所有可用连接的信号，然后获取到那个链接,然后使用tcpsocket接受连接

        //nextPendingConnection 连接返回的是tcpsocket对象
        tcpSocket=tcpService->nextPendingConnection();

        //获取到tcpsocket之后，有一个读取全部数据的信号，使用改信号将所有值显示出来
        //改信号就是当有任何新的值读取到时，触发改信号
        connect(tcpSocket,&QTcpSocket::readyRead,[=](){
            //读取数据，并显示
            QString buf;
            buf =tcpSocket->readAll();
            ui->plainTextEdit->appendPlainText(buf);

        });

    });

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
    //打开服务器
    //监听来自所有人的链接,第二个参数为端口号，自己输入的port,需要转换类型
    tcpService->listen(QHostAddress::Any,ui->portEdit->text().toUInt());
    QMessageBox::information(this,"提示","启动成功");
    this->setWindowTitle("服务器正在运行 正在监听客户机连接......");
}


void MainWindow::on_closeButton_clicked()
{
    tcpService->close();
    QMessageBox::information(this,"提示","关闭成功");
    this->setWindowTitle("Service");
}


void MainWindow::on_pushButton_3_clicked()
{
    //发送数据函数
    //将ui->sendEdit->text() 里面的q身体日ing转为char *
    tcpSocket->write(ui->sendEdit->text().toLocal8Bit().data());
//    QMessageBox::information(this,"提示","发送成功");
}

