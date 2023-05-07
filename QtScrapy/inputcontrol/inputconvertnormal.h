#ifndef INPUTCONVERTNORMAL_H
#define INPUTCONVERTNORMAL_H
#include"inputconvertbase.h"
#include"controlevent.h"

//    通用控制转换类
class InputConvertNormal : public InputConvertBase
{
public:
    InputConvertNormal();
    ~InputConvertNormal();

    //鼠标事件     重新处理三个虚函数， 进行控制指令的转换
    virtual void mouseEvent(const QMouseEvent * from,const QSize& frameSize,const QSize& showSize);
    //滚轮事件
    virtual void wheelEvent(const QWheelEvent * from,const QSize& frameSize,const QSize& showSize);
    //键盘事件
    virtual void keyCodeEvent(const QKeyEvent * from,const QSize& frameSize,const QSize& showSize);

private:
    // 代表控制指令中鼠标按下的哪个按键 ,将qt按下的鼠标按键，转化为控制指令的鼠标事件
    AndroidMotioneventButtons covertMouseButtons(Qt::MouseButtons buttonStatus);
    //  key code 文本事件
    AndroidKeycode convertKeyCode(int key, Qt::KeyboardModifiers modifiers);
    //辅助按键
    AndroidMetastate convertMetastate(Qt::KeyboardModifiers modifiers);
};

#endif // INPUTCONVERTNORMAL_H
