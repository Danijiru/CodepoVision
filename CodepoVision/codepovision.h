#ifndef CODEPOVISION_H
#define CODEPOVISION_H

#include <QMainWindow>
#include <QFileDialog>
#include <opencv/highgui.h>
#include <opencv/cv.h>
#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <QMdiSubWindow>
#include <QMessageBox>
#include "asmmodel.h"

namespace Ui {
class CodepoVision;
}

class CodepoVision : public QMainWindow
{
    Q_OBJECT

public:

    explicit CodepoVision(QWidget *parent = 0);
    ~CodepoVision();

    void afficher_image(QImage *img);

    QImage* IplImage2QImage(IplImage *iplImg);

    void webcamClosed();

private slots:
    void on_actionImporter_une_image_triggered();

    void on_button_import_clicked();

    void on_actionQuitter_triggered();

    void imageSelected(QMdiSubWindow *subWindow);

    void on_button_webcam_clicked();

    void on_button_detectface_clicked();

    void on_button_detectFeatures_clicked();

    void on_button_zonecaracteristique_clicked();

private:
    Ui::CodepoVision *_ui;

    void importer_image();

    IplImage* QImage2IplImage(QImage *qimg);

    IplImage* currentImage();

    vector<CvRect> detecter_zonesInteret(IplImage *img, QString strFilename);

    void searchAndFit(StatModel::ASMModel & asmModel, IplImage* imageIpl, int verboseL);
};

#endif // CODEPOVISION_H
