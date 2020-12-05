# Mandelbrot
This application calculates Mandelbrot set in any resoulution in the realtime. You can zoom in by left mouse button or zoom out by right mouse button.
You can also switch how calculations are performed by pressing: 1 - single threaded 2- multi threaded or 3 - GPU.

The application is written in C++ 11 in QtCreator version 5.7.0. You can compile it using MSVC or MiniGW.

canvasarea.cpp - class for a widget where Mandelbrot Set is calculated and displayed by software (CPU)
It handles some mouse events and draws by QPainter.drawImage(2)

glwidget.cpp - class for a widget where Mandelbrot Set is calculated and displayed by hardware (GPU)
It uses full screen quad fragment GLSL shader for drawing.

mainwindow.cpp - main class for the main Qt window.
