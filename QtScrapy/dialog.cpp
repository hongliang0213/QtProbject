#include "dialog.h"
#include "ui_dialog.h"
#include<QDebug>
#include"adb/adbprocess.h"
#include"server/server.h"
#include<QByteArray>
#include<QBuffer>
#include<input.h>
#include<keycodes.h>
#include"controlevent.h"
#include<videoform.h>
#include<QMessageBox>
#include"QMessageBox"
#include <QtGui>
Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    // 设置圆角边角
//    this->setStyleSheet("QWidget { border-radius: 50%; }");

    setWindowFlag(Qt::FramelessWindowHint); //关闭顶部标题栏
    m_isPressed = false;// 允许窗口移动
    connect(ui->closeButton, &QPushButton::clicked, this, &QWidget::close);
    connect(ui->minButton, &QPushButton::clicked, this, &QWidget::showMinimized);


    setWindowIcon(QIcon(":/res/icon.png"));

    //判断执行结果进行相应的日志输出
    connect(&m_adb,&AdbProcess::adbProcessResult,this,[=](AdbProcess::ADB_EXEC_RESULT processResult){
        QString log = "";
        bool newLine = true;
        switch (processResult) {
        case AdbProcess::AER_SUCCESS_START:
            log = "adb run success";
            newLine = false;
            break;
        case AdbProcess::AER_ERROR_EXEX:
            log = m_adb.getErrorOut();
            break;
        case AdbProcess::AER_ERROR_MISSING_BINARY:
            log = "adb not find";
            break;
        case AdbProcess::AER_SUCCESS_EXEX:
            QStringList args = m_adb.arguments();
            if (args.contains("devices")) {
                QStringList devices = m_adb.getDevicesSerialFromStdOut();
                if (!devices.isEmpty()) {
                    ui->SerialEdit->setText(devices.at(0));
                    outLog("Device Serial:"+devices.at(0),false);
                }
            }
            if (args.contains("show") && args.contains("wlan0")) {
                QString ip = m_adb.getDeviceIpFromStdOut();
                if (!ip.isEmpty()) {
                    ui->deviceIpEdit->setText(ip);
                    outLog("Device Ip:"+ip,false);
                }
            }
            break;
        }

        if (!log.isEmpty()) {
            outLog(log, newLine);
        }
            });


}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::outLog(const QString &log, bool newLine)
{
     ui->outEdit->append(log);
     if(newLine){
         ui->outEdit->append("<br/>");
     }
}

//Qprocess  启动外部进程
//void Dialog::on_TextButton_clicked()
//{

//    AdbProcess *myProcess = new AdbProcess(this);
//    QStringList arguments;
//    arguments << "devices" ;   //传递给外部进程的参数

    //当执行状态为成功时，打印序列号
//    connect(myProcess,&AdbProcess::adbProcessResult,this,[=](AdbProcess::ADB_EXEC_RESULT ProcessResult){
//        if(AdbProcess::AER_SUCCESS_EXEX == ProcessResult){
//            qDebug()<<myProcess->getDevicesSerialFromStdOut().join("*");
//            qDebug()<<myProcess->getDeviceIpFromStdOut();
//        }

//    });
    //adb类中封装的用来执行指令的函数,adb命令不能同时运行
    //myProcess-> removeFile("","/sdcard/text.txt");
    //myProcess->reverse("","scrapy",5037);
    //myProcess->removeReverse("","scrapy");
    //myProcess->execute("",arguments);
//    myProcess->getIPShell("");
    //myProcess->getDeviceShell("");

    //参数一，要启动的外部程序  ，参数二:传递给外部进程的参数
    //s_path是自己在adb类中写的动态获取adb路径的函数
//    myProcess->start(AdbProcess::getAdbPath(),Q_NULLPTR);

//}

/**
 * @brief Dialog::on_StartServerButton_clicked
 * 开启server服务
 */
void Dialog::on_StartServerButton_clicked()
{


    //判断端口是否为空就退出函数，不为空
    if(ui->SerialEdit->text().isEmpty()){
        QMessageBox::information(this,"tips","not fond device",QMessageBox::Ok);
        return;
    }

    if (!m_VideoForm) {
        m_VideoForm = new VideoForm(ui->SerialEdit->text().trimmed());
    }
    m_VideoForm->show();





//     m_server.start("",8080,1080,8000000);
//     //连接时同时显示视屏窗口
//     m_videoWidget->show();
}
/**
 * @brief Dialog::on_StopServerButton_clicked
 *  关闭server服务
 */
void Dialog::on_StopServerButton_clicked()
{
    if(m_VideoForm){
        m_VideoForm->close();
    }
}






/**
 * @brief Dialog::on_pushButton_clicked
 * 测试控制指令  ,改功能不使用， 是发送控制指令的顺序代码
 */
void Dialog::on_pushButton_clicked()
{
//    QPoint point1(100,100);
//    QRect pos1(point1,m_videoWidget->frameSize());  // 将需要按下的点和显示的size放进去进行构造
//    ControlEvent* touchEvent1= new ControlEvent(ControlEvent::CET_TOUCH); //写入鼠标事件
//    //  分别写入鼠标按下的按键，动作，和位置
//    touchEvent1->setTouchEventData(0,AMOTION_EVENT_ACTION_DOWN,pos1);
//    m_controller.postControlEvent(touchEvent1);  //发送事件，触发整个函数运行，


//    QPoint point2(200,200);
//    QRect pos2(point2,m_videoWidget->frameSize());
//    ControlEvent* touchEvent2= new ControlEvent(ControlEvent::CET_TOUCH); //写入鼠标事件
//    //  分别写入鼠标按下的按键，动作，和位置
//    touchEvent2->setTouchEventData(1,AMOTION_EVENT_ACTION_DOWN,pos2);
//    m_controller.postControlEvent(touchEvent2);  //发送事件，触发整个函数运行，



//    ControlEvent mouseEvent(ControlEvent::CET_MOUSE); //写入鼠标事件
//    //  分别写入鼠标按下的按键，动作，和位置
//    mouseEvent.setMouseEventData(AMOTION_EVENT_ACTION_DOWN,AMOTION_EVENT_BUTTON_PRIMARY,pos);
//    QByteArray byteArray = mouseEvent.serializeData();  //调用接口返回拼装好的bytter数据
//    //发送指令
//    m_server.getDeviceSocket()->write(byteArray.data(),byteArray.length());




//    QByteArray byteArray;
//    QBuffer buffer(&byteArray);
//    buffer.open(QBuffer::WriteOnly);  //打开写入模式才能写入数据
//    //构造控制指令
//    //type  mouse 控制指令
//    buffer.putChar(2);
//    //鼠标按下 AMOTION_EVENT_ACTION_DOWN
//    buffer.putChar(AMOTION_EVENT_ACTION_DOWN);
//    //左键按下 AMOTION_EVENT_BUTTON_PRIMARY int数据 放入到 四个字节中去
//    //假设数据为 0x 00 11 22 33 四个字节数据
//    // putchar 会只取一个字节数据，即只取最后一个字节数据
//    buffer.putChar(AMOTION_EVENT_BUTTON_PRIMARY>>24); //把00 移动到 末尾，取到00
//    buffer.putChar(AMOTION_EVENT_BUTTON_PRIMARY>>16); //把11移动到末尾  取 11
//    buffer.putChar(AMOTION_EVENT_BUTTON_PRIMARY>>8); //把22 移动到末尾取  22
//    buffer.putChar(AMOTION_EVENT_BUTTON_PRIMARY);
//    int x =100;
//    buffer.putChar(x>>8); //放入倒数第二个字节
//    buffer.putChar(x); //放入第一个字节
//    int y =100;
//    buffer.putChar(x>>8); //放入倒数第二个字节
//    buffer.putChar(y); //放入第一个字节

//    // w
//    buffer.putChar(m_videoWidget->frameSize().width()>>8);
//    buffer.putChar(m_videoWidget->frameSize().width());

//    // h
//    buffer.putChar(m_videoWidget->frameSize().height()>>8);
//    buffer.putChar(m_videoWidget->frameSize().height());

//    buffer.close();

}

//更新设备
void Dialog::on_updateDeviceButton_clicked()
{
    outLog("update device",false);
    m_adb.execute("",QStringList()<<"devices");
}

void Dialog::on_startAdbButton_clicked()
{
    outLog("start devices adbd...", false);
    // adb tcpip 5555
    QStringList adbArgs;
    adbArgs << "tcpip";
    adbArgs << "8080";
    m_adb.execute(ui->SerialEdit->text().trimmed(), adbArgs);
    ui->devicePortEdit->setText("8080");
}

//获取ip ，通过执行信号输出到框中
void Dialog::on_getIpButton_clicked()
{
    outLog("get ip...", false);

//    // adb shell ip -f inet addr show wlan0 ,  执行获取ip的命令，再再主函数的信号中实现
//    QStringList adbArgs;
//    adbArgs << "shell";
//    adbArgs << "ip";
//    adbArgs << "-f";
//    adbArgs << "inet";
//    adbArgs << "addr";
//    adbArgs << "show";
//    adbArgs << "wlan0";
//    m_adb.execute("", adbArgs);
      m_adb.getIPShell(ui->SerialEdit->text().trimmed());
}

//执行无线连接 ,必须先进行有线连接后，  才能进行无线连接
void Dialog::on_wirelessButton_clicked()
{
    outLog("wireless connect...", false);
        QString addr = ui->deviceIpEdit->text().trimmed();
        if (!ui->devicePortEdit->text().isEmpty()) {
            addr += ":";
            addr += ui->devicePortEdit->text().trimmed();
        }
        // connect ip:port
        QStringList adbArgs;
        adbArgs << "connect";
        adbArgs << addr;
        m_adb.execute(ui->SerialEdit->text().trimmed(), adbArgs); //指定设备执行命令
}

//隐藏标题栏后重写鼠标事件，才能进行移动
void Dialog::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_isPressed = true;
        m_dragPosition = event->globalPos() - frameGeometry().topLeft();
        event->accept();
    }
}
void Dialog::mouseMoveEvent(QMouseEvent *event)
{
    if (m_isPressed) {
        move(event->globalPos() - m_dragPosition);
        event->accept();
    }
}
void Dialog::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_isPressed = false;
        event->accept();
    }
}



void Dialog::on_inforButton_clicked()
{
    QMessageBox::information(this,"tips","v1.0");
}
