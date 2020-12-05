#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QWidget>
#include <QGLWidget>
#include <QTimer>
#include <QGLShaderProgram>
#include <QElapsedTimer>
#include "spandata.h"

class GLWidget : public QGLWidget
{
     Q_OBJECT
public:
    GLWidget(QWidget *parent=0);
    void paintGL();
    void initializeGL();
    void resizeGL(int w, int h);
private:
    BTN_TYPE buttonType;
    PointD mousePos = {0,0};
    PointD _size;
    const char* vertexShaderCode;
    const char* fragmentShaderCode;
    QElapsedTimer timer;
    QTimer glTimer,buttonTimer;
    QVector<QVector3D> vertices;
    QGLShaderProgram shader;
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void SplitDouble(double input, QVector2D* output);
private slots:
    void changeSpan();
signals:
    void drawInfo(float,double,double);
};

#endif // GLWIDGET_H
