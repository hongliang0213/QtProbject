#ifndef ADBPROCESS_H
#define ADBPROCESS_H
#include<QProcess>



//继承qprocess类
class AdbProcess : public QProcess
{
    Q_OBJECT    //使用信号槽时必须添加
public:
    //定义出现异常时的枚举列表
    enum ADB_EXEC_RESULT{
        AER_SUCCESS_START, //启动成功
        AER_ERROR_START,     //启动失败
        AER_SUCCESS_EXEX,    //执行成功
        AER_ERROR_EXEX,      //执行失败
        AER_ERROR_MISSING_BINARY,    //找不到文件
    };

    AdbProcess(QObject *parent = nullptr);

    static QString getAdbPath();

    //封装adb命令,参数一为adb指定序列号  ，参数二为具体命令
    void execute(const QString &serial, const QStringList &args);
    //封装常用的adb命令
    void push(const QString &serial, const QString& local,const QString& remote);
    void removeFile(const QString &serial, const QString& path);

    void reverse(const QString &serial, const QString& deviceSocketName,quint16 localPort);//建立反向代理
    void removeReverse(const QString &serial,const QString& deviceSocketName); //移除反向代理
    void getDeviceShell(const QString &serial);   //执行 adb devices 命令
    QStringList getDevicesSerialFromStdOut();   //解析android终端序列号
    QString getDeviceIpFromStdOut();   //解析 手机ip地址  ，先执行获取ip指令，才能解析
    void getIPShell(const QString &serial);   //执行获取ip命令，获取标准输出


    //获取标准输出函数
    QString getStdOut();
    QString getErrorOut();
signals:
    //自定义异常信号，能够在主函数中使用
    void adbProcessResult(ADB_EXEC_RESULT processResult);

private:
    //封装qprocess信号
    void initSignals();

    //动态设置adb路径
    static QString s_adbPath ;
    //标准输出,序列号和ip就在标准输出中，需要用正则表达式进行解析
    QString m_standardOutput="";
    QString m_errorOutPut="";

};

#endif // ADBPROCESS_H
