#ifndef MYTHREAD_H
#define MYTHREAD_H
#include <QObject>
#include <QThread>
#include <QDebug>
#include "livefacereco.hpp"

class FrameDeal : public QThread
{
    Q_OBJECT
public:
    explicit FrameDeal(QObject *parent = nullptr);

protected:
    void run() override;
signals:
    void SendFrame(cv::Mat sendframe);
public slots:
    void ReceiveFrame(cv::Mat receiveframe);
private:
    cv::Mat threadframe;
};


#endif // MYTHREAD_H
