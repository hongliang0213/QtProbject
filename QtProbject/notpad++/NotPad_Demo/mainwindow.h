#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QMainWindow>
#include<MyCodeEdit/mycodeeditor.h>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


class MainWindow;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();





    //获取打开的文路径
    QString currentFile;

    //覆写closeEvent函数  ,让其关闭时出现提示
//    void closeEvent(QCloseEvent *event);


    //保存文件路径的函数
//    void saveHistory(QString path);

    // 每一秒获取当前tab标签的值
    QString getTabTitle();
    QString currentTabText;   // 当前tab标签的值
private slots:

    void on_clear_history_triggered(); // 清除全部历史记录

    void on_new_file_triggered();     // 新建文件

    void on_open_file_triggered();  // 打开文件

    void on_save_file_triggered(); //保存文件

    void on_save_asfile_triggered(); // 另存

    void on_copy_triggered();  //复制

    void on_cut_triggered();  //剪切

    void on_past_triggered(); //粘贴

    void on_font_triggered();  //字体

    void on_info_triggered();  //关于

    void on_undo_triggered(); //撤回

    void on_redo_triggered();  //取消撤回

    void on_exit_triggered();  //退出

    void on_big_triggered();  //放大字体

    void on_samll_triggered(); //缩小字体


    void on_printer_triggered();   //打印


    void on_tabWidget_tabCloseRequested(int index);



private:
    Ui::MainWindow *ui;




    void initMenu();//历史记录菜单生成初始化

    void saveSuccessAction(MyCodeEditor * codeEditor); //   保存文件模块

//    void new_Statics();//显示状态栏的函数

    void on_open_rencent_file();//打开历史记录中的文件

//    void save_Exit_Tipe();



};
#endif // MAINWINDOW_H
