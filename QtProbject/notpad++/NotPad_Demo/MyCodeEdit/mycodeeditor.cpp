#include "mycodeeditor.h"
#include "MyHighLighter/myhighlighter.h"

#include <QPainter>
#include <QDebug>
#include <QScrollBar>
#include<QFile>
#include <QFileDialog>
#include <QMessageBox>
#include<mainwindow.h>
MyCodeEditor::MyCodeEditor(QWidget *parent) : QPlainTextEdit(parent)
{

    lineNumberWidget = new LineNumberWidget(this);
    //绑定
    initConnection();

    //高亮 ,必须放在设置字体的前面
    initHighlighter();

    //设置本身和mMyHighLighter 的字体
    setAllFont(QFont("Consolas",12));

    //行高亮
    highlightCurrentLine();

    //设置边距
    updateLineNumberWidgetWidth();

    setLineWrapMode(QPlainTextEdit::NoWrap);

    getAllCharCount();  //获取字符总数
}

MyCodeEditor::~MyCodeEditor()
{

    delete lineNumberWidget;



}
/**
 * @brief MyCodeEditor::setFont
 * @param font
 * 主窗口中的设置字体
 */
void MyCodeEditor::setAllFont(QFont font)
{
    this->setFont(font);

    //高亮文字也进行设置
    mMyHighLighter->setFont(font);
    updateLineNumberWidgetWidth();

    mMyHighLighterinitAll();
}

/**
 * @brief MyCodeEditor::mMyHighLighterinitAll
 *  设置mMyHighLighterinitall  重新初始化
 */
void MyCodeEditor::mMyHighLighterinitAll()
{
    mMyHighLighter->initAll();
}

void MyCodeEditor::setfileCurrectName(QString path)
{
    fileCurrectName = path;
}


void MyCodeEditor::initConnection()
{
    //cursor
    connect(this,SIGNAL(cursorPositionChanged()),this,SLOT(highlightCurrentLine()));

    //blockCount
    connect(this,SIGNAL(blockCountChanged(int)),this,SLOT(updateLineNumberWidgetWidth()));

    //updateRequest
    connect(this,SIGNAL(updateRequest(QRect, int)),this,SLOT(updateLineNumberWidget(QRect, int)));
}


/**
 * @brief MyCodeEditor::initHighlighter
 * 字体高亮格式
 */
void MyCodeEditor::initHighlighter()
{
    mMyHighLighter =new MyHighLighter(document());
}

int MyCodeEditor::getLineNumberWidgetWidth()
{
    //获取宽度（合适）
    return 8+QString::number(blockCount()+1).length()*fontMetrics().horizontalAdvance(QChar('0'));
}

/**
 * @brief MyCodeEditor::highlightCurrentLine
 *    行高亮
 */
void MyCodeEditor::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    QTextEdit::ExtraSelection selection;
    selection.format.setBackground(QColor(0,100,100,20));
    selection.format.setProperty(QTextFormat::FullWidthSelection,true);
    selection.cursor =textCursor();

    extraSelections.append(selection);
    setExtraSelections(extraSelections);

}

void MyCodeEditor::updateLineNumberWidget(QRect rect, int dy)
{
    if(dy)
        lineNumberWidget->scroll(0,dy);
    else
        lineNumberWidget->update(0,rect.y(),getLineNumberWidgetWidth(),rect.height());
}

void MyCodeEditor::updateLineNumberWidgetWidth()
{
    //设置边距
    setViewportMargins(getLineNumberWidgetWidth(),0,0,0);
}

/**
 * @brief MyCodeEditor::getAllCharCount
 * 获取总字符数
 */
void MyCodeEditor::getAllCharCount()
{
    connect(this, &MyCodeEditor::textChanged,this, [=]() {
        //从编辑器中获取文本
        QString text = this->toPlainText();
        int lineCount = this->document()->blockCount();
        if(lineCount == 1){
            countAllChar = text.length();
        }else {
            countAllChar = text.length() - lineCount +1;
        }


        qDebug()<<countAllChar;
    });
}

void MyCodeEditor::resizeEvent(QResizeEvent *event)
{
    QPlainTextEdit::resizeEvent(event);
    lineNumberWidget->setGeometry(0,0,getLineNumberWidgetWidth(),contentsRect().height());
}

/**
 * @brief MyCodeEditor::lineNumberWidgetPaintEvent
 * @param event
 *  绘制行号条
 */
void MyCodeEditor::lineNumberWidgetPaintEvent(QPaintEvent *event)
{
    QPainter painter(lineNumberWidget);
    //绘制行号区域
    painter.fillRect(event->rect(),QColor(100,100,100,20));

    //拿到block
    QTextBlock block = firstVisibleBlock();

    //拿到行号
    int blockNumber =block.blockNumber();

    //拿到当前的block的top
    int cursorTop = blockBoundingGeometry(textCursor().block()).translated(contentOffset()).top();

    //拿到block的top
    int top = blockBoundingGeometry(block).translated(contentOffset()).top();

    //拿到block的bottom
    int bottom = top +blockBoundingRect(block).height();

    while(block.isValid()&&top<=event->rect().bottom()){
        //设置画笔颜色
        painter.setPen(cursorTop==top?Qt::black:Qt::gray);
        //绘制文字
        painter.drawText(0,top,getLineNumberWidgetWidth()-3,bottom-top,Qt::AlignRight,QString::number(blockNumber+1));

        //拿到下一个block
        block = block.next();

        top = bottom;
        bottom =  top +blockBoundingRect(block).height();
        blockNumber++;
    }
}

void MyCodeEditor::lineNumberWidgetMousePressEvent(QMouseEvent *event)
{
    setTextCursor(QTextCursor(document()->findBlockByLineNumber(event->y()/fontMetrics().height()+verticalScrollBar()->value())));
}

void MyCodeEditor::lineNumberWidgetWheelEvent(QWheelEvent *event)
{
//    if(event->orientation()==Qt::Horizontal){
//        horizontalScrollBar()->setValue(horizontalScrollBar()->value()-event->delta());
//    }else {
//       verticalScrollBar()->setValue(verticalScrollBar()->value()-event->delta());
//    }
//    event->accept();
}

/**
 * @brief MyCodeEditor::saveFile
 * @return
 * 保存文件模块
 */
bool MyCodeEditor::saveFile()
{
    QString fileName;
    qDebug()<<fileCurrectName;

    if(fileCurrectName == "newText.txt"){
        fileName =QFileDialog::getSaveFileName(this,"保存文件");
        mFileName =fileName;
    }else{
        fileName = fileCurrectName;
        mFileName = fileCurrectName;
    }
    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly|QFile::Text)){
//        QMessageBox::warning(this,"警告","无法保存文件:"+file.errorString());
        return false;
    }

    QTextStream out(&file);

    out<<toPlainText();  //保存
    file.close();


    return  true;
}

void MyCodeEditor::setFileName(QString fileName)
{
    mFileName=fileName;
}

QString MyCodeEditor::getFileName()
{
    return mFileName;
}
/**
 * @brief MyCodeEditor::saveAsFile
 * @return
 *  另存为
 */
bool MyCodeEditor::saveAsFile()
{
    QString fileName=QFileDialog::getSaveFileName(this,"另存文件");
    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly|QFile::Text)){
//        QMessageBox::warning(this,"警告","无法保存文件:"+file.errorString());
        return false;
    }

    mFileName =fileName;
    QTextStream out(&file);
    QString text =toPlainText();
    out<<text;
    file.close();


    return true;
}
