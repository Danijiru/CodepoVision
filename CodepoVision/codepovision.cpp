#include "codepovision.h"
#include "ui_codepovision.h"

#include "webcamwindow.h"

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
    //disconnect(webcamWindow, &WebcamWindow::closed, this, &CodepoVision::webcamClosed);
    disconnect(_ui->mdiArea, &QMdiArea::subWindowActivated, this, &CodepoVision::imageSelected);
    delete _ui;
}

/**
 * @nom : afficher_image
 * @role : afficher une image dans la mdiArea
 * @entrees : QImage image
 * @sorties :
 * @auteur : danny & sam
 */
void CodepoVision::afficher_image(QImage *img){
    QLabel *newProcessWindow = new QLabel(this);
    newProcessWindow->setPixmap(QPixmap::fromImage(*img));
    _ui->mdiArea->addSubWindow(newProcessWindow);
    newProcessWindow->show();
    _ui->button_detectface->setEnabled(true);
}

void CodepoVision::imageSelected(QMdiSubWindow *subWindow){
    if (subWindow == 0) //si on a fermé la dernière image
        _ui->button_detectface->setEnabled(false);  //on désactive l'option de détection de visage
}

/**
 * @nom : importer_image
 * @role : importer une image la mdiArea
 * @entrees : QImage image
 * @sorties :
 * @auteur : danny & sam
 */
void CodepoVision::importer_image(){
    QString newMedia = QFileDialog::getOpenFileName(this, tr("Open an image"), QString(), tr("Images (*.png *.PNG *.jpeg *.JPEG *.jpg *.JPG *.xpm *.XPM *.pgm *.PGM *.tiff *.TIFF *.gif *.GIF *.bmp *.BMP *.ppm *.PPM *.pbm *.PBM *.pnm *.PNM)"));
    if (!newMedia.isEmpty()) {
        QImage *newImg = new QImage;
        newImg->load(newMedia);
        afficher_image(newImg);
    }
}

/**
 * @nom : activer_button_webcam
 * @role : activer_button_webcam depuis l'interface webcam
 * @entrees :
 * @sorties :
 * @auteur : danny & sam
 */
void CodepoVision::activer_button_webcam(){
    _ui->button_webcam->setEnabled(true);
}

/**
 * @nom : detecter_visage
 * @role : detecter le visage et afficher un rectangle
 * @entrees :IplImage
 * @sorties :
 * @auteur : danny & sam
 */
void CodepoVision::detecter_visage(IplImage *img,CvHaarClassifierCascade *cascade, CvMemStorage *storage)
{
   int i;

   CvSeq *faces = cvHaarDetectObjects(img, cascade, storage, 1.1, 3, 0, cvSize(40, 40));

   for( i = 0 ; i < ( faces ? faces->total : 0 ) ; i++ ) {
       CvRect *r = ( CvRect* )cvGetSeqElem( faces, i );
       cvRectangle( img,
                    cvPoint( r->x, r->y ),
                    cvPoint( r->x + r->width, r->y + r->height ),
                    CV_RGB( 255, 0, 0 ), 1, 8, 0 );
   }

   afficher_image(IplImage2QImage(img));
   //cvShowImage( "Window-FT", img );
}

void CodepoVision::on_actionImporter_une_image_triggered()
{
    importer_image();
}

void CodepoVision::on_button_import_clicked()
{
    importer_image();
}

void CodepoVision::on_actionQuitter_triggered()
{
    qApp->quit();
}

void CodepoVision::on_button_webcam_clicked()
{
    _ui->button_webcam->setEnabled(false);
    WebcamWindow *webcamWindow = new WebcamWindow(this);
    webcamWindow->setAttribute(Qt::WA_DeleteOnClose);
    webcamWindow->show();
    connect(webcamWindow, &WebcamWindow::closed, this, &CodepoVision::webcamClosed);
}

void CodepoVision::webcamClosed()
{
    _ui->button_webcam->setEnabled(true);
}


/**
 * @nom :
 * @role :
 * @entrees :
 * @sorties :
 * @auteur :
 */
void CodepoVision::on_button_detectface_clicked()
{
    CvHaarClassifierCascade *cascade;
    CvMemStorage			*storage;

    QString strFilename = "../CodepoVision/cascade/haarcascade_frontalface_alt.xml";
    QByteArray ba = strFilename.toLatin1();
    const char *filename = ba.data();
    //récuperer l'image de la fenetre active
    QLabel *label = (QLabel *)_ui->mdiArea->currentSubWindow()->widget();
    QImage img = label->pixmap()->toImage();
    //convertir en IplImage
    IplImage  *imgIpl;
    imgIpl = QImage2IplImage(&img);
    cascade = ( CvHaarClassifierCascade* )cvLoad( filename, 0, 0, 0 );
    storage = cvCreateMemStorage( 0 );

    //detection du visage
    detecter_visage( imgIpl, cascade,storage );

    cvReleaseHaarClassifierCascade(&cascade);
    cvReleaseMemStorage( &storage );
}

/**
 * @nom : QImage2IplImage
 * @role : convertir une QImage en IplImage
 * @entrees : pointeur QImage
 * @sorties : pointeur IlpImage
 * @auteur :
 */
IplImage* CodepoVision::QImage2IplImage(QImage *qimg)
{
    IplImage *imgHeader = cvCreateImageHeader( cvSize(qimg->width(), qimg->height()), IPL_DEPTH_8U, 4);
    imgHeader->imageData = (char*) qimg->bits();

    uchar* newdata = (uchar*) malloc(sizeof(uchar) * qimg->byteCount());
    memcpy(newdata, qimg->bits(), qimg->byteCount());
    imgHeader->imageData = (char*) newdata;
    //cvClo
    return imgHeader;
}

/**
 * @nom :
 * @role :
 * @entrees :
 * @sorties :
 * @auteur :
 */
QImage*  CodepoVision::IplImage2QImage(IplImage *iplImg)
{
    int h = iplImg->height;
    int w = iplImg->width;
    int channels = iplImg->nChannels;
    QImage *qimg = new QImage(w, h, QImage::Format_ARGB32);
    char *data = iplImg->imageData;

    for (int y = 0; y < h; y++, data += iplImg->widthStep)
    {
        for (int x = 0; x < w; x++)
        {
            char r, g, b, a = 0;
            if (channels == 1)
            {
                r = data[x * channels];
                g = data[x * channels];
                b = data[x * channels];
            }
            else if (channels == 3 || channels == 4)
            {
                r = data[x * channels + 2];
                g = data[x * channels + 1];
                b = data[x * channels];
            }

            if (channels == 4)
            {
                a = data[x * channels + 3];
                qimg->setPixel(x, y, qRgba(r, g, b, a));
            }
            else
            {
                qimg->setPixel(x, y, qRgb(r, g, b));
            }
        }
    }
    return qimg;

}
