#include "webcamwindow.h"
#include "ui_webcamwindow.h"

/**
 * @brief WebcamWindow::WebcamWindow
 * @param parent
 */
WebcamWindow::WebcamWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WebcamWindow)
{
    ui->setupUi(this);
    this->parent = (CodepoVision *)parent;
    capture = cvCreateCameraCapture(CV_CAP_ANY);    // Ouvre le flux vidéo correspondant à la webcam
    // Vérifier si l'ouverture du flux est ok
    if (!capture) {
       std::cout << "Ouverture du flux vidéo impossible !\n" << std::endl;
       exit(1);
    }

    // Initialisation et lancement du timer rythmant l'affichage des images de la webcam
    changeImage = new QTimer(this);
    connect(changeImage, &QTimer::timeout, this, &WebcamWindow::displayImage);
    changeImage->start(10);
}

WebcamWindow::~WebcamWindow()
{
    delete ui;
    delete changeImage;
}

void WebcamWindow::displayImage()
{
   // On récupère une image
   IplImage *image = cvQueryFrame(capture);

   // On traite l'image -> conversion IplImage/QImage
   char *imageData = image->imageData;
   int width = image->width;
   int height = image->height;
   qimage = QImage(width, height, QImage::Format_RGB32);
   uint *pixel;
   for (int y = 0; y < height; y++, imageData += image->widthStep) {
       for (int x = 0; x < width; x++) {
           pixel = (uint*)qimage.scanLine(y) + x;
           *pixel = qRgb(imageData[x * image->nChannels+2], imageData[x * image->nChannels+1], imageData[x * image->nChannels]);
       }
   }

   // On affiche l'image
   ui->image->setPixmap(QPixmap::fromImage(qimage));

   // On attend 10ms
   changeImage->start(10);
}

void WebcamWindow::on_button_fermer_clicked()
{
    disconnect(changeImage, &QTimer::timeout, this, &WebcamWindow::displayImage);
    cvReleaseCapture(&capture);
    this->close();
    parent->activer_button_webcam();
}

void WebcamWindow::on_button_photo_clicked()
{
    parent->afficher_image(&qimage);
}
