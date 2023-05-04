#ifndef MYCODEEDITOR_H
#define MYCODEEDITOR_H
#include<QStatusBar>
#include <QPlainTextEdit>
#include<MyHighLighter/myhighlighter.h>
class LineNumberWidget;

class MyCodeEditor : public QPlainTextEdit
{
    Q_OBJECT
public:
    explicit MyCodeEditor(QWidget *parent = nullptr);
    ~MyCodeEditor();  //析构函数
    void lineNumberWidgetPaintEvent(QPaintEvent * event);

    void lineNumberWidgetMousePressEvent(QMouseEvent *event);

    void lineNumberWidgetWheelEvent(QWheelEvent *event);

    bool saveFile();  //保存文件 模块
    void setFileName(QString fileName);
    QString getFileName();

    //另存
    bool saveAsFile();

    //mainwindows设置字体
    void setAllFont(QFont font);

    void mMyHighLighterinitAll(); //更新字体状态

    QString fileCurrectName;   //接收标签中的filepath
    void setfileCurrectName(QString path);  //settab标签中的path

    //获取字体总数
    int countAllChar;  //字数总数
    void getAllCharCount();

private slots:
    void highlightCurrentLine();
    void updateLineNumberWidget(QRect rect, int dy);
    void updateLineNumberWidgetWidth();


protected:
    void resizeEvent(QResizeEvent *event);

private:
    void initConnection();
    void initHighlighter();
    int getLineNumberWidgetWidth();


    LineNumberWidget * lineNumberWidget;
    QString mFileName;  //保存文件名

    MyHighLighter * mMyHighLighter; //高亮对象


signals:

};

class LineNumberWidget :public QWidget{
public:
    explicit LineNumberWidget(MyCodeEditor *editor=nullptr):QWidget(editor){
        codeEditor=editor;
    }

protected:
    void  paintEvent(QPaintEvent *event) override{
        //把绘制任务提交给MyCodeEditor
        codeEditor->lineNumberWidgetPaintEvent(event);
    }
    void mousePressEvent(QMouseEvent *event) override{
        //把鼠标点击任务提交给MyCodeEditor
        codeEditor->lineNumberWidgetMousePressEvent(event);
    }

    void wheelEvent(QWheelEvent *event) override{
        //把滚轮任务提交给MyCodeEditor
        codeEditor->lineNumberWidgetWheelEvent(event);
    }

private:
    MyCodeEditor *codeEditor;
};

#endif // MYCODEEDITOR_H
