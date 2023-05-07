#ifndef CONTROLLER_H
#define CONTROLLER_H
#include<QObject>
#include<QPointer>
#include"devicesocket.h"
// 该类主要封装发送指令的函数
//class deviceSocket;
class ControlEvent;
class Controller: public QObject
{
    Q_OBJECT
public:
    Controller(QObject *parent=nullptr);

    void setDeviceSocket(deviceSocket *deviceSocket);  // 写入socket连接
    void postControlEvent(ControlEvent * controlEvent); //  触发事件函数

    void text(QRect rc);  // 测试点击

protected:
    virtual bool event(QEvent *event);
private:
    bool sendControl(const QByteArray& buffer);
private:
    deviceSocket* m_deviceSocket;
};

#endif // CONTROLLER_H
