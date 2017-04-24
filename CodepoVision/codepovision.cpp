#include "codepovision.h"
#include "ui_codepovision.h"

#include "webcamwindow.h"
#include <QMdiSubWindow>

CodepoVision::CodepoVision(QWidget *parent) :
    QMainWindow(parent),
    _ui(new Ui::CodepoVision)
{
    _ui->setupUi(this);

    QWidget::showFullScreen();
    QWidget::showMaximized();

}

CodepoVision::~CodepoVision()
{
    //disconnect(webcamWindow, &WebcamWindow::closed, this, &CodepoVision::webcamClosed);
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
    //limage = *img;
    newProcessWindow->setPixmap(QPixmap::fromImage(*img));
    _ui->mdiArea->addSubWindow(newProcessWindow);
    newProcessWindow->show();
    //delete img;
}

/**
 * @nom : importer_image
 * @role : importer une image la mdiArea
 * @entrees : QImage image
 * @sorties :
 * @auteur : danny & sam
 */
void CodepoVision::importer_image(){
    QString newMedia = QFileDialog::getOpenFileName();
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
    std::cout << "fuck you 1" << std::endl;
    CvHaarClassifierCascade *cascade;
    CvMemStorage			*storage;

    QString strFilename = "/home/deprez/Documents/UEProjet/CodepoVision/CodepoVision/cascade/haarcascade_frontalface_alt.xml";
    QByteArray ba = strFilename.toLatin1();
     const char *filename = ba.data();
    std::cout << "fuck you 2" << std::endl;
      //récuperer l'image de la fenetre active
      QLabel *label = (QLabel *)_ui->mdiArea->currentSubWindow()->widget();
      std::cout << "fuck you 2.1" << std::endl;
      QImage img = label->pixmap()->toImage();
      //convertir en IplImage
      std::cout << "fuck you 2.4" << std::endl;
      IplImage  *imgIpl;
      std::cout << "fuck you 2.5" << std::endl;
      imgIpl = QImage2IplImage(&img);
    std::cout << "fuck you 3" << std::endl;
      cascade = ( CvHaarClassifierCascade* )cvLoad( filename, 0, 0, 0 );
      std::cout << "fuck you4" << std::endl;
      storage = cvCreateMemStorage( 0 );
      //capture = cvCreateCameraCapture(CV_CAP_ANY);
      std::cout << "fuck you5" << std::endl;
      //cvNamedWindow( "Window-FT", 1 );
      std::cout << "fuck you6" << std::endl;
      detecter_visage( imgIpl, cascade,storage );
      std::cout << "fuck you7" << std::endl;

      //cvDestroyWindow( "Window-FT" );
      cvReleaseHaarClassifierCascade(&cascade);
      cvReleaseMemStorage( &storage );
}

/**
 * @nom :
 * @role :
 * @entrees :
 * @sorties :
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
