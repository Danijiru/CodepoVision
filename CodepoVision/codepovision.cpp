#include "codepovision.h"
#include "ui_codepovision.h"
#include "webcamwindow.h"
#include <QLabel>

CodepoVision::CodepoVision(QWidget *parent) :
    QMainWindow(parent),
    _ui(new Ui::CodepoVision)
{
    _ui->setupUi(this);

    QWidget::showFullScreen();
    QWidget::showMaximized();

    connect(_ui->mdiArea, &QMdiArea::subWindowActivated, this, &CodepoVision::imageSelected);
}

CodepoVision::~CodepoVision()
{
    disconnect(_ui->mdiArea, &QMdiArea::subWindowActivated, this, &CodepoVision::imageSelected);
    delete _ui;
}

/**
 * @nom : on_actionImporter_une_image_triggered()
 * @role : importer une image
 * @entrees :
 * @sorties :
 */
void CodepoVision::on_actionImporter_une_image_triggered()
{
    importer_image();
}

/**
 * @nom : on_button_import_clicked()
 * @role : importer
 * @entrees :
 * @sorties :
 */
void CodepoVision::on_button_import_clicked()
{
    importer_image();
}

/**
 * @nom : importer_image
 * @role : importer une image dans la mdiArea
 * @entrees : QImage image
 * @sorties : void
 */
void CodepoVision::importer_image()
{
    QString newMedia = QFileDialog::getOpenFileName(this, "Open an image", QString(), "Images (*.png *.PNG *.jpeg *.JPEG *.jpg *.JPG *.xpm *.XPM *.pgm *.PGM *.tiff *.TIFF *.gif *.GIF *.bmp *.BMP *.ppm *.PPM *.pbm *.PBM *.pnm *.PNM)");
    if (!newMedia.isEmpty()) {
        QImage *newImg = new QImage(newMedia);
        afficher_image(newImg);
    }
}

/**
 * @nom : afficher_image
 * @role : afficher une image dans la mdiArea
 * @entrees : QImage image
 * @sorties : void
 */
void CodepoVision::afficher_image(QImage *img)
{
    QLabel *newProcessWindow = new QLabel(this);
    QPixmap pixmap = QPixmap::fromImage(*img);
    /*if (pixmap.size().height() > 400)   //si l'image importée est trop grande on la redimensionne
        pixmap = pixmap.scaled(400, 400, Qt::KeepAspectRatio);*/
    newProcessWindow->setPixmap(pixmap);
    _ui->mdiArea->addSubWindow(newProcessWindow);
    newProcessWindow->show();
    _ui->button_detectface->setEnabled(true);
    _ui->button_zonecaracteristique->setEnabled(true);
    _ui->button_detectFeatures->setEnabled(true);
}

/**
 * @nom : imageSelected
 * @role : Slot privé qui détecte les changements d'image sélectionné
 * @entrees : QMdiSubWindow *subWindow
 * @sorties : void
 */
void CodepoVision::imageSelected(QMdiSubWindow *subWindow)
{
    if (subWindow == 0) {   //si on a fermé la dernière image on désactive les options de détection faciale
        _ui->button_detectface->setEnabled(false);
        _ui->button_zonecaracteristique->setEnabled(false);
        _ui->button_detectFeatures->setEnabled(false);
    }
}

/**
 * @nom : on_button_webcam_clicked()
 * @role : actionne la webcam pour prendre des photos
 * @entrees :
 * @sorties :
 */
void CodepoVision::on_button_webcam_clicked()
{
    _ui->button_webcam->setEnabled(false);
    WebcamWindow webcamWindow(this);
    webcamWindow.setAttribute(Qt::WA_DeleteOnClose);
    webcamWindow.show();
}

/**
 * @nom : webcamClosed
 * @role : réactive le bouton d'ouverture de la webcam
 * @entrees :
 * @sorties :
 */
void CodepoVision::webcamClosed()
{
    _ui->button_webcam->setEnabled(true);
}


/**
 * @nom : QImage2IplImage
 * @role : convertir une QImage en IplImage
 * @entrees : pointeur QImage
 * @sorties : pointeur IlpImage
 */
IplImage* CodepoVision::QImage2IplImage(QImage *qimg)
{
    IplImage *imgHeader = cvCreateImageHeader( cvSize(qimg->width(), qimg->height()), IPL_DEPTH_8U, 4);
    uchar* newdata = (uchar*) malloc(sizeof(uchar) * qimg->byteCount());
    memcpy(newdata, qimg->bits(), qimg->byteCount());
    imgHeader->imageData = (char*) newdata;
    return imgHeader;
}

/**
 * @nom : IplImage2QImage
 * @role : convertir une IplImage en QImage
 * @entrees : pointeur IlpImage
 * @sorties : pointeur IlpImage
 */
QImage* CodepoVision::IplImage2QImage(IplImage *iplImg)
{
    int h = iplImg->height;
    int w = iplImg->width;
    int channels = iplImg->nChannels;
    QImage *qimg = new QImage(w, h, QImage::Format_ARGB32);
    char *data = iplImg->imageData;

    for (int y = 0; y < h; y++, data += iplImg->widthStep) {
        for (int x = 0; x < w; x++) {
            char r, g, b, a = 0;
            if (channels == 1) {
                r = data[x * channels];
                g = data[x * channels];
                b = data[x * channels];
            } else if (channels == 3 || channels == 4) {
                r = data[x * channels + 2];
                g = data[x * channels + 1];
                b = data[x * channels];
            }

            if (channels == 4) {
                a = data[x * channels + 3];
                qimg->setPixel(x, y, qRgba(r, g, b, a));
            } else {
                qimg->setPixel(x, y, qRgb(r, g, b));
            }
        }
    }
    return qimg;

}


/**
 * @nom : currentImage()
 * @role : renvoie l'image actuellement sélectionnée
 * @entrees :
 * @sorties : pointeur sur image de type IplImage
 */
IplImage* CodepoVision::currentImage()
{
    //récuperer l'image de la fenetre active
    QLabel *label = (QLabel *)_ui->mdiArea->currentSubWindow()->widget();
    QImage img = label->pixmap()->toImage();
    //convertir en IplImage
    IplImage  *imgIpl = QImage2IplImage(&img);
    return imgIpl;
}

/**
 * @nom : on_button_detectface_clicked()
 * @role : detecter une visage sur l'image de la fenetre active
 * @entrees :
 * @sorties :
 */
void CodepoVision::on_button_detectface_clicked()
{
    IplImage *imgIpl = currentImage();

    //detection de visage et affichage du rectangle englobant
    vector<CvRect> rects = detecter_zonesInteret(imgIpl, "../CodepoVision/cascade/haarcascade_frontalface_alt.xml");  //dessine le rectangle directement sur imgIpl
    if (rects.empty())
        QMessageBox::warning(this, "Erreur", "Aucun visage detecté");
    else
        afficher_image(IplImage2QImage(imgIpl));
}

/**
 * @nom : detecter_zonesInteret
 * @role : detecter les zones caractéristiques
 * @entrees : IplImage *img, QString strFilename
 * @sorties : vector<CvRect>
 */
vector<CvRect> CodepoVision::detecter_zonesInteret(IplImage *img, QString strFilename)
{
    //conversion QString vers char* du chemin vers le fichier de cascade
    QByteArray ba = strFilename.toLatin1();
    const char *filename = ba.data();

    CvHaarClassifierCascade *cascade = (CvHaarClassifierCascade*)cvLoad(filename, 0, 0, 0);
    CvMemStorage			*storage = cvCreateMemStorage(0);

    vector<CvRect> rects;
    CvRect *r;
    CvSeq *zones = cvHaarDetectObjects(img, cascade, storage, 1.08, 10, 0, cvSize(40, 40));

    for(int i = 0 ; i < zones->total; i++) {
        r = (CvRect*)cvGetSeqElem(zones, i);
        rects.push_back(*r);
        cvRectangle( img,
                     cvPoint(r->x, r->y),
                     cvPoint(r->x + r->width, r->y + r->height),
                     CV_RGB(255, 0, 0), 1, 8, 0 );
    }

    cvReleaseHaarClassifierCascade(&cascade);
    cvReleaseMemStorage(&storage);

    return rects;
}


/**
 * @nom : on_button_zonecaracteristique_clicked
 * @role : detecter des zones caractéristiques
 * @entrees :
 * @sorties :
 */
void CodepoVision::on_button_zonecaracteristique_clicked()
{
    IplImage *imgIpl = currentImage();

    //detection de visage
    vector<CvRect> rects = detecter_zonesInteret(imgIpl, "../CodepoVision/cascade/haarcascade_frontalface_alt.xml");
    if (rects.empty()) {
        QMessageBox::warning(this, "Erreur", "Aucun visage detecté");
    } else {
        CvRect r = rects.back();
        QImage img_face = IplImage2QImage(imgIpl)->copy(r.x,r.y,r.width,r.height);
        IplImage *imgIpl_face = QImage2IplImage(&img_face);

        //detection du nez
        detecter_zonesInteret( imgIpl_face, "../CodepoVision/cascade/haarcascade_Nariz.xml");
        //detection de la bouche
        detecter_zonesInteret( imgIpl_face, "../CodepoVision/cascade/haarcascade_Mouth.xml");
        //detection des yeux
        detecter_zonesInteret( imgIpl_face, "../CodepoVision/cascade/haarcascade_mcs_lefteye.xml");
        detecter_zonesInteret( imgIpl_face, "../CodepoVision/cascade/haarcascade_mcs_righteye.xml");

        afficher_image(IplImage2QImage(imgIpl_face));
    }
}


/**
 * @nom : on_button_detectFeatures_clicked()
 * @role : detecter les points d'interet
 * @entrees :
 * @sorties :
 */
void CodepoVision::on_button_detectFeatures_clicked()
{
    IplImage *imgIpl = currentImage();
    std::string modelFile = "../CodepoVision/data/grayall_asm.model";
    int verboseLevel = 0;

    StatModel::ASMModel asmModel;
    asmModel.loadFromFile(modelFile);

    searchAndFit(asmModel, imgIpl, verboseLevel);
}

/**
 * @nom : searchAndFit
 * @role : detecter et afficher les points d'interet
 * @entrees : StatModel::ASMModel & asmModel, IplImage* imageIpl, int verboseL
 * @sorties :
 */
//! Run OpenCV object detection and do ASM fitting on each detected object.
void CodepoVision::searchAndFit(StatModel::ASMModel & asmModel, IplImage* imageIpl, int verboseL)
{
    // Load image
    cv::Mat img = Mat(imageIpl->height, imageIpl->width, CV_8UC3);
    cv::Mat img4channels = cv::cvarrToMat(imageIpl);
    cv::cvtColor(img4channels, img, CV_BGRA2BGR);   //conversion ARGB (img4channels) vers RGB (img)

    //detection de visage
    vector<CvRect> rects = detecter_zonesInteret(imageIpl, "../CodepoVision/cascade/haarcascade_frontalface_alt.xml");
    if (rects.empty()) {
        QMessageBox::warning(this, "Erreur", "Aucun visage detecté");
    } else {
        CvRect r = rects.back();
        cv::Rect cvr = cv::Rect_<int>(r);
        vector< cv::Rect > faces;
        faces.push_back(cvr);

        // Fit to ASM!
        vector < StatModel::ASMFitResult > fitResult = asmModel.fitAll(img, faces, verboseL);
        cv::Mat ptsImg = asmModel.drawPoints(img, fitResult);

        // conversion cv::Mat vers QImage et affichage des points d'intérêt
        cv::Mat temp(ptsImg.cols, ptsImg.rows, ptsImg.type());
        cvtColor(ptsImg, temp, CV_BGR2RGB);
        QImage qimg = QImage(temp.data, temp.cols, temp.rows, temp.step, QImage::Format_RGB888);
        afficher_image(&qimg);
    }
}


/**
 * @nom : on_actionQuitter_triggered()
 * @role : quitter le programme
 * @entrees :
 * @sorties :
 */
void CodepoVision::on_actionQuitter_triggered()
{
    int reponse = QMessageBox::question(this, "Quitter", "Êtes-vous sûr de vouloir quitter le programme?", QMessageBox::Yes | QMessageBox::No);
    if (reponse == QMessageBox::Yes)
        qApp->quit();
}
