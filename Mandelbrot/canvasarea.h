#ifndef CANVASAREA_H
#define CANVASAREA_H

#include <QWidget>
#include <QPainter>
#include <QTimer>
#include <QElapsedTimer>
#include <thread>
#include <mutex>
#include "spandata.h"

class CanvasArea : public QWidget
{
    Q_OBJECT
public:
    explicit CanvasArea(QWidget *parent = 0);
    ~CanvasArea();
    void threadWork(int id);
    bool _bUseThreads=true;
signals:
    void drawInfo(float,double,double);
protected:
    void paintEvent(QPaintEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void calculateMandelbrot(int x,int y);
    QRgb spectralColor(float l);
private:
    BTN_TYPE buttonType;
    PointD mousePos = {0,0};
    QTimer* buttonTimer;
    QImage* image=nullptr;
    QElapsedTimer timer;
    std::vector<std::thread> threads;
    std::mutex mutexes[16];
    bool tAlive;
    std::atomic<int> nWorkDone;
public slots:
    void changeSpan();
};

#endif // CANVASAREA_H
