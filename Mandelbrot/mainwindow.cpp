#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->canvasArea,SIGNAL(drawInfo(float,double,double)),this,SLOT(onDrawInfo(float,double,double)));
    connect(ui->glWidget,SIGNAL(drawInfo(float,double,double)),this,SLOT(onDrawInfo(float,double,double)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onDrawInfo(float time,double re,double im)
{
    ui->statusBar->clearMessage();
    ui->statusBar->showMessage(QString::number(re,'g',12)+"+i*" + QString::number(im,'g',12) +" time: " + QString::number(time)+" ms ");
}

void MainWindow::keyPressEvent(QKeyEvent *ev)
{
    if(ev->text()=="1")
    {
        ui->centralwidget->setCurrentIndex(0);
        ui->canvasArea->_bUseThreads=false;
    }
    else if(ev->text()=="2")
    {
        ui->centralwidget->setCurrentIndex(0);
        ui->canvasArea->_bUseThreads=true;
    }
    else if(ev->text()=="3")
        ui->centralwidget->setCurrentIndex(1);
}
