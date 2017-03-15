#ifndef CODEPOVISION_H
#define CODEPOVISION_H

#include <QMainWindow>

namespace Ui {
class CodepoVision;
}

class CodepoVision : public QMainWindow
{
    Q_OBJECT

public:
    explicit CodepoVision(QWidget *parent = 0);
    ~CodepoVision();

private slots:
    void on_actionImporter_une_image_triggered();

    void on_button_import_clicked();

    void on_actionQuitter_triggered();

    void on_button_webcam_clicked();

private:
    Ui::CodepoVision *ui;
};

#endif // CODEPOVISION_H
