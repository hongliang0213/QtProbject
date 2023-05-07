#ifndef INPUTCONVERTBASE_H
#define INPUTCONVERTBASE_H
#include<QMouseEvent>
#include<QWheelEvent>
#include<QKeyEvent>
#include<devicesocket.h>
#include"controller.h"

//    控制指令的基类   基类，其他转换控制指令的类需要继承这个基类
class InputConvertBase
{
public:
    InputConvertBase();
    virtual ~InputConvertBase();

    //鼠标事件
    virtual void mouseEvent(const QMouseEvent * from,const QSize& frameSize,const QSize& showSize) = 0;

    //滚轮事件
    virtual void wheelEvent(const QWheelEvent * from,const QSize& frameSize,const QSize& showSize) = 0;

    //键盘事件
    virtual void keyCodeEvent(const QKeyEvent * from,const QSize& frameSize,const QSize& showSize) = 0;

    void setDeviceSocket(deviceSocket * deviceSocket);
private:
    Controller m_controller;
protected:
    void sendControlEvent(ControlEvent* event);

};
#endif // INPUTCONVERTBASE_H
