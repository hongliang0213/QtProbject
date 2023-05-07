#ifndef SERVER_H
#define SERVER_H
#include<QObject>
#include<QTcpSocket>
#include<QTcpServer>
#include<QSize>

#include"devicesocket.h"
#include"adbprocess.h"
#include"tcpserver.h"

// 执行  写入的jar包的命令
// adb shell classpath=...  app_process /com.genymobile.scrcpy.server 1080  200000 false"
class server:public QObject
{
    Q_OBJECT

    //定义一个服务启动标记的枚举 ，用来记录服务启动状态
    enum SERVER_START_STEM{
        SSS_NULL,
        SSS_PUSH,    //将apkpush到android设备
        SSS_ENABLE_REVERSE , // 打开反向代理
        SSS_EXECUTE_SERVER,   // 执行server apk
        SSS_RUN     //SERVER   RUN

    };
public:
    server(QObject *parent = nullptr);
    //参数一，指定android序列，参数二 反向代理端口，参数三：最大尺寸，参数四：波特率
    bool start(const QString& serial,quint16 localPort,quint16 maxSize,quint16 bitRate);
    deviceSocket *getDeviceSocket();  //获取devicesocket对象


signals:
    void serverSatrtResult(bool success); // 执行失败或成功的信号
    void connectToResult(bool succcess,const QString & deviceName,const QSize& size);    //标志android是否真正连接上socket
    void onServerStop();  // server停止信号

private:
    bool StartServerByStep();
    bool removeServer();   //移除android端的文件，当第一步成功时，但其他几步失败时
    bool pushServer();   //执行push文件
    bool enableTunneReverse();   //执行打开反向代理
    bool disableTunneReverse();    // 关闭反向代理
    bool execute();     //执行 写入android设备中的jar
    bool readInfo(QString& deviceName, QSize& size);   //获取deviceName  和 size
public:
    void stop();   //关闭server服务
    TcpServer m_serverSocket;      //为了能让m_serverSocket 正确返回deviceSocket 类型数据，所以需要重写为tcpserver
    deviceSocket * m_deviceSocket  ;   //devicesocket是自己封装的继承自tcpsocket的接收视频流数据的socket类
private slots:
    //定义执行push成功的槽函数
    void onWorkProcessResult(AdbProcess::ADB_EXEC_RESULT processResult);

private:
    QString m_serial="";
    quint16 m_localPort=0;
    quint16 m_maxSize=0;
    quint16 m_bitRate=0;
    AdbProcess m_workProcess;   //  执行AdbProcess主线程
    AdbProcess m_ServerProcess;   //执行第三步时线程会阻塞，所以再开一个m_ServerProcess线程

    QString m_serverPath ="";   //要push的文件存放路径
    QString getServerPath();

    //用来保存当前运行状态，默认状态为null
    //主要是为了万一中途运行错误，知道在哪里出错，进行对应操作
    SERVER_START_STEM m_serverStartStep=SSS_NULL;

    bool m_serverCopiedToDevice =false;   //标志jar包是否成功push到android设备中
    bool m_enableReverseFlag =false;     // 标志 反向代理是否已经打开


};

#endif // SERVER_H
