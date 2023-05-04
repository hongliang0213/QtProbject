#include "mytextedit.h"
#include "ui_mytextedit.h"
#include <myhighlighter.h>
#include<QDebug>
#include<QTextCursor>
#include <QKeyEvent>
#include <QApplication>

/**
 * @brief MyTextEdit::keyPressEvent
 * @param event
 * 按下tab只缩进四个空格
 */
void MyTextEdit::keyPressEvent(QKeyEvent *event)
{
//    if (event->key() == Qt::Key_Tab) {
//        QTextCursor cursor(ui->textEdit->document());
//        cursor.insertText("    "); // 添加四个空格
//        ui->textEdit->setCursor(&cursor);
//    } else {
//        QTextEdit::keyPressEvent(event);
//    }
}

MyTextEdit::MyTextEdit(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MyTextEdit)
{
    ui->setupUi(this);
    BindingHorizontalScrollBar();  //滚动条绑定

    initFont();  //设置字体

    TextLine();  //设置行数显示

    initHighLighter();   //设置高亮

    setCurrentLine();  //设置每行高亮
}

MyTextEdit::~MyTextEdit()
{
    delete ui;
}
/**
 * @brief MyTextEdit::BindingHorizontalScrollBar
 * 绑定文本框的滚动条到底部滚动条
 */
void MyTextEdit::BindingHorizontalScrollBar()
{

    // 将文本框自己的滚动条绑定到 底部新建的滚动条上
    connect(ui->textEdit->horizontalScrollBar(),&QScrollBar::valueChanged,this,[=](){
        ui->horizontalScrollBar->setMinimum(ui->textEdit->horizontalScrollBar()->minimum());
        ui->horizontalScrollBar->setMaximum(ui->textEdit->horizontalScrollBar()->maximum());
        ui->horizontalScrollBar->setPageStep(ui->textEdit->horizontalScrollBar()->pageStep());
        ui->horizontalScrollBar->setValue(ui->textEdit->horizontalScrollBar()->value());

    });
    //将最底部滚动条绑定到 文本框的滚动条上，实现双向绑定
    connect(ui->horizontalScrollBar,&QScrollBar::valueChanged,this,[=](){
        ui->textEdit->horizontalScrollBar()->setValue(ui->horizontalScrollBar->value());
    });

    // 绑定竖直滚动条
    connect(ui->textEdit->verticalScrollBar(),&QScrollBar::valueChanged,this,[=](){
        ui->textBrowser->verticalScrollBar()->setMinimum(ui->textEdit->verticalScrollBar()->minimum());
        ui->textBrowser->verticalScrollBar()->setMaximum(ui->textEdit->verticalScrollBar()->maximum());
        ui->textBrowser->verticalScrollBar()->setPageStep(ui->textEdit->verticalScrollBar()->pageStep());
        ui->textBrowser->verticalScrollBar()->setValue(ui->textEdit->verticalScrollBar()->value());
    });
    connect(ui->textBrowser->verticalScrollBar(),&QScrollBar::valueChanged,this,[=](){
        ui->textEdit->verticalScrollBar()->setValue(ui->textBrowser->verticalScrollBar()->value());
    });

    //cursor  ， 设置行高亮
    connect(ui->textEdit,&QTextEdit::cursorPositionChanged,this,[=](){
        setCurrentLine();  //设置行高亮
    });
}
/**
 * @brief MyTextEdit::initFont
 * 设置字体
 */
void MyTextEdit::initFont()
{
    QFont font("Consolas",11);
    QFont font1("Consolas",11);
    ui->textEdit->setFont(font);
    ui->textBrowser->setFont(font1);


}
/**
 * @brief MyTextEdit::TextLine
 * 设置行号
 */
void MyTextEdit::TextLine()
{

    connect(ui->textEdit,&QTextEdit::textChanged,this,[=](){
        int lineCountOfTextEdit = ui->textEdit->document()->lineCount(); //右侧文本框 count
        QString text = ui->textBrowser->toPlainText();
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
        ui->textBrowser->setMaximumWidth(18+QString::number(lineCountOfTextBrower).length()*8);

        ui->textBrowser->setText(text);
    });
}
/**
 * @brief MyTextEdit::initHighLighter
 * 设置文本高亮
 */
void MyTextEdit::initHighLighter()
{
    //改函数默认构造函数中传入 QTextDocument,将需要高亮
    new MyHighLighter(ui->textEdit->document());
}
/**
 * @brief MyTextEdit::setCurrentLine
 *  设置行高亮
 */
void MyTextEdit::setCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelection;

    QTextEdit::ExtraSelection selection;
    selection.format.setBackground(QColor(0,100,100,20));
    selection.format.setProperty(QTextFormat::FullWidthSelection,true);
    selection.cursor = ui->textEdit->textCursor();

//    selection.cursor.clearSelection();
    extraSelection.append(selection);

    ui->textEdit->setExtraSelections(extraSelection);
}
