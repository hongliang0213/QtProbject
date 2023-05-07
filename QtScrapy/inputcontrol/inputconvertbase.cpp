#include"inputconvertbase.h"



InputConvertBase::InputConvertBase()
{

}

InputConvertBase::~InputConvertBase()
{

}


//写入socket
void InputConvertBase::setDeviceSocket(deviceSocket *deviceSocket)
{
    m_controller.setDeviceSocket(deviceSocket);
}
// 发送事件
void InputConvertBase::sendControlEvent(ControlEvent *event)
{
    if(event){
        m_controller.postControlEvent(event);
    }
}
