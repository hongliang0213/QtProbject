#ifndef DEVICESOCKET_H
#define DEVICESOCKET_H
#include<QTcpSocket>
#include<QWaitCondition>  //条件变量，用来通知pc视频流到来
#include<QMutex>  // 互斥锁，用来保证环境变量的安全性   条件变量和互斥锁是配合使用的


//使用多线程同步的方式进行接收视频流 ，
//*************************************
//  改devicesocket是用来替换server中的tcpsocket的，  因为devicesocket继承了tcpsocket
//    devicesocket  又专门封装了用来接收 android数据的函数
//

class deviceSocket: public QTcpSocket
{
    Q_OBJECT
public:
    deviceSocket(QObject *parent = nullptr);
    ~deviceSocket();
    qint32  subThreadRecvData(quint8 * buf,quint32 bufSize);      //接收h264数据的函数

protected:
    bool event(QEvent *event);  //重写事件，自定义事件，

protected slots:
    void onReadyRead();  // 当数据可读的时候在这个槽函数处理
     void quitNotify();  //退出程序时，唤醒解码线程
private:
    //锁 互斥锁用于保证同一时刻只有一个线程可以访问共享资源
    QMutex m_mutex;     //m_mutex 是一个 QMutex 对象，用于控制多个线程对共享资源的访问
    QWaitCondition m_recvDataCond; //收到数据时，通过改条件变量，通知解码线程

    //标志
    bool m_recvData = false;   //接收数据标志
    bool m_quit = false; //线程退出标志

    //数据缓存
    quint8* m_buffer =Q_NULLPTR;
    qint32  m_bufferSize =0;  //buf缓冲区的大小
    qint32  m_dataSize = 0;   //这次收到的实际数据
};

#endif // DEVICESOCKET_H
