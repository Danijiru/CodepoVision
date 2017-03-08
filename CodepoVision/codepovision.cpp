#include "codepovision.h"
#include "ui_codepovision.h"

CodepoVision::CodepoVision(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CodepoVision)
{
    ui->setupUi(this);
}

CodepoVision::~CodepoVision()
{
    delete ui;
}
