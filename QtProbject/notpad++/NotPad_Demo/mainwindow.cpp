#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QDebug>
#include<QFileDialog>
#include<QMessageBox>
#include<QFontDialog>
#include<QStatusBar>
#include<QLabel>
#include<QSet>
#include<QMenu>
#include <QtWidgets>
#include <QApplication>
#include <QMainWindow>
#include <QCloseEvent>
#include<MyCodeEdit/mycodeeditor.h>
#include<MyHighLighter/myhighlighter.h>
#include<QTimer>
#include<MyTextEdit/mytexteditbycode.h> //引入自己自定义的textedit
#include <QElapsedTimer>  //延时函数

//使用QTextCodec 需要在pro文件中添加 QT += core5compat
#include<QTextCodec>

#include<MyCodeEdit/mycodeedit.h>

//判断打印是不是可用的
#if defined(QT_PRINTSUPPORT_LIB)
#include<QtPrintSupport/qtprintsupportglobal.h>
#if QT_CONFIG(printer)
#if QT_CONFIG(printdialog)
#include<QPrintDialog>
#endif
#include<QPrinter>
#endif
#endif

// 保存配置的库
#include "QSettings"
//qsettings对象初始化
QSettings * mSettings;
//从ini文件中获取保存的历史打开记录
QList<QString> getHistory();

void saveHistory(QString path);


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{

    ui->setupUi(this);



    this->setWindowTitle("NotePad++");
    this->setWindowIcon(QIcon(":/imgs/title.png"));

    //设置文本框居中
    this->setCentralWidget(ui->tabWidget);



    //初始化qsettings对象
    //qsettings实际就是编译生成一个settings.ini的配置文件，
    //配置文件中可以存储自己需要他存储的东西
    if(mSettings==NULL){
        mSettings=new QSettings("settings.ini",QSettings::IniFormat);
    }


//设置打印机
#if !QT_CONFIG(printer)
    ui->printer->setEnabled(false);
#endif

    //初始化状态栏
//    new_Statics();

    //获取历史记录并显示
    initMenu();


    getTabTitle(); //每隔半秒就设置标题
}

MainWindow::~MainWindow()
{
    delete ui;
}



//清空全部历史记录的处理信号
void MainWindow::on_clear_history_triggered()
{
    qDebug()<<"on_clear_history_triggered...";
    //清空数据
    mSettings->clear();

    initMenu();
}

//打开  init  显示 初始化
void MainWindow::initMenu()
{
    QMenu *recent =this->findChild<QMenu *>("recent");
//    qDebug()<<recent->title();
    // 获取recent下的所有子菜单
    QList<QObject *>chList =recent->children().toList();
    foreach (QObject *ch, chList) {
        QAction *action =(QAction *)ch;
        // 清空子菜单栏
        recent->removeAction(action);
    }
    QList<QString> lists =getHistory();

    //从尾部开始遍历
    for(int i=lists.size()-1;i>=0;i--){
        recent->addAction(lists[i],this,&MainWindow::on_open_rencent_file);
    }

    // 新建清空action，并给他槽函数
    if(lists.size()>0){
        recent->addAction("清除历史记录",this,&MainWindow::on_clear_history_triggered);
    }
}







//获取历史记录
QList<QString> getHistory(){
    //打开开始读
    int size =mSettings->beginReadArray("history");

    //创建返回对象
    QList<QString> lists;
    for(int i=0;i<size;i++){
        //通过索引获取对应的value，才能获取vaule
        mSettings->setArrayIndex(i);

        QString path =mSettings->value("path").toString();

        lists.append(path);

//        qDebug()<<path;
    }

    //关闭读
    mSettings->endArray();

    return lists;
}

//  保存文件打开的历史记录数组************去重*********去重******
void saveHistory(QString path){

    //获取文件中的历史路径
    QList<QString> lists = getHistory();
    //先获取ini文件中的全部保存的·路径，再判断传入的路径是否存在，存在则删除路径
    //使用lists删除是因为这个列表数据是写入ini文件中的，所有先删后写也行
    foreach (QString str, lists) {
        if(str==path){
            lists.removeOne(str);
        }
    }
    lists.append(path);
    //打开开始写入
    mSettings->beginWriteArray("history");

    for(int i=0;i<lists.size();i++){
        mSettings->setArrayIndex(i);
        //保存字符串
        mSettings->setValue("path",lists[i]);
    }

    //关闭开始写入
    mSettings->endArray();

    //每次保存路径的时候都要initmenu 才能更新显示的那些路径

}

//新建状态栏并显示
//void MainWindow::new_Statics()
//{
//    //设置状态栏,new一个状态栏，并把他set进窗口中
//    statusBar = new QStatusBar(this);
//    setStatusBar(statusBar);

//    //创建统计字符的lable标签，并把它放进statusBar中
//    QLabel* charCountLabel = new QLabel(this);
//    statusBar->addWidget(charCountLabel);

//    // 创建标签以显示字符编码
//    QLabel* encodingLabel = new QLabel(this);
//    statusBar->addWidget(encodingLabel);

//    //更新字符数
//    charCountLabel->setText(QString("字符数： %1").arg(codeEditor->countAllChar));

//    // 更新编码标签
//    QTextCodec* codec = QTextCodec::codecForName("UTF-8"); // 将此更改为所需的编码
//    QString encoding = codec->name();
//    encodingLabel->setText(QString("编码： %1").arg(encoding));

//}

//按action 打开文件 ，打开历史记录中的文件
void MainWindow::on_open_rencent_file()
{
    //内置的函数sender，能够获取点击的那个函数对象，
    QAction * action =(QAction *)sender();

    QString filePath =action->text();
//    qDebug()<<filePath;
    //如果用户没有打开文件则return，否则会将空路径也写入
    if (filePath=="") {
        return;
    }
    QFile file(filePath);

    // 设置文件打开方式，必须设置，不然打不开
    file.open(QIODeviceBase::ReadWrite);

    currentFile=filePath;
    setWindowTitle(filePath);

    //读文件的内容,尽量使用qtextstream 来读取，支持多种类型的编码
    //传入的对象是file对象
    QTextStream in(&file);
    QString text=in.readAll();
    MyCodeEditor * codeEditor = new MyCodeEditor(this);
    codeEditor->setPlainText(text);

    ui->tabWidget->addTab(codeEditor,currentFile);
    ui->tabWidget->setCurrentIndex(ui->tabWidget->count()-1);


    //关闭文件
    file.close();
    //判断文本有没有被编辑,被编辑就写一个*，保存好后*号消失
//    connect(ui->textEdit,&QTextEdit::textChanged,[=](){
//        //如果文本被编辑，则
//        setWindowTitle("*"+filePath);
//    });

    //将打开文件路径写入ini文件里
    saveHistory(currentFile);

}





//新建
void MainWindow::on_new_file_triggered()
{

    //新建文件的时候创建一个tab，并显示他
    // 新建自己自定义的 textedit
//    ui->tabWidget->addTab(new MyTextEditByCode(this),"newTabl.xjy");
    ui->tabWidget->addTab(new MyCodeEditor(this),"newText.txt");


}



//打开文件
void MainWindow::on_open_file_triggered()
{
    //参数四可以设置打开文档的后缀名
    QString filePath =QFileDialog::getOpenFileName(this,"打开文件","C:");
    qDebug()<<filePath;
    //如果用户没有打开文件则return，否则会将空路径也写入
    if (filePath=="") {
        return;
    }

    //获取这个文件对象
    QFile file(filePath);

    // 设置文件打开方式，必须设置，不然打不开
    file.open(QIODeviceBase::ReadWrite);
    currentFile=filePath;
    setWindowTitle(currentFile);


    //读文件的内容,尽量使用qtextstream 来读取，支持多种类型的编码
    //传入的对象是file对象
    QTextStream in(&file);
    QString text=in.readAll();
    MyCodeEditor * codeEditor = new MyCodeEditor(this);
    codeEditor->setPlainText(text);
    ui->tabWidget->addTab(codeEditor,currentFile);
    ui->tabWidget->setCurrentIndex(ui->tabWidget->count()-1);

    file.close();


    // 判断文本有没有被编辑,被编辑就写一个*，保存好后*号消失
//    connect(codeEditor,&MyCodeEditor::textChanged,[=](){
//        //如果文本被编辑，则
//        setWindowTitle("*"+currentFile);
//        if (codeEditor->document()->isModified()){
//            setWindowTitle("*"+currentFile);
//        }
//        else{
//            return;
//        }
//    });

    //将打开文件路径写入saveHistory中
    saveHistory(currentFile);

    //文件路径写入history中后再使用init显示文件路径
    initMenu();


}

/**
 * @brief MainWindow::getTabTitle
 * @return
 * 每隔0.5秒获取当前tab标签的值,并设置到tab标签
 */
QString MainWindow::getTabTitle()
{

    QTimer *timer = new QTimer(this);
    connect(timer,&QTimer::timeout,this,[=](){
        int index = ui->tabWidget->currentIndex();
        if(index == -1){
            setWindowTitle("NotePad++");

        }

        currentTabText = ui->tabWidget->tabText(index);
        setWindowTitle(currentTabText);
    });
    timer->start(500);

}
//保存文件
void MainWindow::on_save_file_triggered()
{
    if(ui->tabWidget->currentIndex() == -1){
        return ;
    }


    //把保存交给CodeEditor
    MyCodeEditor *codeEditor = (MyCodeEditor* )ui->tabWidget->currentWidget();
    //保存文件前先获取当前tab标题， 用来判断文件是否保存过
    int index = ui->tabWidget->currentIndex();
    currentTabText = ui->tabWidget->tabText(index);
    codeEditor->setfileCurrectName(currentTabText);

    if(codeEditor){
        if(codeEditor->saveFile()){
            saveSuccessAction(codeEditor);

        }
    }

    //保存成功后，修改windows 标题为保存成功，随后立即修改回原来的
    setWindowTitle("保存成功");

    QElapsedTimer t;
    t.start();
    while(t.elapsed()<100);

    setWindowTitle(currentTabText);

}
/**
 * @brief MainWindow::saveSuccessAction
 * @param codeEditor
 * 保存文件函数
 */
void MainWindow::saveSuccessAction(MyCodeEditor *codeEditor)
{
    QString fileName  = codeEditor->getFileName();
    //保存历史
    saveHistory(fileName);
//    //设置tab标题
    ui->tabWidget->setTabText(ui->tabWidget->currentIndex(),fileName);
    //初始化菜单
    initMenu();
}

//另存文件
void MainWindow::on_save_asfile_triggered()
{
    if(ui->tabWidget->currentIndex() == -1){
        return ;
    }

    //把另存为交给CodeEditor
      MyCodeEditor *codeEditor = (MyCodeEditor* )ui->tabWidget->currentWidget();
      if(codeEditor){
          if(codeEditor->saveAsFile()){
              //保存历史
              QString fileName  = codeEditor->getFileName();
              //设置tab标题
              ui->tabWidget->setTabText(ui->tabWidget->currentIndex(),fileName);
              saveHistory(fileName);
              //初始化菜单
              initMenu();
          }
      }
    qDebug()<<"另存成功";

    //将打开文件路径写入ini文件里
//    saveHistory(filePath);
}

//复制
void MainWindow::on_copy_triggered()
{
    if(ui->tabWidget->currentIndex() == -1){
        return ;
    }
    //把复制交给CodeEditor
    MyCodeEditor *codeEditor = (MyCodeEditor* )ui->tabWidget->currentWidget();
    if(codeEditor){
        codeEditor->copy();
    }
}

//剪切
void MainWindow::on_cut_triggered()
{
    if(ui->tabWidget->currentIndex() == -1){
        return ;
    }
    //把剪切交给CodeEditor
    MyCodeEditor *codeEditor = (MyCodeEditor* )ui->tabWidget->currentWidget();
    if(codeEditor){
        codeEditor->cut();
    }
}

//粘贴
void MainWindow::on_past_triggered()
{
    if(ui->tabWidget->currentIndex() == -1){
        return ;
    }
    //把粘贴交给CodeEditor
    MyCodeEditor *codeEditor = (MyCodeEditor* )ui->tabWidget->currentWidget();
    if(codeEditor){
        codeEditor->paste();
    }
}


//字体
void MainWindow::on_font_triggered()
{
    if(ui->tabWidget->currentIndex() == -1){
        return ;
    }
    //使用QfontDialog控件,选择文字的控件
    bool fontSelected;
    QFont font =QFontDialog::getFont(&fontSelected,QFont("Consolas,12"),this);
    if (fontSelected) {
        //拿到字体 ，将字体交给codeeditor
        MyCodeEditor *codeEditor = (MyCodeEditor* )ui->tabWidget->currentWidget();
        if(codeEditor){
            codeEditor->setAllFont(font);
        }

    }
}

//提示信息
void MainWindow::on_info_triggered()
{
    QMessageBox::about(this,"notpad++","还有许多功能还未完善!凑合用吧,感谢");
}

//撤回
void MainWindow::on_undo_triggered()
{
    if(ui->tabWidget->currentIndex() == -1){
        return ;
    }
    //把撤销交给CodeEditor
    MyCodeEditor *codeEditor = (MyCodeEditor* )ui->tabWidget->currentWidget();
    if(codeEditor){
        codeEditor->undo();
    }
}

//取消撤回
void MainWindow::on_redo_triggered()
{
    if(ui->tabWidget->currentIndex() == -1){
        return ;
    }
    //把取消撤销交给CodeEditor
    MyCodeEditor *codeEditor = (MyCodeEditor* )ui->tabWidget->currentWidget();
    if(codeEditor){
        codeEditor->redo();
    }
}

//退出
void MainWindow::on_exit_triggered()
{
    QCoreApplication::exit();
}


//放大
void MainWindow::on_big_triggered()
{
    if(ui->tabWidget->currentIndex() == -1){
        return ;
    }
    //放大字体
    MyCodeEditor *codeEditor = (MyCodeEditor* )ui->tabWidget->currentWidget();
    QFont font = codeEditor->font();
    font.setPointSize(font.pointSize() + 1);
    codeEditor->mMyHighLighterinitAll();
    codeEditor->setAllFont(font);
}

//缩小
void MainWindow::on_samll_triggered()
{
    if(ui->tabWidget->currentIndex() == -1){
        return ;
    }
    //放大字体
    MyCodeEditor *codeEditor = (MyCodeEditor* )ui->tabWidget->currentWidget();
    QFont font = codeEditor->font();
    font.setPointSize(font.pointSize() - 1);
    codeEditor->mMyHighLighterinitAll();
    codeEditor->setAllFont(font);
}




//打印

void MainWindow::on_printer_triggered()
{
    if(ui->tabWidget->currentIndex() == -1){
        return ;
    }
#if defined(QT_PRINTSUPPORT_LIB) && QT_CONFIG(printer)
    QPrinter printDev;
#if QT_CONFIG(printdialog)
    QPrintDialog dialog(&printDev,this);
    if(dialog.exec()==QDialog::Rejected)
        return;
#endif
    //把打印交给CodeEditor
    MyCodeEditor *codeEditor = (MyCodeEditor* )ui->tabWidget->currentWidget();
    if(codeEditor){
        codeEditor->print(&printDev);
    }
#endif
}

//关闭窗口的事件，直接在mainwindow中重写这个函数就可以默认实现
//void MainWindow::closeEvent(QCloseEvent *event)
//{
//    if (ui->textEdit->document()->isModified()) {
//        QMessageBox::StandardButton reply = QMessageBox::question(
//                    this, "保存",
//                    "The text has been modified. Do you want to save your changes?",
//                    QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);


//        // 如果用户选择保存，保存文本并关闭窗口
//        if (reply == QMessageBox::Save) {
//            //使用ui直接click点击保存
//            ui->save_file->triggered();
//            qDebug()<<currentFile;
//            event->accept();  // 允许窗口关闭
//        }
//        // 如果用户选择放弃更改，直接关闭窗口
//        else if (reply == QMessageBox::Discard) {
//            event->accept();  // 允许窗口关闭
//        }
//        // 如果用户选择取消，取消窗口关闭事件
//        else {
//            event->ignore();  // 取消窗口关闭
//        }
//    }
//    // 如果文本编辑器没有被修改，直接关闭窗口
//    else {
//        event->accept();
//    }
//}

//void MainWindow::save_Exit_Tipe()
//{

//}

/**
 * @brief MainWindow::on_tabWidget_tabCloseRequested
 * @param index
 *  tab关闭
 */
void MainWindow::on_tabWidget_tabCloseRequested(int index)
{
//    delete ui->tabWidget->currentWidget();

    MyCodeEditor * codeEditor = (MyCodeEditor*)ui->tabWidget->currentWidget();

    if (codeEditor->document()->isModified()) {
        QMessageBox::StandardButton btn = QMessageBox::question(this,"警告","您还没有保存文档！是否保存？",QMessageBox::Yes|QMessageBox::No);
        if(btn ==QMessageBox::Yes){
            if(codeEditor->saveFile()){
                saveSuccessAction(codeEditor);

            }
            delete codeEditor;
            ui->tabWidget->removeTab(index);
        }
        //为否那就关闭 tab
        else if(btn ==QMessageBox::No){
            delete codeEditor;
            ui->tabWidget->removeTab(index);
        }
    }else{
        //文本没有被修改那就 直接退出
    delete codeEditor;
    ui->tabWidget->removeTab(index);
    }

}


