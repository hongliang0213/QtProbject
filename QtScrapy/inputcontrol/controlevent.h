#ifndef CONTROLEVENT_H
#define CONTROLEVENT_H
#include"input.h"
#include"keycodes.h"

#include<qscrcpyevent.h>
#include <QRect>
#include<QBuffer>
class ControlEvent:public QScrcpyEvent
{
public:
    enum ControlEventType{
        CET_KEYCODE,  //键盘按下
        CET_TEXT ,  //写入文本
        CET_MOUSE, //鼠标事件
        CET_SCROLL, //鼠标滑动时间
        CET_COMMAND,//自定义事件
        CET_TOUCH,  // 多点触摸事件
    };
    ControlEvent(ControlEventType type);

    //鼠标相关事件
    void setMouseEventData(AndroidMotioneventAction action,AndroidMotioneventButtons buttons,QRect positon);
    // id 代表一个触摸点，最多支持10个触摸点[0,9]
    // action 只能是AMOTION_EVENT_ACTION_DOWN，AMOTION_EVENT_ACTION_UP，AMOTION_EVENT_ACTION_MOVE
    // position action动作对应的位置
    void setTouchEventData(quint32 id, AndroidMotioneventAction action, QRect position); // 多点触摸事件
    //鼠标滑动事件
    void setScrollEventData(QRect position, qint32 hScroll, qint32 vScroll);
    //自定义事件
    void setCommandEventData(qint32 action);
    //键盘按下事件
    void setKeycodeEventData(AndroidKeyeventAction action, AndroidKeycode keycode, AndroidMetastate metastate);
    //输入文本事件
    void setTextEventData(QString text);


    QByteArray serializeData();  //构造控制指令，switch进行选择

protected:
    void write32(QBuffer &buffer,quint32 value);  //  写入四个字节数据
    void write16(QBuffer &buffer,quint32 value);   //存放两个字节数据
    void writePosition(QBuffer &buffer,QRect value);   // 写入位置writePosition
private:
    struct ControlEventDate{
        ControlEventType type;
        //union  共用体，在共用体中可以定义多种不同的数据类型
        union {
            struct {
                AndroidMotioneventAction action; //鼠标按下按键
                AndroidMotioneventButtons buttons;  //鼠标按下动作
                QRect position;
            } mouseEvent;  //鼠标相关事件
            struct {
                AndroidKeyeventAction action;
                AndroidKeycode keycode;
                AndroidMetastate metastate;
            } keycodeEvent;    //键盘按下事件
            struct {
                QString text;
            } textEvent;   // f发送文本事件
            struct {
                quint32 id;
                AndroidMotioneventAction action;
                QRect position;
            } touchEvent;  //多点触摸事件
            struct {
                QRect position;
                qint32 hScroll;
                qint32 vScroll;
            } scrollEvent;   //鼠标滑动事件
            struct {
                qint32 action;
            } commandEvent;      //自定义事件
        };
        ControlEventDate(){}
        ~ControlEventDate(){}

    };
    ControlEventDate m_data;
};

#endif // CONTROLEVENT_H
