#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("face recognition");
    this->resize(QSize(635,465));

    initScreen();
    connect(btnEntry, SIGNAL(clicked()), this, SLOT(on_btnEntry_clicked()));
    connect(btnImage, SIGNAL(clicked()), this, SLOT(on_btnImage_clicked()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

//UI界面初始化
void MainWindow::initScreen()
{
    btnEntry = new QPushButton(this);
    QIcon icon(":/new/icon/D:/image/enterUI.png");
    btnEntry->setIcon(icon);
    btnEntry->setIconSize(QSize(180,180));
    btnEntry->setGeometry(345,205,180,180);
    btnEntry->setFlat(true); // 去掉按钮边框
    btnEntry->setStyleSheet("QPushButton { background: #ECE0EA; }");


    btnImage = new QPushButton(this);
    QIcon icon2(":/new/icon/D:/image/btnImage.png");
    btnImage->setIcon(icon2);
    btnImage->setIconSize(QSize(200,200));
    btnImage->setGeometry(120,205,200,200);
    btnImage->setFlat(true); // 去掉按钮边框
    btnImage->setStyleSheet("QPushButton { background: #ECE0EA; }");

    ui->labelTitle->setStyleSheet("QLabel { background: #ECE0EA; }");

}


//进入照片检测界面
void MainWindow::on_btnImage_clicked()
{
    SelectLocalImage *s = new SelectLocalImage();
    s->show();
    this->hide();
}

//进入摄像头检测界面
void MainWindow::on_btnEntry_clicked()
{
    FaceLoginAndRegister *f = new FaceLoginAndRegister();
    f->show();
    this->hide();
}
