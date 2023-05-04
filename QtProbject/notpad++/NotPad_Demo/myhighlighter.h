#ifndef MYHIGHLIGHTER_H
#define MYHIGHLIGHTER_H

#include <QObject>
#include<QRegExp>

#include<QSyntaxHighlighter>  //语法高亮器类
#include <QRegularExpression>
class MyHighLighter: public QSyntaxHighlighter
{
public:
    explicit MyHighLighter(QTextDocument *parent=nullptr);

protected:
    void highlightBlock(const QString &text);  //重写这个父类的虚函数,构造函数

private:
    // 多行注释中使用的参数
    QRegularExpression commentStartExpression;
    QRegularExpression commentEndExpression;
    QTextCharFormat commentFormat;


    //字体默认格式
    QString m_FontFamily ="Consolas";
    int m_FontSize =11;

    struct HighlightRule
    {
        QRegExp pattern;
        QTextCharFormat format;

    };
    QVector<HighlightRule> highlightRules;   //容器类型数据
    void addNormalTextFornmat();  //普通文本高亮
    void addNumberFormat();  //数字高亮
    void addStringFormat();   // 字符串
    void addCommentFormat();  //注释
    void addMultiLineCommentFormat(const QString &text); //多行注释
    void addKeywordsFormat();   //关键字注释
    void addClassNameFormat();   //类名高亮
    void addFunctionFormat();  // 方法名高亮

};

#endif // MYHIGHLIGHTER_H
