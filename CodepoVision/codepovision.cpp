#include "codepovision.h"
#include "ui_codepovision.h"

CodepoVision::CodepoVision(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CodepoVision)
{
    ui->setupUi(this);

    cv::Mat inputImage = cv::imread("/home/sam/Images/emily.jpg");
    cv::imshow("Display Image", inputImage);

}

CodepoVision::~CodepoVision()
{
    delete ui;
}
