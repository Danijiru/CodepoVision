#include "codepovision.h"
#include "ui_codepovision.h"

#include "webcamwindow.h"

CodepoVision::CodepoVision(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CodepoVision)
{
    ui->setupUi(this);

    //cv::Mat inputImage = cv::imread("/home/sam/Images/emily.jpg");
    //cv::imshow("Display Image", inputImage);

    QWidget::showFullScreen();
    QWidget::showMaximized();

    //connect(ui->actionImporter_une_image, &QAction::triggered, this, &CodepoVision::on_pushButton_clicked);

}

CodepoVision::~CodepoVision()
{
    delete ui;
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
    ui->mdiArea->addSubWindow(newProcessWindow);
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
    ui->button_webcam->setEnabled(true);
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
    ui->button_webcam->setEnabled(false);
    WebcamWindow *webcamWindow = new WebcamWindow(this);
    webcamWindow->show();
}
