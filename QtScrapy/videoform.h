#ifndef VIDEOFORM_H
#define VIDEOFORM_H

#include <QWidget>
#include <controller.h>
#include <decoder.h>
#include <server.h>
#include<frames.h>
#include"inputconvertnormal.h"
#include<QMainWindow>
namespace Ui {
class VideoForm;
}

class VideoForm : public QWidget
{
    Q_OBJECT

public:
    explicit VideoForm(const QString& serial,QWidget *parent = nullptr);
    ~VideoForm();

protected:
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);    // 鼠标事件

    virtual void wheelEvent(QWheelEvent *event); //滚轮事件

    virtual void keyPressEvent(QKeyEvent *event);
    virtual void keyReleaseEvent(QKeyEvent *event);  //key code 事件
private:
    void updateShowSzie(const QSize & newSize);
    void createQTollBar();   //创建磁吸工具条
private:
    Ui::VideoForm *ui;
    QString m_serial = "";
    server m_server;   //server 服务查看连接情况

    Decoder m_decoder ;   //decoder解码器对象
    Frames m_frames;     // 视屏帧对象

//    Controller m_controller;  //发送鼠标事件类，封装好的
    InputConvertNormal m_inputConvert;

    QSize m_frameSize;

};

#endif // VIDEOFORM_H
