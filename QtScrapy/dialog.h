#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include"server/server.h"
#include"decoder.h"
#include"frames.h"
#include<qyuvopenglwidget.h>
#include"controller.h"
#include<QPointer>
#include"adbprocess.h"

class VideoForm;  // 前置声明，才能使用这个类
namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = nullptr);
    ~Dialog();

    void outLog(const QString &log,bool newLine);  //参数一，需要输出的日志，参数二是否自动换行
private slots:
//    void on_TextButton_clicked();

    void on_StartServerButton_clicked();

    void on_StopServerButton_clicked();

    void on_pushButton_clicked();

    void on_updateDeviceButton_clicked();

    void on_startAdbButton_clicked();

    void on_getIpButton_clicked();

    void on_wirelessButton_clicked();

    void on_inforButton_clicked();

private:
    Ui::Dialog *ui;
    QPointer<VideoForm> m_VideoForm;
    AdbProcess m_adb;

    bool m_isPressed;   //窗口移动
    QPoint m_dragPosition;
protected:
    void mousePressEvent(QMouseEvent *event);   //窗口移动事件
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);



//       QYUVOpenGLWidget * m_videoWidget;  //  opengl渲染器对象，用来显示视屏

};

#endif // DIALOG_H
