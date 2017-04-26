#include "codepovision.h"
#include "ui_codepovision.h"

#include "webcamwindow.h"
//#include "modelfile.h"

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
    QPixmap pixmap = QPixmap::fromImage(*img);
    /*if (pixmap.size().height() > 400)   //si l'image importée est trop grande on la redimensionne
        pixmap = pixmap.scaled(400, 400, Qt::KeepAspectRatio);*/
    newProcessWindow->setPixmap(pixmap);
    _ui->mdiArea->addSubWindow(newProcessWindow);
    newProcessWindow->show();
    _ui->button_detectface->setEnabled(true);
    _ui->button_detectFeatures->setEnabled(true);
}

void CodepoVision::imageSelected(QMdiSubWindow *subWindow){
    if (subWindow == 0) {   //si on a fermé la dernière image on désactive les options de détection faciale
        _ui->button_detectface->setEnabled(false);
        _ui->button_detectFeatures->setEnabled(false);
    }
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
vector<CvRect> CodepoVision::detecter_visage(IplImage *img,CvHaarClassifierCascade *cascade, CvMemStorage *storage)
{
   int i;
   vector<CvRect> rects;
   CvRect *r;
   CvSeq *faces = cvHaarDetectObjects(img, cascade, storage, 1.1, 3, 0, cvSize(40, 40));

   for( i = 0 ; i < ( faces ? faces->total : 0 ) ; i++ ) {
       r = ( CvRect* )cvGetSeqElem( faces, i );
       rects.push_back(*r);
       cvRectangle( img,
                    cvPoint( r->x, r->y ),
                    cvPoint( r->x + r->width, r->y + r->height ),
                    CV_RGB( 255, 0, 0 ), 1, 8, 0 );
   }

   //afficher_image(IplImage2QImage(img));
   return rects;
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
    afficher_image(IplImage2QImage(imgIpl));

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

//! Run OpenCV object detection and do ASM fitting on each detected object.
void CodepoVision::searchAndFit(StatModel::ASMModel & asmModel, IplImage* imageIpl, int verboseL) {
    // Load image.

   // cv::Mat img = imread("/home/deprez/Bureau/505682.jpg");
    cv::Mat img = Mat(imageIpl,false);
    //cv::Mat img = cv::cvarrToMat(imageIpl);

    if (img.empty()) {
        std::cerr << "Load image  failed." << endl;
        exit(2);
    }

    // Face detection.
    // Pourrait etre remplacée par celle qu'on a développé?
    /*vector< cv::Rect > faces;
    objCascadeClassfifier.detectMultiScale(
        img, faces,
        1.2, 2, CV_HAAR_SCALE_IMAGE, Size(60, 60) );*/

    CvHaarClassifierCascade *cascade;
    CvMemStorage			*storage;

    QString strFilename = "../CodepoVision/cascade/haarcascade_frontalface_alt.xml";
    QByteArray ba = strFilename.toLatin1();
    const char *filename = ba.data();
    cascade = ( CvHaarClassifierCascade* )cvLoad( filename, 0, 0, 0 );
    storage = cvCreateMemStorage( 0 );

    //detection du visage
    vector<CvRect> rects = detecter_visage( imageIpl, cascade,storage );
    //afficher_image(IplImage2QImage(imageIpl));
    CvRect r = rects.back();
    cv::Rect cvr = cv::Rect_<int>(r);
    vector< cv::Rect > faces;
    faces.push_back(cvr);

    // Fit to ASM!
    vector < StatModel::ASMFitResult > fitResult = asmModel.fitAll(img, faces, verboseL);
    std::cout << fitResult.size() << endl;
    asmModel.showResult(img, fitResult);
    //cvWaitKey();
}

void CodepoVision::on_button_detectFeatures_clicked()
{
    //récuperer l'image de la fenetre active
    QLabel *label = (QLabel *)_ui->mdiArea->currentSubWindow()->widget();
    QImage img = label->pixmap()->toImage();
    //convertir en IplImage
    IplImage  *imgIpl;
    imgIpl = QImage2IplImage(&img);
    std::string modelFile = "../CodepoVision/data/grayall_asm.model";
    //std::string picPath = "/home/deprez/Bureau/505682.jpg";
    std::string faceCascadePath = "../CodepoVision/data/haarcascade_frontalface_alt.xml";
    int verboseLevel = 0;

    StatModel::ASMModel asmModel;
    asmModel.loadFromFile(modelFile);

    searchAndFit(asmModel, imgIpl, verboseLevel);
}

void CodepoVision::detecter_ptInteret(IplImage *img, QString strFilename)
{
    int i;

    CvHaarClassifierCascade *cascade;
    CvMemStorage			*storage;

    QByteArray ba = strFilename.toLatin1();
    const char *filename = ba.data();

    cascade = ( CvHaarClassifierCascade* )cvLoad( filename, 0, 0, 0 );
    storage = cvCreateMemStorage( 0 );

    CvSeq *faces = cvHaarDetectObjects(img, cascade, storage, 1.2, 5, CV_HAAR_DO_CANNY_PRUNING, cvSize(40, 40));

    for( i = 0 ; i < ( faces ? faces->total : 0 ) ; i++ ) {
        CvRect *r = ( CvRect* )cvGetSeqElem( faces, i );
        cvRectangle( img,
                     cvPoint( r->x, r->y ),
                     cvPoint( r->x + r->width, r->y + r->height ),
                     CV_RGB( 255, 0, 0 ), 1, 8, 0 );
        //cvCircle(img,  cvPoint( r->x, r->y ),2, CV_RGB( 255, 0, 0 ), 1, 8, 0 );
    }

    //afficher_image(IplImage2QImage(img));

    cvReleaseHaarClassifierCascade(&cascade);
    cvReleaseMemStorage( &storage );
}

void CodepoVision::on_button_zonecaracteristique_clicked()
{
    //récuperer l'image de la fenetre active
        QLabel *label = (QLabel *)_ui->mdiArea->currentSubWindow()->widget();
        QImage img = label->pixmap()->toImage();

        //convertir en IplImage
        IplImage  *imgIpl;
        imgIpl = QImage2IplImage(&img);

        CvHaarClassifierCascade *cascade;
        CvMemStorage			*storage;

        QString strFilename = "../CodepoVision/cascade/haarcascade_frontalface_alt.xml";
        QByteArray ba = strFilename.toLatin1();
        const char *filename = ba.data();
        cascade = ( CvHaarClassifierCascade* )cvLoad( filename, 0, 0, 0 );
        storage = cvCreateMemStorage( 0 );

        //detection du visage
        vector<CvRect> rects = detecter_visage( imgIpl, cascade,storage );
        CvRect r = rects.back();

        QImage img_face = IplImage2QImage(imgIpl)->copy(r.x,r.y,r.width,r.height);

        IplImage  *imgIpl_face;
        imgIpl_face = QImage2IplImage(&img_face);


        //detection du nez
        detecter_ptInteret( imgIpl_face, "../CodepoVision/cascade/haarcascade_Nariz.xml");
        //detection de la bouche
        detecter_ptInteret( imgIpl_face, "../CodepoVision/cascade/haarcascade_Mouth.xml");
        //detection des yeux
        detecter_ptInteret( imgIpl_face, "../CodepoVision/cascade/haarcascade_mcs_righteye.xml");

        afficher_image(IplImage2QImage(imgIpl_face));
}
