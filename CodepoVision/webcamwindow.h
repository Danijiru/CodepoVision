#ifndef WEBCAMWINDOW_H
#define WEBCAMWINDOW_H

#include <QDialog>
#include <opencv2/opencv.hpp>
#include <QTimer>

namespace Ui {
class WebcamWindow;
}

class WebcamWindow : public QDialog
{
    Q_OBJECT

public:
    explicit WebcamWindow(QWidget *parent = 0);
    ~WebcamWindow();

private slots:
    void displayImage();

    void on_button_fermer_clicked();

private:
    Ui::WebcamWindow *ui;
    QTimer *changeImage;
    CvCapture *capture; // Capture vidéo
};

#endif // WEBCAMWINDOW_H
