#include "videoform.h"
#include "ui_videoform.h"

#define VIDEO_FROM_WIDTH 420  //默认宽
#define VIDEO_FROM_HEIGHT 880 //默认高
#include<QMessageBox>
#include<QToolBar>
#include<QDockWidget>
#include <QMainWindow>
VideoForm::VideoForm(const QString& serial,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::VideoForm),
    m_serial(serial)
{


    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);  //当执行了close或者关闭窗口时，会自动关闭，执行析构函数

    createQTollBar();   //磁吸工具条

    //链接server中的相关信号查看服务启动结果
    connect(&m_server,&server::serverSatrtResult,this,[=](bool success){
        qDebug()<<"server push "<<success;

    });
    connect(&m_server,&server::connectToResult,this,[=](bool succcess,const QString & deviceName,const QSize& size){
        qDebug()<<"server connectToResult "<<succcess;  // 成功建立域套接字连接
        qDebug()<<"server deviceName "<<deviceName;
        qDebug()<<"server Qsize "<<size;

        //更新窗口和 size
        setWindowTitle(deviceName);
        updateShowSzie(size);


        //成功创建连接后，从devicesocket中读取数据
        if(succcess){

            m_decoder.setDeviceSocket(m_server.getDeviceSocket());  // 将devicesocket设置到解码器上
            m_decoder.startDecode();   // 启动解码器
            m_inputConvert.setDeviceSocket(m_server.getDeviceSocket());
        }
    });

    m_frames.init();  //    初始化视频帧对象
    m_decoder.setFrames(&m_frames);   // 使用m_frames对象保存视屏帧

    connect(&m_server,&server::onServerStop,this,[=](){  //当server服务中断时，关闭窗口
        close();
        qDebug()<<"server process stop";
    });
    connect(&m_decoder,&Decoder::onDecodeStop,this,[=](){  //当解码线程退出时，关闭服务
        close();
        qDebug()<<"decoder process stop";
    });

    // 将一帧解码成功后会抛出一个newframes的新帧信号
    connect(&m_decoder,&Decoder::onNewFrame,this,[=](){
//        qDebug()<<"Decoder :: newonFrames";
        m_frames.lock();
        const AVFrame * frame = m_frames.consumeRenderedFrame();  //获取解码的帧
        updateShowSzie(QSize(frame->width,frame->height)); //更新宽高

        //渲染frame,调用opengl渲染模块 渲染yuv 数据,  ui 中的videowidget已经提升为渲染模块了，可以进行渲染
        ui->Videowidget->setFrameSize(QSize(frame->width,frame->height));  //将fram 的宽和高设置进去
        //frame->data[0] 代表y，1是u ，2是v frame->linesize[0] 代表更新纹理
        ui->Videowidget->updateTextures(frame->data[0],frame->data[1],frame->data[2],frame->linesize[0],frame->linesize[1],frame->linesize[2]);
        m_frames.unLock();
    });

    updateShowSzie(size());
    m_server.start(m_serial,8080,1080,8000000);  //启动server服务
}

VideoForm::~VideoForm()
{
    m_server.stop();
    m_decoder.stopDecode();
    m_frames.deInit();
    delete ui;

}

// 当这个窗口的鼠标事件触发时，  转到 m_inputConvert.mouseEvent 进行实际处理 ，因为这个是显示窗口，所以在这个窗口的事件都需要进行处理
void VideoForm::mousePressEvent(QMouseEvent *event)
{
    m_inputConvert.mouseEvent(event,ui->Videowidget->frameSize(),ui->Videowidget->size());
}

void VideoForm::mouseReleaseEvent(QMouseEvent *event)
{
    m_inputConvert.mouseEvent(event,ui->Videowidget->frameSize(),ui->Videowidget->size());
}

void VideoForm::mouseMoveEvent(QMouseEvent *event)
{
    m_inputConvert.mouseEvent(event,ui->Videowidget->frameSize(),ui->Videowidget->size());
}

//滚轮滑动事件
void VideoForm::wheelEvent(QWheelEvent *event)
{
    m_inputConvert.wheelEvent(event,ui->Videowidget->frameSize(),ui->Videowidget->size());
}

//         key code 文本事件
void VideoForm::keyPressEvent(QKeyEvent *event)
{
    m_inputConvert.keyCodeEvent(event,ui->Videowidget->frameSize(),ui->Videowidget->size());
}

void VideoForm::keyReleaseEvent(QKeyEvent *event)
{
    m_inputConvert.keyCodeEvent(event,ui->Videowidget->frameSize(),ui->Videowidget->size());
}

//   自动切换横竖屏
void VideoForm::updateShowSzie(const QSize &newSize)
{
    if(m_frameSize != newSize){
        m_frameSize = newSize;
        bool vertical = newSize.height() > newSize.width(); //如果宽大于高，那就是横屏，那就自动更改显示尺寸
        if(vertical){
            resize(VIDEO_FROM_WIDTH,VIDEO_FROM_HEIGHT);
        }else {
            resize(VIDEO_FROM_HEIGHT,VIDEO_FROM_WIDTH);
        }
    }
}

//    磁吸工具条
void VideoForm::createQTollBar()
{


}
