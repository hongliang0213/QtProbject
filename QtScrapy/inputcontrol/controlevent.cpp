#include "controlevent.h"
#include<QBuffer>
#include<qdebug.h>
#define TEXT_MAX_CHARACTER_LENGTH 300

ControlEvent::ControlEvent(ControlEvent::ControlEventType type)
    :QScrcpyEvent(Control)
{
    m_data.type = type;
}

// 鼠标相关事件
void ControlEvent::setMouseEventData(AndroidMotioneventAction action, AndroidMotioneventButtons buttons, QRect position)
{
    m_data.mouseEvent.action = action;
    m_data.mouseEvent.buttons = buttons;
    m_data.mouseEvent.position = position;
}
//    键盘按下事件
void ControlEvent::setKeycodeEventData(AndroidKeyeventAction action, AndroidKeycode keycode, AndroidMetastate metastate)
{
    m_data.keycodeEvent.action = action;
    m_data.keycodeEvent.keycode = keycode;
    m_data.keycodeEvent.metastate = metastate;
}
 // 写入文本事件
void ControlEvent::setTextEventData(QString text)
{
    m_data.textEvent.text = text;
}
//多点触控事件
void ControlEvent::setTouchEventData(quint32 id, AndroidMotioneventAction action, QRect position)
{
    m_data.touchEvent.action = action;
    m_data.touchEvent.id = id;
    m_data.touchEvent.position = position;
}
// 鼠标滑动事件
void ControlEvent::setScrollEventData(QRect position, qint32 hScroll, qint32 vScroll)
{
    m_data.scrollEvent.position = position;
    m_data.scrollEvent.hScroll = hScroll;
    m_data.scrollEvent.vScroll = vScroll;
}
//自定义事件
void ControlEvent::setCommandEventData(qint32 action)
{
    m_data.commandEvent.action = action;
}

 // 构造控制指令
QByteArray ControlEvent::serializeData()
{
    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    buffer.open(QBuffer::WriteOnly);  //打开写入模式才能写入数据
    //构造控制指令
    //type  mouse 控制指令
    buffer.putChar(m_data.type);
    switch (m_data.type) {
        case CET_KEYCODE:
            buffer.putChar(m_data.keycodeEvent.action);
            write32(buffer, m_data.keycodeEvent.keycode);
            write32(buffer, m_data.keycodeEvent.metastate);
            break;
        case CET_TEXT:
        {
            // write length (2 byte) + date (non nul-terminated) TEXT_MAX_CHARACTER_LENGTH，允许发送的最大文本长度
            if (TEXT_MAX_CHARACTER_LENGTH < m_data.textEvent.text.length()) {
                // injecting a text takes time, so limit the text length
                m_data.textEvent.text = m_data.textEvent.text.left(TEXT_MAX_CHARACTER_LENGTH);
            }
            QByteArray tmp = m_data.textEvent.text.toUtf8();
            write16(buffer, tmp.length());
            buffer.write(tmp.data(), tmp.length());
        }
            break;
        case CET_MOUSE:
            buffer.putChar(m_data.mouseEvent.action);
            write32(buffer, m_data.mouseEvent.buttons);
            writePosition(buffer, m_data.mouseEvent.position);
            break;
        case CET_TOUCH:
            buffer.putChar(m_data.touchEvent.id);
            buffer.putChar(m_data.touchEvent.action);
            writePosition(buffer, m_data.touchEvent.position);
            break;
        case CET_SCROLL:
            writePosition(buffer, m_data.scrollEvent.position);
            write32(buffer, m_data.scrollEvent.hScroll);
            write32(buffer, m_data.scrollEvent.vScroll);
            break;
        case CET_COMMAND:
            buffer.putChar(m_data.commandEvent.action);
            break;
        default:
            qDebug() << "Unknown event type:" << m_data.type;
            break;
        }
    buffer.close();
    return byteArray;

}


// 写入四个字节数据

void ControlEvent::write32(QBuffer &buffer, quint32 value)
{
    // putchar 会只取一个字节数据，即只取最后一个字节数据
    buffer.putChar(value>>24); //把00 移动到 末尾，取到00
    buffer.putChar(value>>16); //把11移动到末尾  取 11
    buffer.putChar(value>>8); //把22 移动到末尾取  22
    buffer.putChar(value);
}


//写入两个字节的位置

void ControlEvent::write16(QBuffer &buffer, quint32 value)
{
    buffer.putChar(value>>8);
    buffer.putChar(value);
}
//写入具体按下坐标
void ControlEvent::writePosition(QBuffer &buffer, QRect value)
{
    write16(buffer,value.left());
    write16(buffer,value.top());
    write16(buffer,value.width());
    write16(buffer,value.height());
}
