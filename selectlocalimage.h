#ifndef SELECTLOCALIMAGE_H
#define SELECTLOCALIMAGE_H

#include <QDialog>
#include "mainwindow.h"
#include <QFileDialog>
#include <QFileInfo>
#include <QImage>
#include "qmessagebox.h"
#include "faceloginandregister.h"
namespace Ui {
class SelectLocalImage;
}

class SelectLocalImage : public QDialog
{
    Q_OBJECT

public:
    explicit SelectLocalImage(QWidget *parent = nullptr);
    ~SelectLocalImage();

private slots:
    void on_btnSelect_clicked();

    void on_btnDetect_clicked();

    void on_btnHome_clicked();

private:
    Ui::SelectLocalImage *ui;
    void initSelectIcon();
    QString OpenFile;
    QString OpenFilePath;
    QImage image;
    QImage Detect_img;
    cv::Mat mat;
};

#endif // SELECTLOCALIMAGE_H
