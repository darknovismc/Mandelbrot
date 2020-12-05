#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QKeyEvent>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    void keyPressEvent(QKeyEvent *ev) override;
    ~MainWindow();
public slots:
    void onDrawInfo(float,double,double);
private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
