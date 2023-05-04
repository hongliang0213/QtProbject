#ifndef MYTEXTEDIT_H
#define MYTEXTEDIT_H

#include <QWidget>

namespace Ui {
class MyTextEdit;
}

class MyTextEdit : public QWidget
{
    Q_OBJECT
protected:
    void keyPressEvent(QKeyEvent *event);  //重写keyPressEvent 捕捉tab键，
public:
    explicit MyTextEdit(QWidget *parent = nullptr);
    ~MyTextEdit();

    void BindingHorizontalScrollBar(); //绑定滚动条，横条和竖条
    void initFont();   //设置字体
    void TextLine();   //设置行号
    void initHighLighter(); //设置文本高亮
    void setCurrentLine();  //设置行高亮信号


private:
    Ui::MyTextEdit *ui;
};

#endif // MYTEXTEDIT_H
