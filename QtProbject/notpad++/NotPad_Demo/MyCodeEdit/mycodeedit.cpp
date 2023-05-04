#include "mycodeedit.h"
#include<MyHighLighter/myhighlighter.h>
#include<QPainter>
MyCodeEdit::MyCodeEdit(QWidget *parent)
    : QPlainTextEdit{parent}
{
    //初始化绘制类
    lineNumberWidget = new LineNumberWidget(this);

    //初始字体
    initFont();

    // 文本高亮
    initHighlighter();

    //行高亮
    initHighlighterLine();

    //设置边距，防止输入箭头进入左侧行号区域
    updateLineNumberWidgetWidth();
}
/**
 * @brief MyCodeEdit::lineNumberWidgetPaintEvent
 * @param event
 * 绘制行号
 */
void MyCodeEdit::lineNumberWidgetPaintEvent(QPaintEvent *event)
{
    QPainter painter(lineNumberWidget);
    // 绘制行号区域
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
        painter.drawText(0,top,25,bottom-top,Qt::AlignRight,QString::number(blockNumber+1));

        //拿到下一个block
        block = block.next();

        top = bottom;
        bottom =  top +blockBoundingRect(block).height();
        blockNumber++;
    }
    //需要一个信号，来实现每行都显示行数
    initUpdate();
}
/**
 * @brief MyCodeEdit::resizeEvent
 * @param event
 * 设置绘制的行号边距大小
 */
void MyCodeEdit::resizeEvent(QResizeEvent *event)
{
    QPlainTextEdit::resizeEvent(event);
    QRect rect =    contentsRect();
    lineNumberWidget->setGeometry(QRect(rect.left(),rect.top(),25,rect.height()));
}
/**
 * @brief MyCodeEdit::updateLineNumberWidgetWidth
 *
 * 自适应边距
 */
void MyCodeEdit::updateLineNumberWidgetWidth()
{
    setViewportMargins(getLineNumberWidgetWidth(),0,0,0);
}

/**
 * @brief MyCodeEditor::initFont
 * 初始化字体
 */
void MyCodeEdit::initFont()
{
    this->setFont(QFont("Consolas",11));
}

/**
 * @brief MyCodeEdit::initHighlighterLine
 * 行高亮
 */

void MyCodeEdit::initHighlighterLine()
{
    //cursor  ， 设置行高亮   ,整个控件就是文本框就是this  使所以发送信号的是this
    connect(this,&MyCodeEdit::cursorPositionChanged,this,[=](){
        QList<QTextEdit::ExtraSelection> extraSelection;

        QTextEdit::ExtraSelection selection;
        selection.format.setBackground(QColor(0,100,100,20));
        selection.format.setProperty(QTextFormat::FullWidthSelection,true);
        selection.cursor = textCursor();

        //    selection.cursor.clearSelection();
        extraSelection.append(selection);

        setExtraSelections(extraSelection);
    });
}

/**
 * @brief MyCodeEdit::initHighlighter
 * 文本高亮
 */
void MyCodeEdit::initHighlighter()
{
    new MyHighLighter(document());
}
/**
 * @brief MyCodeEdit::initUpdate
 * 更新行数信号
 */
void MyCodeEdit::initUpdate()
{
    connect(this,&MyCodeEdit::updateRequest,this,[=](QRect rect, int dy){
        if(dy)
            lineNumberWidget->scroll(0,dy);
        else
            lineNumberWidget->update(0,rect.y(),getLineNumberWidgetWidth(),rect.height());

    });
}
/**
 * @brief MyCodeEdit::getLineNumberWidgetWidth
 * @return
 *  获取合适宽度，自适应行的宽度
 */
int MyCodeEdit::getLineNumberWidgetWidth()
{
    return 8+QString::number(blockCount()+1).length()*fontMetrics().horizontalAdvance(QChar('0'));
}
