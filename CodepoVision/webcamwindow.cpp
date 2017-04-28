#include "webcamwindow.h"
#include "ui_webcamwindow.h"

/**
 * @brief WebcamWindow::WebcamWindow
 * @param parent
 */
WebcamWindow::WebcamWindow(QWidget *parent) :
    QDialog(parent),
    _ui(new Ui::WebcamWindow)
{
    _ui->setupUi(this);
    _parent = (CodepoVision *)parent;
    _capture = cvCreateCameraCapture(CV_CAP_ANY);    // Ouvre le flux vidéo correspondant à la webcam
    // Vérifier si l'ouverture du flux est ok
    if (!_capture) {
       QMessageBox::critical(this, "Erreur", "Ouverture du flux vidéo impossible");
       this->close();
    }
    // Initialisation et lancement du timer rythmant l'affichage des images de la webcam
    _changeImage = new QTimer(this);
    connect(_changeImage, &QTimer::timeout, this, &WebcamWindow::displayImage);
    _changeImage->start(10);
}

WebcamWindow::~WebcamWindow()
{
    _parent->webcamClosed();
    disconnect(_changeImage, &QTimer::timeout, this, &WebcamWindow::displayImage);
    cvReleaseCapture(&_capture);
    delete _ui;
    delete _changeImage;
}

void WebcamWindow::displayImage()
{
   // On récupère une image
   IplImage *image = cvQueryFrame(_capture);

   // On traite l'image -> conversion IplImage/QImage
   char *imageData = image->imageData;
   int width = image->width;
   int height = image->height;
   _qimage = QImage(width, height, QImage::Format_RGB32);
   uint *pixel;
   for (int y = 0; y < height; y++, imageData += image->widthStep) {
       for (int x = 0; x < width; x++) {
           pixel = (uint*)_qimage.scanLine(y) + x;
           *pixel = qRgb(imageData[x * image->nChannels+2], imageData[x * image->nChannels+1], imageData[x * image->nChannels]);
       }
   }

   // On affiche l'image
   _ui->image->setPixmap(QPixmap::fromImage(_qimage));

   // On attend 10ms
   _changeImage->start(10);
}

void WebcamWindow::on_button_photo_clicked()
{
    _parent->afficher_image(&_qimage);
}

void WebcamWindow::on_button_fermer_clicked()
{
    this->close();
}
