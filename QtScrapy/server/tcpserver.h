#ifndef TCPSERVER_H
#define TCPSERVER_H
#include<QTcpServer>


//改功能是需要重新实现qtcpserver中的一个虚函数，让他返回自己定义的devicesocket类
class TcpServer:public QTcpServer
{
    Q_OBJECT
public:
    TcpServer(QObject *parent = nullptr);

protected:
    virtual void incomingConnection(qintptr handle);
};

#endif // TCPSERVER_H
