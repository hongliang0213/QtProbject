#include "adbprocess.h"
#include<QDebug>
#include<QFileInfo>
#include<QCoreApplication>
#include<QStringList>
#include<QString>
//动态adb路径
QString AdbProcess::s_adbPath="";

AdbProcess::AdbProcess(QObject *parent)
    :QProcess(parent)
{
    //\Thrid\adb\win
    //默认启动这些信号，输出错误或启动成功的信息
    initSignals();
    getAdbPath();
}
/**
 * @brief AdbProcess::getAdbPath
 * @return
 * 设置adb 启动路径，动态设置
 */
QString AdbProcess::getAdbPath()
{
    //使用qt中的环境变量来动态配置adbpath
    //先判断是否为空，
    if(s_adbPath.isEmpty()){
        s_adbPath = QString::fromLocal8Bit(qgetenv("QTSCRAPY_ADB_PATH"));
        QFileInfo fileInfo(s_adbPath);
        //如果通过环境变量指定的目录不存在，再去应用程序所在的目录下寻找adb路径
        if(s_adbPath.isEmpty() || !fileInfo.isFile()){
            //
            s_adbPath =QCoreApplication::applicationDirPath() +"adb";

        }
    }
    return s_adbPath;
}

/**
 * @brief AdbProcess::initSignals
 * 异常信息处理函数
 */
void AdbProcess::initSignals()
{

    connect(this,&QProcess::errorOccurred,this,[=](QProcess::ProcessError error){
        //ERROR 是信号触发后传递的参数

        if(error==ProcessError::FailedToStart){
            //emit 是触发信号的意思
            emit adbProcessResult(AER_ERROR_MISSING_BINARY);
        }else {
            emit adbProcessResult(AER_ERROR_START);
        }
        qDebug()<< error;

    });

    connect(this, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
          [=](int exitCode, QProcess::ExitStatus exitStatus){

        if(exitStatus==QProcess::NormalExit && exitCode ==0){
            emit adbProcessResult(AER_SUCCESS_EXEX);
        }else {
            emit adbProcessResult(AER_ERROR_EXEX);
        }
        qDebug()<<exitCode<<exitStatus;
    });

    connect(this,&QProcess::readyReadStandardError,this,[=](){
       m_errorOutPut=QString::fromLocal8Bit(readAllStandardError().trimmed());
       qDebug()<<m_errorOutPut;
    });

    connect(this,&QProcess::readyReadStandardOutput,this,[=](){
       m_standardOutput=QString::fromLocal8Bit(readAllStandardOutput().trimmed());
       qDebug()<<m_standardOutput;
    });

    connect(this,&QProcess::started,this,[=](){
       qDebug()<<"started success";
       emit adbProcessResult(AER_SUCCESS_START);
    });
}

/**
 * @brief AdbProcess::execute
 * @param serial
 * @param args
 * 执行adb 命令函数
 */
void AdbProcess::execute(const QString &serial, const QStringList &args)
{
    QStringList adbArgs;
    //判断serial是否为空，为空就是没有指定端口序列，不为空就是指定了端口，就代表连接了多台设备
    if(!serial.isEmpty()){
        adbArgs<<"-s"<<serial;
    }
    adbArgs<<args;
    //打印输出执行的命令
    qDebug()<<"adb "<<adbArgs.join("  ");

    //启动adb进程,将进程路径和 经过处理后的命令传入start
    start(getAdbPath(),adbArgs);
}
/**
  将文件push到手机设备中
 * @brief AdbProcess::push
 * @param serial   设备序列号
 * @param local   电脑设备文件路径
 * @param remote  手机文件路径
 */

void AdbProcess::push(const QString &serial, const QString &local, const QString &remote)
{
    QStringList adbArgs;
    adbArgs<<"push";
    adbArgs<<local;
    adbArgs<<remote;

    execute(serial,adbArgs);

}
/**
  将文件从移动设备中移除，使用 adb sheel命令
 * @brief AdbProcess::removeFile
 * @param serial
 * @param local   要移除文件的路径
 */
void AdbProcess::removeFile(const QString &serial, const QString &path)
{
    QStringList adbArgs;
    adbArgs<<"shell";
    adbArgs<<"rm";
    adbArgs<<path;

    execute(serial,adbArgs);

}
/**
   建立反向代理,域套接字
 * @brief AdbProcess::reverse
 * @param serial
 * @param deviceSocketName  代理名称
 * @param localPort  代理端口
 */
void AdbProcess::reverse(const QString &serial, const QString &deviceSocketName, quint16 localPort)
{
    QStringList adbArgs;
    adbArgs<<"reverse";
    adbArgs<<QString("localabstract:%1").arg(deviceSocketName);
    adbArgs<<QString("tcp:%1").arg(localPort);

    execute(serial,adbArgs);
}
/**
  移除代理
 * @brief AdbProcess::removeReverse
 * @param serial
 * @param deviceSocketName 要移除代理的名称
 */
void AdbProcess::removeReverse(const QString &serial, const QString &deviceSocketName)
{
    QStringList adbArgs;
    adbArgs<<"reverse";
    adbArgs<<"--remove";
    adbArgs<<QString("localabstract:%1").arg(deviceSocketName);

    execute(serial,adbArgs);
}
//执行adb  devices
void AdbProcess::getDeviceShell(const QString &serial)
{
    QStringList adbArgs;
    adbArgs<<"devices";
    execute(serial,adbArgs);

}
/**
  通过解析标准输出获取android设备序列号
 * @brief AdbProcess::getDevicesSerialFromStdOut
 * @return
 */
QStringList AdbProcess::getDevicesSerialFromStdOut()
{   //"List of devices attached\r\n15792232940086X\tdevice"
    QStringList serials;
    //先按/r/n 分割成几段
    QStringList devicesInfoList = m_standardOutput.split(QRegExp("\r\n|\n"),QString::SkipEmptyParts);
    foreach (QString deviceInfo, devicesInfoList) {
        //然后通过循环再把分割的段按\t分割
        QStringList deviceInfos=deviceInfo.split(QRegExp("\t"),QString::SkipEmptyParts);
        //通过判断分割的下标1的字段是否为devices 和总长度是否为2    来获取序列号
        if(deviceInfos.count() == 2 && 0==deviceInfos[1].compare("device")){
            serials<<deviceInfos[0];
        }
    }
    return serials;
}
/**
  获取设备ip地址
 * @brief AdbProcess::getDeviceIpFromStdOut
 * @return
 */
QString AdbProcess::getDeviceIpFromStdOut()
{   //   21: wlan0: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc mq state UP group default qlen 3000
    //   inet 192.168.0.101/24 brd 192.168.0.255 scope global wlan0
    //  valid_lft forever preferred_lft forever
    QString ip ="";
    QString strIpExp ="inet [\\d.]*";  //能匹配到inet 192.168.0.101这些
    QRegExp ipRegExp(strIpExp,Qt::CaseInsensitive);
    if(ipRegExp.indexIn(m_standardOutput) != -1){
        ip = ipRegExp.cap(0);
        ip = ip.right(ip.size() - 5);
    }


    return ip;
}
/**
  执行获取ip的指令
 * @brief AdbProcess::startGetIP
 * @param serial
 */
void AdbProcess::getIPShell(const QString &serial)
{
    QStringList adbArgs;
    adbArgs<<"shell";
    adbArgs<<"ip";
    adbArgs<<"-f";
    adbArgs<<"inet";
    adbArgs<<"addr";
    adbArgs<<"show";
    adbArgs<<"wlan0";
    execute(serial,adbArgs);
}

QString AdbProcess::getStdOut()
{
    return m_standardOutput;
}

QString AdbProcess::getErrorOut()
{
    return m_errorOutPut;
}
