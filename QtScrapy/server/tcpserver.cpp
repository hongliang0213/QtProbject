#include "tcpserver.h"
#include "devicesocket.h"
TcpServer::TcpServer(QObject *parent)
    :QTcpServer(parent)
{

}

void TcpServer::incomingConnection(qintptr handle)
{
    deviceSocket* socket = new deviceSocket();
    socket->setSocketDescriptor(handle);
    addPendingConnection(socket);

}
