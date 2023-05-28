#include "mythread.h"

FrameDeal::FrameDeal(QObject *parent) : QThread(parent)
{
}

void FrameDeal::run()
{
    //qDebug() << "子线程：" << QThread::currentThread();
    MTCNNDetection(threadframe);
    emit SendFrame(threadframe);

}
void FrameDeal::ReceiveFrame(cv::Mat receiveframe)
{
    threadframe = receiveframe;
}
