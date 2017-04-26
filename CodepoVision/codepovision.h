#ifndef CODEPOVISION_H
#define CODEPOVISION_H

#include <QMainWindow>
#include <QFileDialog>
#include <opencv/highgui.h>
#include <opencv/cv.h>
#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <QMdiSubWindow>
#include "asmmodel.h"

namespace Ui {
class CodepoVision;
}

class CodepoVision : public QMainWindow
{
    Q_OBJECT

public:

    explicit CodepoVision(QWidget *parent = 0);

    void afficher_image(QImage *img);

    void activer_button_webcam();

    ~CodepoVision();

private slots:
    void on_actionImporter_une_image_triggered();

    void on_button_import_clicked();

    void on_actionQuitter_triggered();

    void imageSelected(QMdiSubWindow *subWindow);

    void on_button_webcam_clicked();

    void webcamClosed();

    void on_button_detectface_clicked();

    void on_button_detectFeatures_clicked();

    void on_button_zonecaracteristique_clicked();

private:
    Ui::CodepoVision *_ui;

    void importer_image();

    vector<CvRect> detecter_visage( IplImage *img,CvHaarClassifierCascade *cascade, CvMemStorage *storage );

    QImage*  IplImage2QImage(IplImage *iplImg);

    IplImage* QImage2IplImage(QImage *qimg);

    void searchAndFit(StatModel::ASMModel & asmModel, IplImage* imageIpl, int verboseL);

    void detecter_ptInteret(IplImage *img, QString strFilename);
};

#endif // CODEPOVISION_H
