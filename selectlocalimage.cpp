#include "selectlocalimage.h"
#include "ui_selectlocalimage.h"

SelectLocalImage::SelectLocalImage(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SelectLocalImage)
{
    ui->setupUi(this);
    this->setWindowTitle("picture verification");
    initSelectIcon();
}

SelectLocalImage::~SelectLocalImage()
{
    delete ui;
}

//初始化界面图标
void SelectLocalImage::initSelectIcon()
{
    ui->btnHome->setIconSize(QSize(80,80));
    ui->btnHome->setIcon(QIcon(":/new/icon/D:/image/home.png"));

    ui->btnSelect->setIconSize(QSize(80,80));
    ui->btnSelect->setIcon(QIcon(":/new/icon/D:/image/openImg.png"));

    ui->btnDetect->setIconSize(QSize(80,80));
    ui->btnDetect->setIcon(QIcon(":/new/icon/D:/image/detect.png"));

    ui->lineEdit->setFocusPolicy(Qt::NoFocus);
}

//QImage格式转化成cv::Mat
cv::Mat QimageToMat(QImage image)
{
    cv::Mat mat;
    switch (image.format())
    {
        case QImage::Format_ARGB32:
        case QImage::Format_RGB32:
        case QImage::Format_ARGB32_Premultiplied:
            mat = cv::Mat(image.height(), image.width(), CV_8UC4, (void*)image.constBits(), image.bytesPerLine());
            break;
        case QImage::Format_RGB888:
            mat = cv::Mat(image.height(), image.width(), CV_8UC3, (void*)image.constBits(), image.bytesPerLine());
            cv::cvtColor(mat, mat, CV_BGR2RGB);
            break;
        case QImage::Format_Indexed8:
            mat = cv::Mat(image.height(), image.width(), CV_8UC1, (void*)image.constBits(), image.bytesPerLine());
            break;
     }
     return mat;

}

//打开文件夹按钮相应事件
void SelectLocalImage::on_btnSelect_clicked()
{
    //打开文件夹中的图片文件
    OpenFile = QFileDialog::getOpenFileName(this, "Please choose a person image", "", "Image Files(*.jpg *.png *.bmp *.pgm *.pbm);;All(*.*)");
    if( OpenFile != "" )
    {
        if( image.load(OpenFile) )
        {
            QImage newImg = image.scaled(ui->labelOrigin->width(), ui->labelOrigin->height());
            ui->labelOrigin->setPixmap(QPixmap::fromImage(newImg));

        }
    }

    //显示所示图片的路径
    QFileInfo OpenFileInfo;
    OpenFileInfo = QFileInfo(OpenFile);
    OpenFilePath = OpenFileInfo.filePath();
    ui->lineEdit->setText(OpenFilePath);
}


void SelectLocalImage::on_btnDetect_clicked()
{
    if(OpenFile == "")
    {
        QMessageBox::information(this, "alert", "Please select a face photo locally before identifying it!");
    }else{
        mat = QimageToMat(image);
        cvtColor(mat, mat, CV_BGR2RGB);
        MTCNNDetection(mat);
        QImage image2 = QImage((uchar*)(mat.data), mat.cols, mat.rows, QImage::Format_RGB888);
        QImage newImg2 = image2.scaled(ui->labelDetect->width(), ui->labelDetect->height());
        ui->labelDetect->setPixmap(QPixmap::fromImage(newImg2));

        std::string name = getName();
        QString name2 = QString(QString::fromLocal8Bit(name.c_str()));
        if(!name2.isEmpty()){
            ui->labelName_2->setText(name2);
        }else{
            ui->labelName_2->setText("unknown person");
        }

        std::string faceCo = getFaceConfidence();
        QString faceCo2 = QString(QString::fromLocal8Bit(faceCo.c_str()));
        ui->labelFaCo_2->setText(faceCo2);

        float detectionTime = getDetectionTime();
        QString detectionTime2 = QString::number(detectionTime, 'f', 0);
        ui->labelDeTi_2->setText(detectionTime2+"ms");
    }
}

void SelectLocalImage::on_btnHome_clicked()
{
    MainWindow *m = new MainWindow();
    m->show();
    this->hide();
}
