#ifndef SPANDATA_H
#define SPANDATA_H


enum BTN_TYPE {BTN_LEFT_PRESSED,BTN_RIGHT_PRESSED};

struct PointD
{
    double x,y;
};

struct SpanData
{
    void increaseSpan()
    {
        span.x*=1.05;
        span.y*=1.05;
    }
    void decreaseSpan()
    {
        span.x*=0.95;
        span.y*=0.95;
    }
    PointD setMousePos(double posX,double posY)
    {
        PointD mousePos;
        mousePos.x = posX*delta.x+center.x-span.x*_ratio;
        mousePos.y = posY*delta.y+center.y-span.y*_ratio;
        return mousePos;
    }
    void setDelta(double width,double height)
    {
        _ratio = width/height;
        if(_ratio>1.0)
            _ratio = 1.0/_ratio;
        delta.x = 2.0*span.x/width*_ratio;
        delta.y = 2.0*span.y/height*_ratio;
    }
    PointD center = {0,0};
    PointD span = {1.0,1.0};
    PointD delta = {0,0};
    const unsigned int iterations = 512;
    double _ratio;
};

extern SpanData spanData;

#endif // SPANDATA_H
