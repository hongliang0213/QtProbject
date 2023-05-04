#include "myhighlighter.h"
#include "mytexteditbycode.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
# include<MyHighLighter/myhighlighter.h>
#include<QFont>
#include<QTextCursor>
MyTextEditByCode::MyTextEditByCode(QWidget *parent)
    : QWidget{parent}
{

    initWidget();    //UI组件

    initFont(); //字体

    initConnection();    //滚动条绑定

    onTextChanged();    //显示行号

    initHighlighter();    //高亮

    highlightCurrentLine();   //行高亮
}
/**
 * @brief MyTextEditByCode::initWidget
 * 初始化主布局
 */
void MyTextEditByCode::initWidget()
{
    //  widget主窗口
    QWidget * horizontalLayoutWidget = new QWidget();
    //竖向布局
    QVBoxLayout *verticalLayout = new QVBoxLayout(this);

    verticalLayout->setSpacing(0);
    verticalLayout->setContentsMargins(0,0,0,0);

    //横向布局
    QHBoxLayout *horizontalLayout = new QHBoxLayout(horizontalLayoutWidget);
    horizontalLayout->setSpacing(0);
    horizontalLayout->setContentsMargins(0,0,0,0); //设置上下左右的边距

    // 主文本框
    textEdit = new QTextEdit();
    textEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    textEdit->setLineWrapMode(QTextEdit::NoWrap);

    //左侧显示行数框
    textBrowser = new QTextBrowser();
    textBrowser->setMaximumWidth(21);
    textBrowser->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    textBrowser->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // 底部滚动条
    scrollBar = new QScrollBar();
    scrollBar->setOrientation(Qt::Horizontal);

    horizontalLayout->addWidget(textBrowser);
    horizontalLayout->addWidget(textEdit);

    verticalLayout->addWidget(horizontalLayoutWidget);
    verticalLayout->addWidget(scrollBar);
}
/**
 * @brief MyTextEditByCode::initFont
 * 初始化字体
 */
void MyTextEditByCode::initFont()
{
    textEdit->setFont(QFont("Consolas",11));
    textBrowser->setFont(QFont("Consolas",11));
    textBrowser->setAlignment(Qt::AlignRight);  // 设置行号右对齐
}

/**
 * @brief MyTextEditByCode::initConnection
 * 绑定滚动条
 */
void MyTextEditByCode::initConnection()
{
    // 将文本框自己的滚动条绑定到 底部新建的滚动条上
    connect(textEdit->horizontalScrollBar(),&QScrollBar::valueChanged,this,[=](){
        scrollBar->setMinimum(textEdit->horizontalScrollBar()->minimum());
        scrollBar->setMaximum(textEdit->horizontalScrollBar()->maximum());
        scrollBar->setPageStep(textEdit->horizontalScrollBar()->pageStep());
        scrollBar->setValue(textEdit->horizontalScrollBar()->value());

    });
    //将最底部滚动条绑定到 文本框的滚动条上，实现双向绑定
    connect(scrollBar,&QScrollBar::valueChanged,this,[=](){
        textEdit->horizontalScrollBar()->setValue(scrollBar->value());
    });

    // 绑定竖直滚动条
    connect(textEdit->verticalScrollBar(),&QScrollBar::valueChanged,this,[=](){
        textBrowser->verticalScrollBar()->setMinimum(textEdit->verticalScrollBar()->minimum());
        textBrowser->verticalScrollBar()->setMaximum(textEdit->verticalScrollBar()->maximum());
        textBrowser->verticalScrollBar()->setPageStep(textEdit->verticalScrollBar()->pageStep());
        textBrowser->verticalScrollBar()->setValue(textEdit->verticalScrollBar()->value());
    });
    connect(textBrowser->verticalScrollBar(),&QScrollBar::valueChanged,this,[=](){
        textEdit->verticalScrollBar()->setValue(textBrowser->verticalScrollBar()->value());
    });

}
/**
 * @brief MyTextEditByCode::onTextChanged
 * 显示行号
 */
void MyTextEditByCode::onTextChanged()
{
    connect(textEdit,&QTextEdit::textChanged,this,[=](){
        int lineCountOfTextEdit = textEdit->document()->lineCount(); //右侧文本框 count
        QString text = textBrowser->toPlainText();
        int lineCountOfTextBrower = text.trimmed().split("\n").length();   //  左侧数字框count

        //获取text 行数
        if(lineCountOfTextBrower>lineCountOfTextEdit){
            for (int i = lineCountOfTextBrower; i >lineCountOfTextEdit; --i) {
                //如果左侧大于右侧的总行数,那就减少左侧显示的行数
                text.chop((QString::number(i)+"\n").length());
            }

        }else if(lineCountOfTextBrower == 1 && text.length()<1){
            text+="1\n";
        } else if(lineCountOfTextBrower<lineCountOfTextEdit){
            //如果右侧大于左侧，那就在左侧加上行数，
            for (int i = lineCountOfTextBrower; i < lineCountOfTextEdit; ++i) {
                text+=QString::number(i+1)+"\n";
            }
        }

        //设置字数条显示宽度,获取line的宽度加上  默认宽度，每个字符宽度加5 ，宽度自适应
        textBrowser->setMaximumWidth(18+QString::number(lineCountOfTextBrower).length()*8);
        textBrowser->setText(text);

    });
}
/**
 * @brief MyTextEditByCode::initHighlighter
 * 文字高亮
 */
void MyTextEditByCode::initHighlighter()
{
    new MyHighLighter(textEdit->document());
}
/**
 * @brief MyTextEditByCode::highlightCurrentLine
 *  行高亮
 */
void MyTextEditByCode::highlightCurrentLine()
{
    //cursor  ， 设置行高亮
    connect(textEdit,&QTextEdit::cursorPositionChanged,this,[=](){
        QList<QTextEdit::ExtraSelection> extraSelection;

        QTextEdit::ExtraSelection selection;
        selection.format.setBackground(QColor(0,100,100,20));
        selection.format.setProperty(QTextFormat::FullWidthSelection,true);
        selection.cursor = textEdit->textCursor();

        //    selection.cursor.clearSelection();
        extraSelection.append(selection);

        textEdit->setExtraSelections(extraSelection);
    });
}
