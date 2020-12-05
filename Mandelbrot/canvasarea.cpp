#include "canvasarea.h"
#include <QMouseEvent>

SpanData spanData;

CanvasArea::CanvasArea(QWidget *parent) : QWidget(parent)
{
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
    timer.start();
    setMouseTracking(true);
    image = new QImage(this->width(),this->height(),QImage::Format_ARGB32);
    buttonTimer = new QTimer();
    buttonTimer->setInterval(20);
    connect(buttonTimer,SIGNAL(timeout()),this,SLOT(changeSpan()));
    auto functor =[this](int id) ->void {threadWork(id);};
    tAlive=true;
    for (int i = 0; i < 16; i++)
        threads.push_back(std::thread(functor, i));
}

CanvasArea::~CanvasArea()
{
    tAlive=false;
    nWorkDone=0;
    for(int i=0;i<16;i++)
        mutexes[i].unlock();
    while(nWorkDone<16){};
    for(int i =0;i<16;i++)
        threads[i].join();
    delete image;
    delete buttonTimer;
}

void CanvasArea::threadWork(int id)
{
    while(tAlive)
    {
        mutexes[id].lock();
        int start = id/16.0f*image->height();
        int end = (id+1)/16.0f*image->height();
        for(int x = 0;x<image->width();x++)
            for(int y = start;y<end;y++)
                calculateMandelbrot(x,y);
        nWorkDone++;
    }
}

void CanvasArea::changeSpan()
{
    if(buttonType == BTN_LEFT_PRESSED)
        spanData.decreaseSpan();
    else if(buttonType == BTN_RIGHT_PRESSED)
        spanData.increaseSpan();
}

void CanvasArea::mouseMoveEvent(QMouseEvent *event)
{
    mousePos = spanData.setMousePos((double)event->pos().x(),(double)event->pos().y());
}

void CanvasArea::mousePressEvent(QMouseEvent *event)
{
    spanData.center.x = mousePos.x;
    spanData.center.y = mousePos.y;
    if (event->buttons() & Qt::LeftButton)
    {
        buttonType = BTN_LEFT_PRESSED;
        buttonTimer->start();
    }
    else if (event->buttons() & Qt::RightButton)
    {
        buttonType = BTN_RIGHT_PRESSED;
        buttonTimer->start();
    }
}

void CanvasArea::mouseReleaseEvent(QMouseEvent *event)
{
    buttonTimer->stop();
}

void CanvasArea::paintEvent(QPaintEvent*  event)
{
    quint64 frameTime = timer.nsecsElapsed();
    if((width()!=image->width()) || (height()!=image->height()))
    {
        delete image;
        image = new QImage(width(),height(),QImage::Format_ARGB32);
    }
    QPainter painter(this);
    spanData.setDelta(width(),height());

    if(_bUseThreads)
    {
        nWorkDone=0;
        for(int i=0;i<16;i++)
            mutexes[i].unlock();
        while(nWorkDone<16){};
    }
    else
    {
        for(int x = 0;x<image->width();x++)
            for(int y = 0;y<image->height();y++)
                calculateMandelbrot(x,y);
    }

    painter.drawImage(QPointF(0,0),*image);
    update();
    frameTime = timer.nsecsElapsed()-frameTime;
    emit drawInfo(frameTime/1000000.0f,mousePos.x,-mousePos.y);
}

QRgb CanvasArea::spectralColor(float l)
{
    float t,r,g,b;
    if ((l>=400.0)&&(l<410.0))
    {
        t=(l-400.0)/(410.0-400.0);
        r=    +(0.33*t)-(0.20*t*t);
    }
    else if ((l>=410.0)&&(l<475.0))
    {
        t=(l-410.0)/(475.0-410.0);
        r=0.14         -(0.13*t*t);
    }
    else if ((l>=545.0)&&(l<595.0))
    {
        t=(l-545.0)/(595.0-545.0);
        r=    +(1.98*t)-(     t*t);
    }
    else if ((l>=595.0)&&(l<650.0))
    {
        t=(l-595.0)/(650.0-595.0);
        r=0.98+(0.06*t)-(0.40*t*t);
    }
    else if ((l>=650.0)&&(l<700.0))
    {
        t=(l-650.0)/(700.0-650.0);
        r=0.65-(0.84*t)+(0.20*t*t);
    }
    else
        r=0;
    if ((l>=415.0)&&(l<475.0))
    {
        t=(l-415.0)/(475.0-415.0);
        g=             +(0.80*t*t);
    }
    else if ((l>=475.0)&&(l<590.0))
    {
        t=(l-475.0)/(590.0-475.0);
        g=0.8 +(0.76*t)-(0.80*t*t);
    }
    else if ((l>=585.0)&&(l<639.0))
    {
        t=(l-585.0)/(639.0-585.0);
        g=0.84-(0.84*t);
    }
    else
        g=0;
    if ((l>=400.0)&&(l<475.0))
    {
        t=(l-400.0)/(475.0-400.0);
        b=    +(2.20*t)-(1.50*t*t);
    }
    else if ((l>=475.0)&&(l<560.0))
    {
        t=(l-475.0)/(560.0-475.0);
        b=0.7 -(     t)+(0.30*t*t);
    }
    else
        b=0;
    return qRgb(r*255,g*255,b*255);
}

void CanvasArea::calculateMandelbrot(int x,int y)
{
    double x0 = (double)x*spanData.delta.x+spanData.center.x-spanData.span.x*spanData._ratio;
    double y0 = (double)y*spanData.delta.y+spanData.center.y-spanData.span.y*spanData._ratio;
    double re=0,im=0,tmp;
    unsigned int iter=0;
    while(re*re+im*im<4 && iter<spanData.iterations)
    {
        tmp=re;
        re = re*re-im*im+x0;
        im = 2*tmp*im+y0;
        iter++;
    }
    float q=float(iter)/float(spanData.iterations);
    q=powf(q,0.5);
    QRgb color=spectralColor((400.0+(300.0*q)));
    image->setPixel(x,y,color);
}
