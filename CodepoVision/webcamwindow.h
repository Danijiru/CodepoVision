#ifndef WEBCAMWINDOW_H
#define WEBCAMWINDOW_H

#include <QDialog>
#include <opencv2/opencv.hpp>
#include <QTimer>
#include "codepovision.h"

namespace Ui {
class WebcamWindow;
}

class WebcamWindow : public QDialog
{
    Q_OBJECT

public:
    explicit WebcamWindow(QWidget *_parent = 0);
    ~WebcamWindow();

private slots:
    void displayImage();

    void on_button_fermer_clicked();

    void on_button_photo_clicked();

private:
    Ui::WebcamWindow *_ui;
    QTimer *_changeImage;
    QImage _qimage;
    CodepoVision *_parent;
    CvCapture *_capture; // Capture vidéo
};

#endif // WEBCAMWINDOW_H
