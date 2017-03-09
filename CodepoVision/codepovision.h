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
    //void on_pushButton_clicked();

private slots:
    void on_actionImporter_une_image_triggered();

    void on_button_import_clicked();

    void on_actionQuitter_triggered();

private:
    Ui::CodepoVision *ui;
};

#endif // CODEPOVISION_H
