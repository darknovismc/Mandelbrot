#include "glwidget.h"
#include <QMouseEvent>
#include <GL/glu.h>
#include <QMessageBox>

GLWidget::GLWidget(QWidget *parent): QGLWidget(QGLFormat(), parent),vertexShaderCode(
"attribute vec3 vertex; \
varying vec2 texCoord; \
void main() \
{ \
    texCoord.x = vertex.x*0.5+0.5; \
    texCoord.y = 0.5 -vertex.y*0.5; \
    gl_Position = vec4(vertex.xyz,1.0); \
}"
)
,fragmentShaderCode(
"varying vec2 texCoord; \
uniform vec2 sizeL; \
uniform vec2 sizeH; \
uniform vec2 centerL; \
uniform vec2 centerH; \
uniform vec2 spanL; \
uniform vec2 spanH; \
uniform vec2 deltaL; \
uniform vec2 deltaH; \
uniform float ratioL; \
uniform float ratioH; \
uniform int iterations; \
vec3 spectralColor(float l) \
{ \
    float t,r,g,b; \
    if ((l>=400.0)&&(l<410.0)) \
    { \
        t=(l-400.0)/(410.0-400.0); \
        r=    +(0.33*t)-(0.20*t*t); \
    } \
    else if ((l>=410.0)&&(l<475.0)) \
    { \
        t=(l-410.0)/(475.0-410.0); \
        r=0.14         -(0.13*t*t); \
    } \
    else if ((l>=545.0)&&(l<595.0)) \
    { \
        t=(l-545.0)/(595.0-545.0); \
        r=    +(1.98*t)-(     t*t); \
    } \
    else if ((l>=595.0)&&(l<650.0)) \
    { \
        t=(l-595.0)/(650.0-595.0); \
        r=0.98+(0.06*t)-(0.40*t*t); \
    } \
    else if ((l>=650.0)&&(l<700.0)) \
    { \
        t=(l-650.0)/(700.0-650.0); \
        r=0.65-(0.84*t)+(0.20*t*t); \
    } \
    else \
        r=0; \
    if ((l>=415.0)&&(l<475.0)) \
    { \
        t=(l-415.0)/(475.0-415.0); \
        g=             +(0.80*t*t); \
    } \
    else if ((l>=475.0)&&(l<590.0)) \
    { \
        t=(l-475.0)/(590.0-475.0); \
        g=0.8 +(0.76*t)-(0.80*t*t); \
    } \
    else if ((l>=585.0)&&(l<639.0)) \
    { \
        t=(l-585.0)/(639.0-585.0); \
        g=0.84-(0.84*t); \
    } \
    else \
        g=0; \
    if ((l>=400.0)&&(l<475.0)) \
    { \
        t=(l-400.0)/(475.0-400.0); \
        b=    +(2.20*t)-(1.50*t*t); \
    } \
    else if ((l>=475.0)&&(l<560.0)) \
    { \
        t=(l-475.0)/(560.0-475.0); \
        b=0.7 -(     t)+(0.30*t*t); \
    } \
    else \
        b=0; \
    return vec3(r,g,b); \
} \
void main() \
{ \
dvec2 size = (dvec2)sizeL+(dvec2)sizeH; \
dvec2 delta = (dvec2)deltaL+(dvec2)deltaH; \
dvec2 center = (dvec2)centerL+(dvec2)centerH; \
dvec2 span = (dvec2)spanL+(dvec2)spanH; \
double ratio = (dvec2)ratioL+(dvec2)ratioH; \
double x0 = double(texCoord.x)*size.x*delta.x+center.x-span.x*ratio; \
double y0 = double(texCoord.y)*size.y*delta.y+center.y-span.y*ratio; \
double re=0,im=0,tmp; \
int iter=0; \
while((re*re+im*im<4.0) && (iter<iterations)) \
{ \
    tmp=re; \
    re = re*re-im*im+x0; \
    im = 2*tmp*im+y0; \
    iter++; \
} \
float q=float(iter)/float(iterations); \
q=pow(q,0.5); \
vec3 color =spectralColor((400.0+(300.0*q))); \
gl_FragColor =vec4(color,1.0); \
}"
)
{
    timer.start();
    setMouseTracking(true);
    connect(&glTimer,SIGNAL(timeout()),this,SLOT(updateGL()));
    glTimer.start(16);
    buttonTimer.setInterval(20);
    connect(&buttonTimer,SIGNAL(timeout()),this,SLOT(changeSpan()));
}

void GLWidget::initializeGL()
{
    glClearColor(0.0f,0.0f,0.5f,1.0f);
    vertices << QVector3D(-1.0,-1.0,1.0) << QVector3D(-1.0,1.0,1.0) << QVector3D(1.0,-1.0,1.0) << QVector3D(-1.0,1.0,1.0) << QVector3D(1.0,1.0,1.0) << QVector3D(1.0,-1.0,1.0);
    if(!shader.addShaderFromSourceCode(QGLShader::Vertex,vertexShaderCode))
        QMessageBox::warning(this,"vertex shader error",shader.log());
    else if(!shader.addShaderFromSourceCode(QGLShader::Fragment,fragmentShaderCode))
        QMessageBox::warning(this,"fragment shader error",shader.log());
    else if(!shader.link())
        QMessageBox::warning(this,"shader linkage error",shader.log());
}

void GLWidget::resizeGL(int w, int h)
{
    _size.x=(double)w;
    _size.y=(double)h;
    glViewport(0,0,w,h);
}

void GLWidget::SplitDouble(double input, QVector2D* output)
{
    double delta;
    output->setY((float) input);
    delta = input - ((double) output->y());
    output->setX((float) delta);
}

void GLWidget::paintGL()
{
    quint64 frameTime = timer.nsecsElapsed();
    spanData.setDelta(_size.x,_size.y);

    glClear(GL_COLOR_BUFFER_BIT);
    shader.bind();
    QVector2D convertedX,convertedY;
    SplitDouble(_size.x,&convertedX);
    SplitDouble(_size.y,&convertedY);
    shader.setUniformValue("sizeL", QVector2D(convertedX.x(),convertedY.x()));
    shader.setUniformValue("sizeH", QVector2D(convertedX.y(),convertedY.y()));
    SplitDouble(spanData.center.x-1.0*spanData.span.x/_size.x,&convertedX);//-1.0*spanData.span.x/_size.x ?
    SplitDouble(spanData.center.y-1.0*spanData.span.y/_size.y,&convertedY);//-1.0*spanData.span.y/_size.y ?
    shader.setUniformValue("centerL", QVector2D(convertedX.x(),convertedY.x()));
    shader.setUniformValue("centerH", QVector2D(convertedX.y(),convertedY.y()));
    SplitDouble(spanData.span.x,&convertedX);
    SplitDouble(spanData.span.y,&convertedY);
    shader.setUniformValue("spanL", QVector2D(convertedX.x(),convertedY.x()));
    shader.setUniformValue("spanH", QVector2D(convertedX.y(),convertedY.y()));
    SplitDouble(spanData.delta.x,&convertedX);
    SplitDouble(spanData.delta.y,&convertedY);
    shader.setUniformValue("deltaL", QVector2D(convertedX.x(),convertedY.x()));
    shader.setUniformValue("deltaH", QVector2D(convertedX.y(),convertedY.y()));
    SplitDouble(spanData._ratio,&convertedX);
    shader.setUniformValue("ratioL", convertedX.x());
    shader.setUniformValue("ratioH", convertedX.y());
    shader.setUniformValue("iterations", spanData.iterations);
    shader.setAttributeArray("vertex", vertices.constData());
    shader.enableAttributeArray("vertex");
    glDrawArrays(GL_TRIANGLES, 0, vertices.size());
    shader.disableAttributeArray("vertex");
    shader.release();
    frameTime = timer.nsecsElapsed()-frameTime;
    emit drawInfo(frameTime/1000000.0f,mousePos.x,-mousePos.y);
}

void GLWidget::changeSpan()
{
    if(buttonType == BTN_LEFT_PRESSED)
        spanData.decreaseSpan();
    else if(buttonType == BTN_RIGHT_PRESSED)
        spanData.increaseSpan();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    mousePos = spanData.setMousePos((double)event->pos().x(),(double)event->pos().y());
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    spanData.center.x=mousePos.x;
    spanData.center.y=mousePos.y;
    if (event->buttons() & Qt::LeftButton)
    {
        buttonType = BTN_LEFT_PRESSED;
        buttonTimer.start();
    }
    else if (event->buttons() & Qt::RightButton)
    {
        buttonType = BTN_RIGHT_PRESSED;
        buttonTimer.start();
    }
}

void GLWidget::mouseReleaseEvent(QMouseEvent *event)
{
    buttonTimer.stop();
}
