#ifndef MYCODEEDIT_H
#define MYCODEEDIT_H

#include <QWidget>
#include<QPlainTextEdit>
#include<QTextBrowser>
class LineNumberWidgets;  //绘制行号的类

class MyCodeEdit : public QPlainTextEdit
{
    Q_OBJECT
public:
    explicit MyCodeEdit(QWidget *parent = nullptr);
    void lineNumberWidgetPaintEvent(QPaintEvent * event);   //绘制行号提供的函数
signals:


protected:
    void resizeEvent(QResizeEvent *event);
private:
    //控件
    LineNumberWidget * lineNumberWidget;

    QTextEdit *textEdit ;
    QTextBrowser *textBrowser;
    QScrollBar *scrollBar ;
    QFont mFont;


    void updateLineNumberWidgetWidth();  //  自适应边距
    void initWidget();
    void initFont();  //初始化字体
    void initHighlighterLine();   //行高亮
    void initHighlighter();   //高亮
    void initUpdate();  //更新行数的信号，用来实现显示行号
    int getLineNumberWidgetWidth();  //获取合适宽度
};



class LineNumberWidgets :public QWidget
{
public:
    //   构造初始化, 传入一个codeedit进行绘制
    LineNumberWidgets(MyCodeEdit * editor=nullptr):QWidget(editor){
        codeEditor = editor;
    }

protected:
    //绘制
    void paintEvent(QPaintEvent *event){
        codeEditor->lineNumberWidgetPaintEvent(event);
    }

private:
    MyCodeEdit * codeEditor;
};
#endif // MYCODEEDIT_H
