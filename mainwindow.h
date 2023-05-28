#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "QPushButton"
#include "faceloginandregister.h"
#include "selectlocalimage.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btnEntry_clicked();
    void on_btnImage_clicked();
private:
    Ui::MainWindow *ui;
    QPushButton *btnEntry;
    QPushButton *btnImage;
    void initScreen();
};
#endif // MAINWINDOW_H
