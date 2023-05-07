#include "devicesocket.h"
#include<qcoreapplication.h>
#include<QThread>
#include<QMutexLocker>
#include"common/qscrcpyevent.h"
deviceSocket::deviceSocket(QObject *parent)
    :QTcpSocket (parent)
{
     connect(this,&deviceSocket::readyRead,this,&deviceSocket::onReadyRead);  //接收到数据进行读取的信号
     connect(this, &deviceSocket::disconnected, this, &deviceSocket::quitNotify); //android断开连接时，
     connect(this, &deviceSocket::aboutToClose, this, &deviceSocket::quitNotify); //主动close进程时，都进行关闭线程
}

deviceSocket::~deviceSocket()
{

}
/**
  接收buf数据的函数
 * @brief deviceSocket::subThreadRecvData
 * @param buf    h264数据存储在buf中
 * @param bufSize  buf大小
 * @return
 */
qint32 deviceSocket::subThreadRecvData(quint8 *buf, quint32 bufSize)
{

    //保证在子线程中调用 ， 确保主线程  不等于 当前线程
    Q_ASSERT(QCoreApplication::instance()->thread() != QThread::currentThread());
    if(m_quit){
        return 0;   //退出标志为true则直接返回
    }

    QMutexLocker locker(&m_mutex);   //互斥锁用于保证同一时刻只有一个线程可以访问共享资源

    m_buffer = buf;
    m_bufferSize = bufSize;
    m_dataSize =0;

    //发送事件，调用onReadyRead  ，需要使用common中的自定义事件，当执行这一步触发自定义事件时，可以重写事件，让其执行想要的步骤
    DeviceSocketEvent* getDataEvent = new DeviceSocketEvent();
    QCoreApplication::postEvent(this, getDataEvent);

    while (!m_recvData) {
        m_recvDataCond.wait(&m_mutex);
    }

    m_recvData = false;  //接收完数据标志位变为false
    return m_dataSize;  //返回实际收到的数据


}

/**
 * @brief deviceSocket::event
 * @param event
 * @return
 * 重写自定义事件
 */
bool deviceSocket::event(QEvent *event)
{
    if (event->type() == QScrcpyEvent::DeviceSocket) {  //如果事件名称是devicesocket时则执行
        //当执行devicesocket事件时，先尝试去收取数据，
            onReadyRead();
            return true;
        }
        return QTcpSocket::event(event);    //如果没有收到devicesocket事件时，直接交给父类去处理
}


void deviceSocket::onReadyRead()
{
    QMutexLocker locker(&m_mutex);
    //如果我们设置了buf指针，并且当前有可读的数据，那么就接收数据
    if(m_buffer && 0 < bytesAvailable()){
        //接收数据
        qint64 readSize = qMin(bytesAvailable(),(qint64)m_bufferSize);  // 挑小的值收取数据
        m_dataSize = read((char*)m_buffer,readSize);  //read函数是读取接收数据的函数 ，将读取到的数据放在m_dataSize中，

        m_buffer = Q_NULLPTR;
        m_bufferSize = 0;
        m_recvData =true;
        m_recvDataCond.wakeOne();

    }
}

// 当退出时，唤醒解码线程
void deviceSocket::quitNotify()
{
    m_quit = true;
    QMutexLocker locker(&m_mutex);
    if (m_buffer) {
        m_buffer = Q_NULLPTR;
        m_bufferSize = 0;
        m_recvData = true;
        m_dataSize = 0;
        m_recvDataCond.wakeOne();
    }
}
