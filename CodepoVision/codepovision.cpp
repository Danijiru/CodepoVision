#include "codepovision.h"
#include "ui_codepovision.h"
#include <QFileDialog>
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

void CodepoVision::on_actionImporter_une_image_triggered()
{
    QString newMedia = QFileDialog::getOpenFileName();
    if (!newMedia.isEmpty()) {
        QImage *newImg = new QImage;
        newImg->load(newMedia);
        //ui->media->setPixmap(QPixmap::fromImage(*newImg));
        QLabel *newProcessWindow = new QLabel(this);
        newProcessWindow->setPixmap(QPixmap::fromImage(*newImg));
        ui->mdiArea->addSubWindow(newProcessWindow);
        newProcessWindow->show();
        //processWindowList.push_back(newProcessWindow);
        delete newImg;
    }
}

void CodepoVision::on_button_import_clicked()
{
    QString newMedia = QFileDialog::getOpenFileName();
    if (!newMedia.isEmpty()) {
        QImage *newImg = new QImage;
        newImg->load(newMedia);
        //ui->media->setPixmap(QPixmap::fromImage(*newImg));
        QLabel *newProcessWindow = new QLabel(this);
        newProcessWindow->setPixmap(QPixmap::fromImage(*newImg));
        ui->mdiArea->addSubWindow(newProcessWindow);
        newProcessWindow->show();
        //processWindowList.push_back(newProcessWindow);
        delete newImg;
    }
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
