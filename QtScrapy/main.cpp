#include <QApplication>
#include <QDir>
#include <QDebug>
#include "dialog.h"
#include"decoder.h"   //decoder-ffmepg模块
Dialog* g_mainDlg = Q_NULLPTR;
QtMessageHandler g_oldMessageHandler = Q_NULLPTR;
void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg); //截取打印debug函数

int main(int argc, char *argv[])
{
    g_oldMessageHandler = qInstallMessageHandler(myMessageOutput);
    //设置环境变量
    qputenv("QTSCRAPY_ADB_PATH","..\\QtScrapy\\Thrid\\adb\\win\\adb.exe");
    qputenv("QTSCRAPY_SERVER_PATH","..\\QtScrapy\\Thrid\\scrcpy-server.jar");

    Decoder::init();   //decode初始化
    QApplication a(argc, argv);

    // 加载样式表
    QFile file(":/res/flatwhite.css");
    if (file.open(QFile::ReadOnly)) {
        QString qss = QLatin1String(file.readAll());
        QString paletterColor = qss.mid(20, 7);  //设置中性颜色，即样式表的常规颜色
        // 设置颜色
        qApp->setPalette(QPalette(QColor(paletterColor)));
        // 应用样式表
        qApp->setStyleSheet(qss);
        file.close();
    }

    Dialog w;
    g_mainDlg = &w; //将dialog地址给g_mainDlg
    w.show();

    int ret = a.exec();
    Decoder::deInit();   //decode 清除

    return ret;
}

// 重写qdebug，截取qdebug打印的数据 输出
void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    if (g_oldMessageHandler) {
        g_oldMessageHandler(type, context, msg);
    }
    if (g_mainDlg) {
        g_mainDlg->outLog(msg,false);
    }
}
