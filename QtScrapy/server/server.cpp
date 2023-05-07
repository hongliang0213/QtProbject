#include "server.h"
#include<QFileInfo>
#include<QCoreApplication>

//将jar文件存放在android设备上的路径
#define DEVICE_SERVER_PATH "/data/local/tmp/scrcpy-server.jar"
#define SOCKET_NAME "scrcpy"
#define DEVICE_NAME_FIELD_LENGTH 64   //传入的前64个字节数据就是deviceName
server::server(QObject *parent)
    : QObject(parent)
{
    connect(&m_workProcess,&AdbProcess::adbProcessResult,this,&server::onWorkProcessResult);

    // 执行android jar 文件时候信号要另外处理
    connect(&m_ServerProcess,&AdbProcess::adbProcessResult,this,&server::onWorkProcessResult);

    // 监听pc端的socket 连接，有 socket连接时执行槽函数
    connect(&m_serverSocket, &QTcpServer::newConnection, this, [this](){
        // 获取这个socket连接 ,需要进行转化
        //   将qtcpsocket对象转化为tcpsocket
        deviceSocket *socket =dynamic_cast<deviceSocket*>(m_serverSocket.nextPendingConnection());
        if (socket) {
            m_deviceSocket = socket;
//            qDebug() << "Connection established: " << m_deviceSocket->peerAddress().toString();
        } else {
            qWarning() << "Failed to establish connection";
        }

//        qDebug()<<m_serverSocket.nextPendingConnection();
//        m_deviceSocket = static_cast<QTcpSocket *>(m_serverSocket.nextPendingConnection());
//        qDebug()<<m_deviceSocket;

         QString deviceName;
         QSize size;
         if (m_deviceSocket && m_deviceSocket->isValid() && readInfo(deviceName, size)) {
             disableTunneReverse();    //连接成功后可以关闭反向代理和移除android端的文件， 因为已经建立了域套接进程
             removeServer();   //移除jar文件时因为jar文件正在使用，所以不会立即移除，会在连接断开后移除
             emit connectToResult(true, deviceName, size);  // 触发信号
         } else {
             stop();
             emit connectToResult(false, deviceName, size);
         }
     });
}


/**
 * @brief server::start
 * @param serial  指定android序列
 * @param localPort 反向代理端口
 * @param maxSize  最大尺寸
 * @param bitRate  波特率
 * @return
 */
bool server::start(const QString &serial, quint16 localPort, quint16 maxSize, quint16 bitRate)
{
    m_serial=serial;
    m_localPort=localPort;
    m_maxSize=maxSize;
    m_bitRate=bitRate;

    // 设置服务运行状态
    m_serverStartStep=SSS_PUSH;

    return StartServerByStep();
}

//   获取devicesocket对象
deviceSocket *server::getDeviceSocket()
{
    return m_deviceSocket;

}


/**
 * @brief server::stop
 * 关闭整个server服务
 */
void server::stop()
{
    if (m_deviceSocket) {
           m_deviceSocket->close();
           //m_deviceSocket->deleteLater();
       }

       m_ServerProcess.kill();  //杀死第三步所打开的进程
       disableTunneReverse(); //   // 关闭反向代理
       removeServer();  //移除 jar文件
       m_serverSocket.close();    //关闭m_serverSocket套接字
//       qDebug()<<" remove all server";
}

/**
 * @brief server::StartServerByStep
 * @return
 * 具体执行函数,每部分执行都会返回执行结果，如果有一步没执行成功，就为false，
 * 当每步执行成功后就将状态置为true
 *  状态机
 */
bool server::StartServerByStep()
{
    bool stepSucess=false;
    if(m_serverStartStep != SSS_NULL){
        switch (m_serverStartStep) {
        case SSS_PUSH:
            stepSucess = pushServer();
            break;
        case SSS_ENABLE_REVERSE:
            stepSucess = enableTunneReverse();
            break;
        case SSS_EXECUTE_SERVER:

            m_serverSocket.setMaxPendingConnections(1);   //设置pc端最大连接数，
            //监听端口
            if (!m_serverSocket.listen(QHostAddress::LocalHost, m_localPort)) {
                //如果没有监听成功就进行一些处理
                qCritical(QString("Could not listen on port %1").arg(m_localPort).toStdString().c_str());//打印日志
                m_serverStartStep = SSS_NULL; //清空状态机
                disableTunneReverse();  // 关闭反向代理
                removeServer();   // 清除android  jar文件
                emit serverSatrtResult(false);  //返回错误信号

                return false;
            }



            stepSucess = execute();
            break;
        default:
             break;
        }

    }
    if (!stepSucess) {
        emit serverSatrtResult(false);
    }

    return stepSucess;
}
/**
  移除android端的文件
 * @brief server::removeServer
 * @return
 */
bool server::removeServer()
{
    //判断是否将jar包写入android设备中
    if(!m_serverCopiedToDevice){
        return true;
    }
    m_serverCopiedToDevice = false; //判断成功后再置为false

    // new 一个adbprocess ，当执行移除完成后，无论成功与否，都进行移除
    AdbProcess * adb =new AdbProcess();
    if(!adb){
        return false;
    }
    //当adb对象执行删除操作后，无论成功与否清理adb对象内存
    connect(adb,&AdbProcess::adbProcessResult,this,[=](AdbProcess::ADB_EXEC_RESULT processResult){
        //只要执行状态不是初始化启动成功，那么就直接清空内存
        if(processResult!=AdbProcess::AER_SUCCESS_START){
            sender()->deleteLater(); // 获取发送信号的对象并清除
        }
    });
    //执行清除操作
    adb->removeFile(m_serial,DEVICE_SERVER_PATH);
//    qDebug()<<"移除android jar 文件成功";
    return true;
}
/**
  执行 push server
 * @brief server::pushServer
 * @return
 */
bool server::pushServer()
{
    m_workProcess.push(m_serial,getServerPath(),DEVICE_SERVER_PATH);

    return true;
}
/**
  执行打开反向代理
 * @brief server::enableTunneReverse
 * @return
 */
bool server::enableTunneReverse()
{
    //执行打开反向代理 ，执行成功返回true
    m_workProcess.reverse(m_serial,SOCKET_NAME,m_localPort);
//    qDebug()<<"打开反向代理";
    return true;
}
/**
  关闭反向代理
 * @brief server::disableTunneReverse
 * @return
 */
bool server::disableTunneReverse()
{
    //判断是否开启反向代理
    if(!m_enableReverseFlag){
        return true;
    }
    m_enableReverseFlag = false; //判断成功后再置为false

    // new 一个adbprocess ，当执行移除完成后，无论成功与否，都进行移除
    AdbProcess * adb =new AdbProcess();
    if(!adb){
        return false;
    }
    //当adb对象执行删除操作后，无论成功与否清理adb对象内存
    connect(adb,&AdbProcess::adbProcessResult,this,[=](AdbProcess::ADB_EXEC_RESULT processResult){
        //只要执行状态不是初始化启动成功，那么就直接清空内存
        if(processResult!=AdbProcess::AER_SUCCESS_START){
            sender()->deleteLater(); // 获取发送信号的对象并清除
        }
    });
    //   remove server
    adb->removeReverse(m_serial,SOCKET_NAME);
//    qDebug()<<" 移除反向代理成功";
    return true;
}
/**
  执行写入android设备中的jar .apk包
 * @brief server::excute
 * @return
 */
bool server::execute()
{   //执行shell 命令线程会阻塞
    QStringList args;
    args<<"shell";
    args<<QString("CLASSPATH=%1").arg(DEVICE_SERVER_PATH); //通过shell命令指定执行的文件路径
    args<<"app_process";
    args<<"/";
    args<<"com.genymobile.scrcpy.Server";  //指定执行的jar包的类名
    args<<QString::number(m_maxSize);   //指定最大尺寸
    args<<QString::number(m_bitRate);  //   比特率
    args<<false;
    args<<"";
    // 执行shell启动文件时，不会有返回结果，所以需要另外处理
    m_ServerProcess.execute(m_serial,args);

    return true;
}


/**
  每个步骤执行后触发信号，执行这个槽函数,执行失败就跳外部，执行成功就返回到StartServerByStep函数中，并将状态置为下一步
 * @brief server::onWorkProcessResult
 * @param processResult
 */
void server::onWorkProcessResult(AdbProcess::ADB_EXEC_RESULT processResult)
{
    if(sender() == &m_workProcess){
        if(m_serverStartStep != SSS_NULL){
            switch (m_serverStartStep) {
            case SSS_PUSH:
                //判断push是否执行成功，成功的话把m_serverStartStep改变状态步骤
                if(AdbProcess::AER_SUCCESS_EXEX == processResult){
                    m_serverStartStep = SSS_ENABLE_REVERSE;
                    m_serverCopiedToDevice = true  ; // 标志jar包已经成功push到android设备中
                    StartServerByStep();  //执行成功后跳转到StartServerByStep，因为执行状态已经转到下一步，所以会自己执行下一步
                }else if(processResult != AdbProcess::AER_SUCCESS_START){
                    qCritical("adb push failed");
                    m_serverStartStep = SSS_NULL;
                    //执行失败触发信号
                    emit serverSatrtResult(false);
                }
                break;
            case SSS_ENABLE_REVERSE:
                //判断是否执行成功，成功的话把m_serverStartStep改变状态步骤,并转到执行步骤函数执行下一步
                if(AdbProcess::AER_SUCCESS_EXEX == processResult){
                    m_enableReverseFlag =true;    //反向代理开启标志flag
                    m_serverStartStep = SSS_EXECUTE_SERVER;   //修改执行状态
                    StartServerByStep();  //跳转到执行下一步函数
                }else if(processResult != AdbProcess::AER_SUCCESS_START){
                    qCritical("adb reverse failed");
                    m_serverStartStep = SSS_NULL;  //执行失败重置步骤为null
                    removeServer();               //因为到第二步，失败了就把第一步的jar包删除掉
                    emit serverSatrtResult(false);//执行失败触发信号
                }
                break;

            default:
                break;
            }
        }

    }
    if(sender() == &m_ServerProcess){
        if(m_serverStartStep == SSS_EXECUTE_SERVER){
            if(AdbProcess::AER_SUCCESS_START == processResult){
                //服务启动成功只会抛出一个 AER_SUCCESS_START
                m_serverStartStep = SSS_RUN;  //启动成功后将服务置为正在运行
                emit serverSatrtResult(true);      // 向外部发送启动成功的信号
            }else if(AdbProcess::AER_ERROR_START == processResult){
                qCritical("adb shell start server failed");
                 m_serverStartStep = SSS_NULL;
                disableTunneReverse();  // 移除反向代理
                removeServer();   //移除server 即jar包
                emit serverSatrtResult(false);
            }
        }else if(m_serverStartStep == SSS_RUN){
            m_serverStartStep = SSS_NULL;
            emit onServerStop();
        }

    }


}

/**
  获取scrcpy-server-jar文件路径
 * @brief server::getServerPath
 * @return
 */
QString server::getServerPath()
{
    //使用qt中的环境变量来动态配置adbpath
    //先判断是否为空，
    if(m_serverPath.isEmpty()){
        m_serverPath = QString::fromLocal8Bit(qgetenv("QTSCRAPY_SERVER_PATH"));
        QFileInfo fileInfo(m_serverPath);
        //如果通过环境变量指定的目录不存在，再去应用程序所在的目录下寻找adb路径
        if(m_serverPath.isEmpty() || !fileInfo.isFile()){
            //
            m_serverPath =QCoreApplication::applicationDirPath() +"/scrcpy-server.jar";
        }
    }
    return m_serverPath;
}
/**
   获取android 设备相应的设备信息
 * @brief server::readInfo
 * @param deviceName
 * @param size
 * @return
 */
bool server::readInfo(QString &deviceName, QSize &size)
{
    //前64个字节为设备名称，  后面的4个字节分别是设备宽和高
    // abk001-----------------------0x0438 0x02d0
       //               64b            2b w   2b h
       unsigned char buf[DEVICE_NAME_FIELD_LENGTH + 4];
       // 判断devicesocket 接收了多少个字节数据，如果小于68个字节，就让他等待3s
       if (m_deviceSocket->bytesAvailable() <= (DEVICE_NAME_FIELD_LENGTH + 4)) {
           m_deviceSocket->waitForReadyRead(300);
       }

       qint64 len = m_deviceSocket->read((char*)buf, sizeof(buf));  //判断接收的数据长度，长度不够就是数据缺失，返回false
       if (len < DEVICE_NAME_FIELD_LENGTH + 4) {
           qInfo("Could not retrieve device information");
           return false;
       }

       // 这是一个C或C++语言中的代码片段，它的意思是将一个名为"buf"的字符数组的第DEVICE_NAME_FIELD_LENGTH - 1个元素赋值为'\0'，
       //这段代码的目的可能是要确保"buf"数组中的字符串不超过预设的长度，以防止缓冲区溢出。在C/C++中，
       //字符串是以null终止符'\0'来标记字符串的结尾的，因此将最后一个字符设为'\0'可以确保字符串以正确的方式终止。
       //也就是将其设为字符串的结尾符号。
       buf[DEVICE_NAME_FIELD_LENGTH - 1] = '\0';
       deviceName = (char*)buf;  //转为(char*)
       size.setWidth((buf[DEVICE_NAME_FIELD_LENGTH] << 8) | buf[DEVICE_NAME_FIELD_LENGTH + 1]);
       size.setHeight((buf[DEVICE_NAME_FIELD_LENGTH + 2] << 8) | buf[DEVICE_NAME_FIELD_LENGTH + 3]);
       return true;
}
