#include "faceloginandregister.h"
#include "ui_faceloginandregister.h"
#include "mainwindow.h"
FaceLoginAndRegister::FaceLoginAndRegister(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FaceLoginAndRegister)
{
    ui->setupUi(this);
    this->setWindowTitle("verification");

    qDebug() << "主线程：" << QThread::currentThread();

    framedeal = new FrameDeal;

    connect(this,SIGNAL(ReceiveFramefromThread(cv::Mat)),framedeal,SLOT(ReceiveFrame(cv::Mat)));

//    connect(framedeal, SIGNAL(&FrameDeal::SendFrame(cv::Mat sendframe)), this, [=](cv::Mat sendframe){
//        frame = sendframe;
//    });

    initPhotoIcon();
    initSignalSlots();
}

FaceLoginAndRegister::~FaceLoginAndRegister()
{
    if(capture.isOpened())
        capture.release();
    delete ui;
}

// ui界面初始化
void FaceLoginAndRegister::initPhotoIcon()
{
    ui->lineEditNewName->setVisible(false);
    ui->lineEditNewName->setPlaceholderText(" Enter the name of the registered user ");


    ui->buttonHome->setIconSize(QSize(80,80));
    ui->buttonHome->setIcon(QIcon(":/new/icon/D:/image/home.png"));

    btnCameraOpen = new QPushButton(this);
    btnCameraClose = new QPushButton(this);
    btnTakePhoto = new QPushButton(this);

    QIcon iconCO(":/new/icon/D:/image/cameraopen.png");
    QIcon iconCC(":/new/icon/D:/image/cameraclose.png");
    QIcon iconTF(":/new/icon/D:/image/takephoto.png");

    btnCameraOpen->setIcon(iconCO);
    btnCameraClose->setIcon(iconCC);
    btnTakePhoto->setIcon(iconTF);

    btnCameraOpen->setIconSize(QSize(80,80));
    btnCameraOpen->setGeometry(50,120,80,80);
    btnCameraClose->setIconSize(QSize(80,80));
    btnCameraClose->setGeometry(50,320,80,80);
    btnTakePhoto->setIconSize(QSize(80,80));
    btnTakePhoto->setGeometry(50,520,80,80);

    btnCameraOpen->setFlat(true); // 去掉按钮边框
    btnCameraClose->setFlat(true);
    btnTakePhoto->setFlat(true);

    ui->labelLogin->resize(75,75);
    ui->labelLogin->setStyleSheet(QString("border-image:url(:/new/icon/D:/image/login.png)"));
    ui->labelRegister->resize(75,75);
    ui->labelRegister->setStyleSheet(QString("border-image:url(:/new/icon/D:/image/register.png)"));

}

//信号与槽
void FaceLoginAndRegister::initSignalSlots()
{
    connect(btnCameraOpen, SIGNAL(clicked()), this, SLOT(on_btnCameraOpen_clicked()));
    connect(btnCameraClose, SIGNAL(clicked()), this, SLOT(on_btnCameraClose_clicked()));
    connect(btnTakePhoto, SIGNAL(clicked()), this, SLOT(on_btnTakePhoto_clicked()));
    connect(&theTimer, SIGNAL(timeout()), this, SLOT(updateImage()));
}

void FaceLoginAndRegister::infoPrint()
{
    bool status =ui->checkBox->isChecked();
    if(status == true)
    {
        std::string live = getLive();
        QString live2 = QString(QString::fromLocal8Bit(live.c_str()));
        ui->labelLive_3->setText(live2);

        float liCo = getLiveConfidence();
        QString liCo2 = QString::number(liCo, 'f', 6);
        ui->labelLiCo_3->setText(liCo2);

    }
    else if(status == false)
    {
        ui->labelLive_3->setText(" ");
        ui->labelLiCo_3->setText(" ");
    }

    if(rbtnRegisterIsChecked==false && capture.isOpened()){
        std::string name = getName();
        QString name2 = QString(QString::fromLocal8Bit(name.c_str()));
        if(!name2.isEmpty()){
            ui->labelName_3->setText(name2);
        }else{
            ui->labelName_3->setText("unknown person");
        }

        std::string faceCo = getFaceConfidence();
        QString faceCo2 = QString(QString::fromLocal8Bit(faceCo.c_str()));
        ui->labelFaCo_3->setText(faceCo2);

        float detectionTime = getDetectionTime();
        QString detectionTime2 = QString::number(detectionTime, 'f', 0);
        ui->labelDeTi_3->setText(detectionTime2+"ms");
    }else{
        ui->labelName_3->setText(" ");
        ui->labelFaCo_3->setText(" ");
        ui->labelDeTi_3->setText(" ");
    }
}

//更新每一帧
void FaceLoginAndRegister::updateImage()
{
    if(rbtnRegisterIsChecked==false){
        capture>>frame;
        cv::flip(frame, frame, 1);

        if(frame.data)
        {
            cvtColor(frame, frame, CV_BGR2RGB);//Qt中支持的是RGB图像, OpenCV中支持的是BGR
            this->update();  //发送刷新消息
        }

        //子线程
        emit ReceiveFramefromThread(frame);
        framedeal->start();
        Sleep(100);
        infoPrint();
    }else{
        capture>>frame;
        cv::flip(frame, frame, 1);


        if(frame.data)
        {
           cvtColor(frame, frame, CV_BGR2RGB);//Qt中支持的是RGB图像, OpenCV中支持的是BGR
           this->update();  //发送刷新消息
        }
    }


}

//识别图片显示在UI中
void FaceLoginAndRegister::paintEvent(QPaintEvent *e)
{

    QImage image1 = QImage((uchar*)(frame.data), frame.cols, frame.rows, QImage::Format_RGB888);

    ui->photoShow->setPixmap(QPixmap::fromImage(image1));
    ui->photoShow->resize(image1.size());
    ui->photoShow->show();

}



// 打开摄像头
void FaceLoginAndRegister::on_btnCameraOpen_clicked()
{

    if(rbtnLoginIsChecked==false && rbtnRegisterIsChecked==false)
        QMessageBox::information(this, "alert", "Please select face login or face registration first!");
    else{

        ui->radioButtonLogin_3->setEnabled(false);
        ui->radioButtonRegister_3->setEnabled(false);

        if(rbtnRegisterIsChecked){
            ui->lineEditNewName->setVisible(true);

        }
        if (capture.isOpened())
            return;
        // 200 usb
        if (capture.open(0)) {
            std::cout<<"cam is opened"<<std::endl;
            frame = cv::Mat::zeros(capture.get(cv::CAP_PROP_FRAME_HEIGHT), capture.get(cv::CAP_PROP_FRAME_WIDTH), CV_8UC3);
            theTimer.start(33);

        }else {
            QMessageBox::information(this, "alert", "Please check if the usb camera is connected!");
            capture.release();
        }
    }

}

// 关闭摄像头
void FaceLoginAndRegister::on_btnCameraClose_clicked()
{
    if (!capture.isOpened())
        QMessageBox::information(this, "alert", "Please click the camera button before proceeding!");

    ui->lineEditNewName->setVisible(false);

    ui->radioButtonLogin_3->setEnabled(true);
    ui->radioButtonRegister_3->setEnabled(true);

    capture.release();

}

//拍照存入文件夹
void FaceLoginAndRegister::on_btnTakePhoto_clicked()
{

    if (!capture.isOpened())
        QMessageBox::information(this, "alert", "Please click the camera button before taking a photo!");
    else{
        if (rbtnLoginIsChecked)
            QMessageBox::information(this, "alert", "Please select face registration before taking pictures!");
        else{

            cv::Mat frame1;
            //QPixmap mmp;

            capture >> frame1;

            if (frame1.empty())
            {
                std::cout << "frame1 is empty" << std::endl;
            }else{
                //保存图片
                QString new_name = ui->lineEditNewName->text();
                std::string new_name2 = new_name.toStdString();
                if(new_name.isEmpty())
                    QMessageBox::information(this, "alert", "Please enter the name of the registered face!");
                else{
                    FaceRegisterAligned(frame1, new_name2);
                    //sleep(1);
                    QMessageBox::information(this, "success", "Face registration is successful!");
                }
            }
         }

    }
}

//判断用户选择的是人脸登录还是人脸注册
void FaceLoginAndRegister::on_radioButtonLogin_3_clicked(bool checked)
{
    rbtnRegisterIsChecked=!checked;
    rbtnLoginIsChecked=checked;
}
//判断用户选择的是人脸登录还是人脸注册
void FaceLoginAndRegister::on_radioButtonRegister_3_clicked(bool checked)
{
    rbtnLoginIsChecked=!checked;
    rbtnRegisterIsChecked=checked;
}

// 返回主页
void FaceLoginAndRegister::on_buttonHome_clicked()
{
    MainWindow *m = new MainWindow();
    m->show();
    this->hide();
}
