#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <QFileDialog>
#include <QDebug>
#include <QtGui>
#include <QPixmap>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QTextBrowser>
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <stdint.h>
#include <math.h>
#include <QFileInfo>

cv::Mat mat1;
cv::Mat mat2;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QPushButton* ui_findButton = findChild<QPushButton*>("adauga1");
    QPushButton* ui_findButton2 = findChild<QPushButton*>("adauga2");
    QPushButton* ui_findButton3 = findChild<QPushButton*>("print");
    connect(ui_findButton, &QPushButton::clicked, this, &MainWindow::openFileDialog);
    connect(ui_findButton2, &QPushButton::clicked, this, &MainWindow::openFileDialog2);
    connect(ui_findButton3, &QPushButton::clicked, this, &MainWindow::print);

}

void MainWindow::openFileDialog()
{
    QFileDialog dialog;
    QPainter painter;
    dialog.show();
    QStringList fileNames;

    if(dialog.exec())
    {
        fileNames = dialog.selectedFiles();
    }

    QGraphicsView* ui_imageWidget = findChild<QGraphicsView*>("image1");
    QPixmap pic = QPixmap(fileNames.constFirst()).scaledToHeight(ui_imageWidget->height());

    mat1 = cv::imread(fileNames.join("").toStdString(), cv::IMREAD_GRAYSCALE);

    QGraphicsPixmapItem* item = new QGraphicsPixmapItem(pic);
    QGraphicsScene* scene = new QGraphicsScene();
    scene->addItem(item);
    ui_imageWidget->setScene(scene);

}

void MainWindow::openFileDialog2()
{
    QFileDialog dialog;
    QPainter painter;
    dialog.show();
    QStringList fileNames;
    if(dialog.exec())
    {
        fileNames = dialog.selectedFiles();
    }
    QGraphicsView* ui_imageWidget = findChild<QGraphicsView*>("image2");
    QPixmap pic = QPixmap(fileNames.constFirst()).scaledToHeight(ui_imageWidget->height());

    mat2 = cv::imread(fileNames.join("").toStdString(), cv::IMREAD_GRAYSCALE);

    QGraphicsPixmapItem* item = new QGraphicsPixmapItem(pic);
    QGraphicsScene* scene = new QGraphicsScene();
    scene->addItem(item);
    ui_imageWidget->setScene(scene);

}

class Pacient{
public:
    Pacient(){}
    QString nume;
    float matrice_zone(cv::Mat img, int inceput_linii, int inceput_coloane, int sfarsit_linii, int sfarsit_coloane);
    void construire_matrice(cv::Mat img, float matrice[12][9]);
    float calculare_medie(float matrice[12][9]);
    QString comparatie(cv::Mat img1, float imag1[12][9], cv::Mat img2, float  imag2[12][9]);
};

float Pacient::matrice_zone(cv::Mat img, int inceput_linii, int inceput_coloane, int sfarsit_linii, int sfarsit_coloane) {
    float intensitate_pixeli = 0;
    float nr_pixeli = 0;

    for (int i = inceput_linii; i < sfarsit_linii; i++) {
        for (int j = inceput_coloane; j < sfarsit_coloane; j++) {
            if ((int)img.at<uint8_t>(i, j) >= 50) {
                intensitate_pixeli += (int)img.at<uint8_t>(i, j);
                nr_pixeli++;
            }
        }
    }

    return (intensitate_pixeli / ((sfarsit_linii - inceput_linii) * (sfarsit_coloane - inceput_coloane))) * 100;

}

void Pacient::construire_matrice(cv::Mat img, float matrice[12][9]) {

    for (int i = 0; i < 12; i++) {
        for (int j = 0; j < 9; j++) {
            matrice[i][j] = matrice_zone(img, (img.rows * i) / 12, (img.cols * j) / 9, (img.rows * (i + 1)) / 12, (img.cols * (j + 1)) / 9);
        }
    }
}

float Pacient::calculare_medie(float matrice[12][9]) {
    double zone_interes[12][9] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 1, 1, 0.8, 0, 0},
    {0, 0, 1, 1, 1, 1, 1, 0.2, 0},
    {0, 0.3, 1, 1, 1, 1, 1, 0.4, 0},
    {0, 0.3, 1, 1, 1, 1, 1, 1, 0},
    {0, 0.3, 1, 1, 1, 1, 1, 1, 0.2},
    {0, 0.3, 1, 1, 1, 1, 1, 1, 1},
    {0, 0.3, 1, 1, 1, 1, 1, 1, 1},
    {0, 0, 0, 1, 1, 1, 1, 1, 1},
    {0, 0, 0, 1, 1, 1, 1, 1, 0.4},
    {0, 0, 0, 0, 0.5, 0.4, 0.3, 0.2, 0}
    };

    float medie = 0;

    for (int i = 0; i < 12; i++) {
        for (int j = 0; j < 9; j++) {
            medie += matrice[i][j] * zone_interes[i][j];
        }
    }

    return medie / 108;
}

QString Pacient::comparatie(cv::Mat img1, float imag1[12][9], cv::Mat img2, float  imag2[12][9]) {

    QString mesaj;

    double zone_interes[12][9] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 1, 1, 0.8, 0, 0},
    {0, 0, 1, 1, 1, 1, 1, 0.2, 0},
    {0, 0.3, 1, 1, 1, 1, 1, 0.4, 0},
    {0, 0.3, 1, 1, 1, 1, 1, 1, 0},
    {0, 0.3, 1, 1, 1, 1, 1, 1, 0.2},
    {0, 0.3, 1, 1, 1, 1, 1, 1, 1},
    {0, 0.3, 1, 1, 1, 1, 1, 1, 1},
    {0, 0, 0, 1, 1, 1, 1, 1, 1},
    {0, 0, 0, 1, 1, 1, 1, 1, 0.4},
    {0, 0, 0, 0, 0.5, 0.4, 0.3, 0.2, 0}
    };

    int vector_imag1[108], vector_imag2[108];

    int k = 0;

    for (int i = 0; i < 12; i++) {
        for (int j = 0; j < 9; j++) {
            vector_imag1[k] = imag1[i][j] * zone_interes[i][j];
            vector_imag2[k] = imag2[i][j] * zone_interes[i][j];
            k++;
        }
    }

    //Distanta hamming si levenstein

    int hamming = 0;
    int levenstein = 0;
    float euclid = 0;

    for (int i = 0; i < 108; i++) {
        if (vector_imag1[i] != vector_imag2[i]) {
            hamming++;
            levenstein++;
        }
        euclid += pow(vector_imag1[i] - vector_imag2[i], 2);
    }

    euclid = sqrt(euclid) / 10;

    float procent = (abs(2764.35 - euclid) / 2764.35) * 100;

    if (calculare_medie(imag1) > calculare_medie(imag2)) {
        mesaj = "Tumoarea s-a retras cu " + QString::number(procent) + "%.\n";
    }
    else if (calculare_medie(imag1) < calculare_medie(imag2)) {
        mesaj = "Tumoarea s-a extins cu " + QString::number(procent) + "%.\n";
        int maximi1 = 0, maximj1 = 0, maximi2 = 0, maximj2 = 0, maxim1 = 0, maxim2 = 0;
        for (int i = 0; i < 12; i++) {
            for (int j = 0; j < 9; j++) {
                if (imag1[i][j] * zone_interes[i][j] > maxim1) {
                    maxim1 = imag1[i][j] * zone_interes[i][j];
                    maximi1 = i;
                    maximj1 = j;
                }

                if (imag2[i][j] * zone_interes[i][j] > maxim2) {
                    maxim2 = imag2[i][j] * zone_interes[i][j];
                    maximi2 = i;
                    maximj2 = j;
                }
            }
        }

        if (maximi1 > maximi2 && maximj1 > maximj2) {
            if ((maximi1 - maximi2) > 1 && (maximj1 - maximj2) > 1) {
          mesaj += "Tumoarea s-a deplasat in sus si stanga\n";
            }
            else if ((maximi1 - maximi2) > 1) {
            mesaj += "Tumoarea s-a deplatat in sus\n";
            }
            else if ((maximj1 - maximj2) > 1) {
             mesaj += "Tumoarea s-a deplasat spre stanga.\n";
            }
        }

        if (maximi1 > maximi2 && maximj1 < maximj2) {
            if ((maximi1 - maximi2) > 1 && (maximj2 - maximj1) > 1) {
              mesaj+= "Tumoarea s-a deplasat in sus si dreapta\n";
            }
            else if ((maximi1 - maximi2) > 1) {
                mesaj+= "Tumoarea s-a deplatat in sus\n";
            }
            else if ((maximj2 - maximj1) > 1) {
                mesaj+= "Tumoarea s-a deplasat spre dreapta.\n";
            }
        }

        if (maximi1 < maximi2 && maximj1 < maximj2) {
            if ((maximi2 - maximi1) > 1 && (maximj2 - maximj1) > 1) {
              mesaj += "Tumoarea s-a deplasat in jos si dreapta\n";
            }
            else if ((maximi2 - maximi1) > 1) {
                mesaj += "Tumoarea s-a deplatat in jos\n";
            }
            else if ((maximj2 - maximj1) > 1) {
              mesaj += "Tumoarea s-a deplasat spre dreapta.\n";
            }
        }

        if (maximi1 < maximi2 && maximj1 > maximj2) {
            if ((maximi2 - maximi1) > 1 && (maximj1 - maximj2) > 1) {
             mesaj += "Tumoarea s-a deplasat in jos si stanga\n";
            }
            else if ((maximi2 - maximi1) > 1) {
                mesaj += "Tumoarea s-a deplatat in jos\n";
            }
            else if ((maximj1 - maximj2) > 1) {
               mesaj += "Tumoarea s-a deplasat spre stanga.\n";
            }
        }
    }


    mesaj+= "Distanta Hamming este egala cu " + QString::number(hamming) + "\n";
    mesaj+= "Distanta Levenshtein este egala cu " + QString::number(levenstein) + "\n";
    mesaj+= "Distanta euclidiana este egala cu " + QString::number(euclid) + "\n";

   return mesaj;
}

Pacient p;
void MainWindow::print()
{
    float matrice1[12][9];
    float matrice2[12][9];
    p.construire_matrice(mat1, matrice1);
    p.construire_matrice(mat2, matrice2);
    QTextBrowser* ui_TextBrowser = findChild<QTextBrowser*>("textfile");
    ui_TextBrowser->append(p.comparatie(mat1, matrice1, mat2, matrice2));

}

MainWindow::~MainWindow()
{
    delete ui;
}

