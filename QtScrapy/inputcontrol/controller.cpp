#include "controller.h"
#include"devicesocket.h"
#include<qcoreapplication.h>
#include<controlevent.h>
Controller::Controller(QObject *parent)
    :QObject(parent)
{

}

void Controller::setDeviceSocket(deviceSocket *deviceSocket)
{
    m_deviceSocket= deviceSocket;
}

/**
 * @brief Controller::postControlEvent
 * @param controlEvent
 * 发送事件函数能触发整个发送事件
 */
void Controller::postControlEvent(ControlEvent *controlEvent)
{
    if(controlEvent){
        QCoreApplication::postEvent(this,controlEvent);
    }
}
/**
 * @brief Controller::text
 * @param rc
 *  测试函数
 */
void Controller::text(QRect rc)
{
    ControlEvent* mouseEvent= new ControlEvent(ControlEvent::CET_MOUSE); //写入鼠标事件
    //  分别写入鼠标按下的按键，动作，和位置
    mouseEvent->setMouseEventData(AMOTION_EVENT_ACTION_DOWN,AMOTION_EVENT_BUTTON_PRIMARY,rc);
    postControlEvent(mouseEvent);  //发送事件，触发整个函数运行，


//    QByteArray byteArray = mouseEvent.serializeData();  //调用接口返回拼装好的bytter数据
//    //发送指令
//    m_server.getDeviceSocket()->write(byteArray.data(),byteArray.length());
}

//触发事件之后会进行event事件操作
bool Controller::event(QEvent *event)
{
    //入过事件类型是controlEvent，则获取已经封装好的数据buffer
    if(event && event->type() == ControlEvent::Control){
        ControlEvent * controlEvent = dynamic_cast<ControlEvent *>(event);
        if(controlEvent){
            sendControl(controlEvent->serializeData());
        }
        return true;

    }
    return QObject::event(event);
}

//通过socket发送封装好的指令给安卓端
bool Controller::sendControl(const QByteArray &buffer)
{
    //如果数据不为空,则通过devicesocket发送封装好的buffer数据
    if (buffer.isEmpty()) {
        return false;
    }
    qint32 len = 0;
    if(m_deviceSocket){
        len = m_deviceSocket->write(buffer.data(), buffer.length());
    }
//    qDebug()<<"set true";

    return len == buffer.length() ? true : false;
}
