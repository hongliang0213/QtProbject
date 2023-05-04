#ifndef MYTEXTEDITBYCODE_H
#define MYTEXTEDITBYCODE_H

#include <QWidget>
#include <QTextEdit>
#include <QTextBrowser>
#include <QScrollBar>

class MyTextEditByCode : public QWidget
{
    Q_OBJECT
public:
    explicit MyTextEditByCode(QWidget *parent = nullptr);
private slots:

private:
    QFont  mFont;
    QTextEdit *textEdit ;
    QTextBrowser *textBrowser;
    QScrollBar *scrollBar ;

    void initWidget();  //初始化布局
    void initFont();  //初始化字体
    void initConnection(); //绑定滚动条
    void onTextChanged();  // 行号显示
    void initHighlighter();   //高亮显示
    void highlightCurrentLine();  //射置行高亮
signals:
};

#endif // MYTEXTEDITBYCODE_H
