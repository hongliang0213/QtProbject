#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QMessageBox>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("Clinet");
    tcpsocket=new QTcpSocket(this);


    //连接成功后,获取当前本地连接
    connect(tcpsocket,&QTcpSocket::connected,[=](){
        //使用这个连接，获取数据
        connect(tcpsocket,&QTcpSocket::readyRead,[=](){
            //将获取到的数据写入到框中
             ui->reviceTextEdit->appendPlainText(tcpsocket->readAll());

        });


    });

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_openButton_clicked()
{
    //新建连接
    tcpsocket->connectToHost(ui->ipEdit->text(),ui->portEdit->text().toUInt());
    QMessageBox::information(this,"提示","启动成功");
    this->setWindowTitle("连接成功");



}


void MainWindow::on_closeButton_clicked()
{
   tcpsocket->close();
   QMessageBox::information(this,"提示","关闭成功");
   this->setWindowTitle("Clinet");
}


void MainWindow::on_sendButton_clicked()
{
    //发送数据
    tcpsocket->write(ui->sendEdit->text().toLocal8Bit().data());
}

