#include "myhighlighter.h"
#include<QFile>
#include<QTextStream>
#include"QTextEdit"

MyHighLighter::MyHighLighter(QTextDocument *parent)
    :QSyntaxHighlighter(parent)
{
    addNormalTextFornmat(); // 普通字符

    addNumberFormat();  //数字

    addStringFormat(); //字符串

    addCommentFormat(); //注释

    addKeywordsFormat();  //关键字注释

    addClassNameFormat();    //类名高亮

    addFunctionFormat();  //方法名高亮

}

/**
 * @brief MyHighLighter::highlightBlock
 * @param text
 * 重写父类中的highlightBlock 虚函数
 */
void MyHighLighter::highlightBlock(const QString &text)
{
    foreach (const HighlightRule &rule, highlightRules) {
        QRegExp regExp(rule.pattern);  //获取正则表达式
        int index = regExp.indexIn(text);  // 获取匹配成功的字符索引
        while (index>=0) {
            int length=regExp.matchedLength(); // 获取总的字符长度
            setFormat(index,length,rule.format);
            index = regExp.indexIn(text,index+length);

        }
    }
    //多行注释
    addMultiLineCommentFormat(text);

}
/**
 * @brief MyHighLighter::addNormalTextFornmat
 * 普通文本高亮
 */
void MyHighLighter::addNormalTextFornmat()
{
    HighlightRule rule;   //定义一个匹配规则和语法高亮,匹配规则为正则表达式
    rule.pattern = QRegExp("[a-zA-Z0-9]+");  //正则表达式，+表示可以连续

    QTextCharFormat normalTextFormat;  //定义字体
    normalTextFormat.setFont(QFont(m_FontFamily,m_FontSize));
    normalTextFormat.setForeground(QColor(0,0,0));  //设置前景色

    rule.format = normalTextFormat;

    //将普通文本的规则添加到容器里面存放
    highlightRules.append(rule);

}
/**
 * @brief MyHighLighter::addNumberFormat
 * 数字高亮
 */
void MyHighLighter::addNumberFormat()
{
    HighlightRule rule;   //定义一个匹配规则和语法高亮,匹配规则为正则表达式
    rule.pattern = QRegExp("\\b\\d+|\\d+\\.\\d+\\b");  //正则表达式，+表示可以连续

    QTextCharFormat numberTextFormat;  //定义字体
    numberTextFormat.setFont(QFont(m_FontFamily,m_FontSize));
    numberTextFormat.setForeground(QColor(250,80,50));  //设置前景色

    rule.format = numberTextFormat;

    //将普通文本的规则添加到容器里面存放
    highlightRules.append(rule);
}
/**
 * @brief MyHighLighter::addStringFornmat
 * 字符串高亮
 */
void MyHighLighter::addStringFormat()
{
    HighlightRule rule;   //定义一个匹配规则和语法高亮,匹配规则为正则表达式
    QTextCharFormat stringTextFormat;  //定义字体
    stringTextFormat.setFont(QFont(m_FontFamily,m_FontSize));
    stringTextFormat.setForeground(QColor(0,180,180));  //设置前景色

    rule.format = stringTextFormat;

    rule.pattern = QRegExp("'[^']*'");  //多条匹配  ''  [^'] 在''  中不包含'符号
    highlightRules.append(rule);

    rule.pattern = QRegExp("\"[^\"]*\"");   // ""
    highlightRules.append(rule);

    rule.pattern = QRegExp("`[^`]*`");
    highlightRules.append(rule);

}
/**
 * @brief MyHighLighter::addCommentForamt
 * 字符串注释
 */
void MyHighLighter::addCommentFormat()
{
    HighlightRule rule;   //定义一个匹配规则和语法高亮,匹配规则为正则表达式
    QTextCharFormat commentTextFormat;  //定义字体
    commentTextFormat.setFont(QFont(m_FontFamily,m_FontSize));
    commentTextFormat.setForeground(Qt::darkGreen);  //设置前景色

    rule.format = commentTextFormat;

    rule.pattern = QRegExp("\\/\\/.*$");  // //data  单行  $ 末尾
    highlightRules.append(rule);

//    rule.pattern = QRegExp("\\/\\*[^*]*\\*+(?:[^/*][^*]*\\*+)*\\/");   //  /*  */  多行字符
//    highlightRules.append(rule);


}
/**
 * @brief MyHighLighter::addMultiLineCommentFormat
 * 多行注释
 */
void MyHighLighter::addMultiLineCommentFormat(const QString &text)
{
    // 初始化正则表达式，匹配注释
    commentStartExpression = QRegularExpression("/\\*");
    commentEndExpression = QRegularExpression("\\*/");
    // 设置注释的高亮格式为绿色
    commentFormat.setForeground(Qt::darkGreen);


    // 高亮注释
    setCurrentBlockState(0);

    int startIndex = 0;
    if (previousBlockState() != 1)
        startIndex = text.indexOf(commentStartExpression);

    while (startIndex >= 0) {
        QRegularExpressionMatch match = commentEndExpression.match(text, startIndex);
        int endIndex = match.capturedStart();
        int commentLength = 0;
        if (endIndex == -1) {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        } else {
            commentLength = endIndex - startIndex
                    + match.capturedLength();
        }
        setFormat(startIndex, commentLength, commentFormat);
        startIndex = text.indexOf(commentStartExpression, startIndex + commentLength);
    }
}
/**
 * @brief MyHighLighter::addKeywordsFormat
 *  关键字高亮
 */
void MyHighLighter::addKeywordsFormat()
{
    //打开文件
    QFile file(":/config/config/keywords.txt");
    QTextStream keyWordsStream(&file);

    HighlightRule rule;
    QTextCharFormat keyTextFormat;  //定义字体
    keyTextFormat.setFont(QFont(m_FontFamily,m_FontSize));
    keyTextFormat.setForeground(QColor(128,128,0));  //设置前景色
    rule.format = keyTextFormat;

    //判断是否能成功打开文件，
    if(file.open(QIODevice::ReadOnly|QIODevice::Text)){
        // 匹配文件中的关键字对他进行高亮处理
        keyWordsStream.seek(0);  //  从头开始
        QString line;  //需要匹配的关键字
        //判断是不是到来末尾
        while (!keyWordsStream.atEnd()) {
            line  = keyWordsStream.readLine();
            if(line!=""){
                qDebug()<<line;
                rule.pattern = QRegExp("\\b"+line+"\\b");
                highlightRules.append(rule);
            }
        }
        file.close();
    }else
    {
        qDebug()<<file.errorString();
    }


}
/**
 * @brief MyHighLighter::addClassNameFormat
 *  类名高亮
 */
void MyHighLighter::addClassNameFormat()
{
    HighlightRule rule;   //定义一个匹配规则和语法高亮,匹配规则为正则表达式
    rule.pattern = QRegExp("\\b[A-Z]+\\w*");  //正则表达式，+表示可以连续

    QTextCharFormat ClassTextFormat;  //定义字体
    ClassTextFormat.setFont(QFont(m_FontFamily,m_FontSize));
    ClassTextFormat.setForeground(QColor(151,0,172));  //设置前景色
    ClassTextFormat.setFontWeight(99);

    rule.format = ClassTextFormat;

    //将普通文本的规则添加到容器里面存放
    highlightRules.append(rule);
}

/**
 * @brief MyHighLighter::addFunctionFormat
 *  方法名高亮
 */
void MyHighLighter::addFunctionFormat()
{
    HighlightRule rule;


    QTextCharFormat functionTextFormat;  //定义字体
    functionTextFormat.setFont(QFont(m_FontFamily,m_FontSize));
    functionTextFormat.setForeground(QColor(0,152,183));  //设置前景色
    functionTextFormat.setFontWeight(99);

    rule.format = functionTextFormat;

    rule.pattern = QRegExp("\\w+\\(");
    highlightRules.append(rule);
    rule.pattern = QRegExp("\\)");
    highlightRules.append(rule);

}


