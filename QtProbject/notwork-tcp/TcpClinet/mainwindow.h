#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<QTcpSocket>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QTcpSocket * tcpsocket;
private slots:
    void on_openButton_clicked();

    void on_closeButton_clicked();

    void on_sendButton_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
