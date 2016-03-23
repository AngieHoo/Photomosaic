#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDir>
#include <cv.h>
#include <highgui.h>
#include <QMessageBox>
#include <QFileDialog>
#include <QFutureWatcher>
#include <QProgressDialog>
#include <QtConcurrent/QtConcurrent>
#include <QPainter>
#include <QLabel>
#include <QGraphicsScene>
#include <QDebug>
#include <QScrollArea>
#include <QPushButton>
#include <omp.h>
#include "msavedialog.h"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
protected:
    void askSubImageDir();
    void askMainImagePath();
    void match();
    void findBestSub();
    int calculateDis2(CvScalar point1,CvScalar point2);
    void startLoadeImgInfo();
    void loadSubImgInfo();
    void loadBaseImgInfo();
    void drawSubImg(QPoint pos,int num);
    IplImage *ConvertToIplImage(const QImage &img);

private slots:
    void on_zoomBig1_clicked();
    void on_zoomSmall1_clicked();
    void on_zoomBig2_clicked();
    void on_zoomSmall2_clicked();

private:
    inline  QImage* IplImageToQImage(const IplImage *pIplImage);
    void saveMosaic();
    void baseimgZoom(const qreal &zoom);
    void mosaicImgZoom(const qreal &zoom);
    Ui::MainWindow *ui;
    QString m_baseImgPath;
    QVector<QString> m_subImgPath;
    IplImage *m_baseImg;
    QVector<QImage> m_subImgCache;
    QImage *m_baseImg2;
    QImage *m_mosaicImg;
    QVector<CvScalar> m_subInfoCache;
    QVector<CvScalar> m_baseInfoCache;
    int m_matchflag;
    int m_wnum,m_hnum;
    qreal m_zoomBase,m_zoomMos;
    //int m_width,m_height;
    QLabel *m_base,*m_mosaic;
    int m_subLen;
    MsaveDialog *m_saveDialog;

};

#endif // MAINWINDOW_H
