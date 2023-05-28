#ifndef FACELOGINANDREGISTER_H
#define FACELOGINANDREGISTER_H

#include <QDialog>
#include "qpushbutton.h"
#include "qradiobutton.h"
#include <opencv2/opencv.hpp>
#include <opencv2/videoio/videoio.hpp>
#include <opencv2/imgproc/types_c.h>
#include <QPaintEvent>
#include <QTimer>
#include <QPainter>
#include <QPixmap>
#include <QLabel>
#include <QImage>
#include <QTimer>
#include <QString>
#include "mtcnn.h"
#include "qbuttongroup.h"
#include "qmessagebox.h"
#include "mythread.h"
namespace Ui {
class FaceLoginAndRegister;
}

class FaceLoginAndRegister : public QDialog
{
    Q_OBJECT

public:
    explicit FaceLoginAndRegister(QWidget *parent = nullptr);
    ~FaceLoginAndRegister();
    cv::VideoCapture capture;
    bool isChecked();

public slots:
    void updateImage();

protected:
    void paintEvent(QPaintEvent *e);

private slots:
    void on_btnCameraOpen_clicked();
    void on_btnCameraClose_clicked();
    void on_btnTakePhoto_clicked();
    void on_buttonHome_clicked();

    void on_radioButtonLogin_3_clicked(bool checked);

    void on_radioButtonRegister_3_clicked(bool checked);

private:
    Ui::FaceLoginAndRegister *ui;

    QPushButton *btnCameraOpen;
    QPushButton *btnCameraClose;
    QPushButton *btnTakePhoto;
    bool rbtnLoginIsChecked;
    bool rbtnRegisterIsChecked;
    QTimer theTimer;
    cv::Mat frame;
    int index = 0;
    void initPhotoIcon();
    void initSignalSlots();
    void faceRegister();
    void infoPrint();
    FrameDeal *framedeal;

signals:
    void ReceiveFramefromThread(cv::Mat frame);
};

#endif // FACELOGINANDREGISTER_H
