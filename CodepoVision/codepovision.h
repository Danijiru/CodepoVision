#ifndef CODEPOVISION_H
#define CODEPOVISION_H

#include <QMainWindow>
#include <QFileDialog>

namespace Ui {
class CodepoVision;
}

class CodepoVision : public QMainWindow
{
    Q_OBJECT

public:
    explicit CodepoVision(QWidget *parent = 0);

    void afficher_image(QImage *img);

    void activer_button_webcam();

    ~CodepoVision();

private slots:
    void on_actionImporter_une_image_triggered();

    void on_button_import_clicked();

    void on_actionQuitter_triggered();

    void on_button_webcam_clicked();
    void webcamClosed();

private:
    Ui::CodepoVision *_ui;

    void importer_image();


};

#endif // CODEPOVISION_H
