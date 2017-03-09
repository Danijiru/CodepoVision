#include "codepovision.h"
#include "ui_codepovision.h"
#include <QFileDialog>

CodepoVision::CodepoVision(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CodepoVision)
{
    ui->setupUi(this);

    //cv::Mat inputImage = cv::imread("/home/sam/Images/emily.jpg");
    //cv::imshow("Display Image", inputImage);

    QWidget::showFullScreen();
    QWidget::showMaximized();

    connect(ui->actionImporter_une_image, &QAction::triggered, this, &CodepoVision::on_pushButton_clicked);

}

CodepoVision::~CodepoVision()
{
    delete ui;
}

void CodepoVision::on_pushButton_clicked()
{
    //ProcessWindow *newProcessWindow = new ProcessWindow;
    QString newDocument = QFileDialog::getOpenFileName();
    //if (newDocument.isEmpty())	delete newProcessWindow;
    //else
    //{
    QImage *newImg = new QImage;
    newImg->load(newDocument);
    //newProcessWindow->getUi().document->setPixmap(QPixmap::fromImage(*newImg));
    ui->media->setPixmap(QPixmap::fromImage(*newImg));
    //ui.mdiArea->addSubWindow(newProcessWindow);
    //newProcessWindow->show();
    //processWindowList.push_back(newProcessWindow);
    delete newImg;
    //}
}
