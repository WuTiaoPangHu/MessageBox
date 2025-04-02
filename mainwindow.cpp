#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "qmesboxwidget.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QMesBoxWidget::setMesBox(LightTheme,1,2,5);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    QMesBoxWidget::MesBox(ClassicTheme,"TEST","This is a test message!",1,1,3);
}


void MainWindow::on_pushButton_2_clicked()
{

    QMesBoxWidget::MesBox("提示","这是第二种提示这是第二种提示这是第二种提示这是第二种提示这是第二种提示这是第二种提示这是第二种提示这是第二种提示这是第二种提示");
}

