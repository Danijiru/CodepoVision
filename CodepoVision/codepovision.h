#ifndef CODEPOVISION_H
#define CODEPOVISION_H

#include <QMainWindow>
#include <opencv2/opencv.hpp>

namespace Ui {
class CodepoVision;
}

class CodepoVision : public QMainWindow
{
    Q_OBJECT

public:
    explicit CodepoVision(QWidget *parent = 0);
    ~CodepoVision();

private:
    Ui::CodepoVision *ui;
};

#endif // CODEPOVISION_H
