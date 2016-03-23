#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTextStream>
#include <QBoxLayout>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    QString s = QCoreApplication::applicationDirPath ();
    qDebug()<<s;
    ui->setupUi(this);
    m_matchflag = -1;
    m_wnum = 0;
    m_hnum = 0;
    m_zoomBase = 1;
    m_zoomMos = 1;
    ui->zoomBig1->setVisible(false);
    ui->zoomBig2->setVisible(false);
    ui->zoomSmall1->setVisible(false);
    ui->zoomSmall2->setVisible(false);
    m_base = new QLabel(this);
    m_mosaic = new QLabel(this);
    m_mosaicImg = new QImage();
    m_baseImg2 = new QImage();
    m_baseImg = new IplImage();
    m_saveDialog = new MsaveDialog(this);
    m_saveDialog->hide();

    connect(ui->baseimg, QPushButton::clicked,this,MainWindow::askMainImagePath);
    connect(ui->subimg, QPushButton::clicked,this,MainWindow::askSubImageDir);
    connect(ui->start, QPushButton::clicked,this,MainWindow::startLoadeImgInfo);
    connect(m_saveDialog,MsaveDialog::saveMosaic,this,MainWindow::saveMosaic);
    ui->scrollArea->setWidget(m_base);
    ui->scrollArea_2->setWidget(m_mosaic);
    ui->baseimg->setStyleSheet("QPushButton"
                               "{"
                               "border-radius:15px;"
                               " background-color: rgb(0, 170, 255);"
                               "color: rgb(255, 255, 255);"
                               "font-size:26px;"
                               "font-family:System;"
                               "}"
                               "QPushButton:pressed{"
                               "background:rgb(86, 128, 136);"
                               "}");
    ui->subimg->setStyleSheet("QPushButton"
                              "{"
                              "border-radius:15px;"
                              " background-color: rgb(0, 170, 255);"
                              "color: rgb(255, 255, 255);"
                              "font-size:26px;"
                              "font-family:System;"
                              "}"
                              "QPushButton:pressed{"
                              "background:rgb(86, 128, 136);"
                              "}");
    ui->start->setStyleSheet("QPushButton"
                             "{"
                             "border-radius:15px;"
                             " background-color: rgb(150,150,150);"
                             "color: rgb(255, 255, 255);"
                             "font-size:26px;"
                             "font-family:System;"
                             "}"
                             );

}



void MainWindow::askSubImageDir()
{
    QString subFilePath = QFileDialog::getExistingDirectory(0, "Select the directory of pictures",
                                                            "./",
                                                            QFileDialog::ShowDirsOnly
                                                            | QFileDialog::DontResolveSymlinks);
    QDir directory(subFilePath);
    directory.setFilter(QDir::Files | QDir::NoSymLinks);
    QFileInfoList infoList= directory.entryInfoList();
    m_subImgPath.clear();
    int size = infoList.size();

    for (int i = 0; i < size; i++)
    {
        m_subImgPath.append(infoList.at(i).filePath());
    }
    m_matchflag++;
    if(m_matchflag)
    {
        ui->start->setEnabled(true);
        ui->start->setStyleSheet("QPushButton"
                                 "{"
                                 "border-radius:15px;"
                                 " background-color: rgb(0, 170, 255);"
                                 "color: rgb(255, 255, 255);"
                                 "font-size:26px;"
                                 "font-family:System;"
                                 "}"
                                 "QPushButton:pressed{"
                                 "background:rgb(86, 128, 136);"
                                 "}");
    }


}

void MainWindow::askMainImagePath()
{
    m_baseImgPath= QFileDialog::getOpenFileName(0,"Select the Base picture","",
                                                "Images (*.jpg *.jpeg *.png)");
    m_baseImg =cvLoadImage(m_baseImgPath.toStdString().data());
    if(!m_baseImg)
        return;
    m_baseImg2 =IplImageToQImage(m_baseImg);
    int width = m_baseImg2->width(),height =m_baseImg2->height();
    while(width*height >400000)
    {
        width /=1.41;
        height /=1.41 ;
    }
    QImage tempimg = m_baseImg2->scaled(QSize(width,height), Qt::KeepAspectRatio,Qt::SmoothTransformation);
    QPixmap pic(QPixmap::fromImage(tempimg));
    m_base->setPixmap(pic);
    m_base->resize(QSize(pic.width(),pic.height()));
    m_zoomBase = 1;
    ui->zoomBig1->setVisible(true);
    ui->zoomSmall1->setVisible(true);
    m_matchflag++;
    if(m_matchflag)
    {
        ui->start->setEnabled(true);
        ui->start->setStyleSheet("QPushButton"
                                 "{"
                                 "border-radius:15px;"
                                 " background-color: rgb(0, 170, 255);"
                                 "color: rgb(255, 255, 255);"
                                 "font-size:26px;"
                                 "font-family:System;"
                                 "}"
                                 "QPushButton:pressed{"
                                 "background:rgb(86, 128, 136);"
                                 "}");
    }
    // cvReleaseImage(&tempImg);

}
void MainWindow::startLoadeImgInfo()
{
    delete m_mosaicImg;
    m_mosaicImg = new QImage();
    m_mosaicImg = IplImageToQImage(m_baseImg);
    m_baseInfoCache.clear();
    m_subInfoCache.clear();
    m_subImgCache.clear();
    m_zoomMos = 1;
    int width =m_baseImg->width,height =m_baseImg->height;
    int rank = ui->comboBox->currentIndex();
    int devide;
    switch(rank)
    {
    case 0: devide=25;break;
    case 1: devide= 50;break;
    case 2:devide= 100;break;
    case 3:devide= 200;break;
    case 4:devide= 400;break;
    default: break;
    }
    qDebug()<<"rank:"<<rank<<"\n";
    m_subLen =(width<height?width:height)/devide;
    if(m_subLen < 1)m_subLen = 1;

    QProgressDialog pd(this);
    pd.setWindowTitle("Please Wait");
    pd.setLabelText("processing photos..... ^u^");
    pd.setRange(0,0);
    pd.setCancelButton(NULL);
    pd.setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint);
    QFutureWatcher<void> watcher_base(this);
    QFuture<void> lodeBaseThread =  QtConcurrent::run(this,loadBaseImgInfo);
    QObject::connect(&watcher_base,SIGNAL(finished()),&pd,SLOT(reset()));
    QObject::connect(&pd,SIGNAL(canceled()),&watcher_base,SLOT(cancel()));
    watcher_base.setFuture(lodeBaseThread);
    pd.exec();
    watcher_base.waitForFinished();

    QProgressDialog pd2(this);
    pd2.setWindowTitle("Please Wait");
    pd2.setLabelText("processing  photos..... ^u^");
    pd2.setRange(0,0);
    pd2.setCancelButton(NULL);
    pd2.setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint);
    QFutureWatcher<void> watcher_sub(this);
    QFuture<void> lodeSubThread = QtConcurrent::run(this,loadSubImgInfo);
    QObject::connect(&watcher_sub,SIGNAL(finished()),&pd2,SLOT(reset()));
    QObject::connect(&pd2,SIGNAL(canceled()),&watcher_sub,SLOT(cancel()));
    watcher_sub.setFuture(lodeSubThread);
    pd2.exec();
    watcher_sub.waitForFinished();

    match();


}
QImage* MainWindow::IplImageToQImage(const IplImage *pIplImage)
{
    QImage *qImage;
    int w = pIplImage->width;
    int h = pIplImage->height;
    qImage = new QImage(w, h, QImage::Format_RGB32);

    int x, y;
    for(x = 0; x < pIplImage->width; ++x)
    {
        for(y = 0; y < pIplImage->height; ++y)
        {
            CvScalar color = cvGet2D(pIplImage, y, x);
            int r = color.val[2];
            int g = color.val[1];
            int b = color.val[0];
            qImage->setPixel(x, y, qRgb(r,g,b));
        }
    }

    return qImage;
}
void MainWindow::loadSubImgInfo()
{

    CvScalar avg = cvScalarAll(0);
    QImage *qimg;
    int len;
    //int flag =0 ;
    qDebug()<<"number:"<<m_subImgPath.size()<<"\n";
   // #pragma omp parallel for
    for(int i = 0;i<m_subImgPath.size();i++)
    {
        IplImage * img = cvLoadImage(m_subImgPath.at(i).toStdString().data(),CV_LOAD_IMAGE_COLOR);
        len= img->width<img->height?img->width:img->height;
        IplImage *img2 = cvCreateImage(cvSize(img->width*m_subLen/len,img->height*m_subLen/len),
                                       img->depth,img->nChannels);
        cvResize(img, img2, CV_INTER_LINEAR);
        cvSetImageROI(img2,cvRect(0,0,m_subLen,m_subLen));
        IplImage *dst = cvCreateImage(cvSize(m_subLen,m_subLen),IPL_DEPTH_8U,img2->nChannels);
        cvCopy(img2,dst,0);
        cvResetImageROI(img2);
        qimg =IplImageToQImage(img2);
       // #pragma omp critical
        m_subImgCache.append(*qimg);
        // qDebug()<<i<<"\n";
        avg = cvAvg(img2);
      //  #pragma omp critical
        m_subInfoCache.append(avg);
        // flag++;
        // qDebug()<<flag<<"\n";
        cvReleaseImage(&img2);
        cvReleaseImage(&dst);
        cvReleaseImage(&img);
    }
    qDebug()<<"number:"<<m_subInfoCache.size()<<"\n";
}


void MainWindow::loadBaseImgInfo()
{
    m_hnum = 0;
    m_wnum = 0;
    int width =m_baseImg->width,height =m_baseImg->height;
    CvRect subRect;
    CvMat *model = cvCreateMat(m_subLen, m_subLen, CV_8U);
    CvScalar avg = cvScalarAll(0);
    for (int i = 0; i<height; i = i + m_subLen)
    {
        m_hnum++;
        for (int j = 0; j<width; j = j + m_subLen)
        {
            int temp_w,temp_h;
            if(j+m_subLen>width)
                temp_w = width-j;
            else temp_w = m_subLen;
            if(i+m_subLen>height)
                temp_h = height-i;
            else temp_h = m_subLen;
            subRect = cvRect(j, i,temp_w,temp_h);
            cvGetSubRect(m_baseImg, model, subRect);
            avg = cvAvg(model);
            m_baseInfoCache.append(avg);
        }
    }
    m_wnum = m_baseInfoCache.size()/m_hnum;
    qDebug()<<"the number of mosaic:"<<m_hnum*m_wnum<<"\n";
    cvReleaseMat(&model);


}

void MainWindow::match()
{
    QElapsedTimer timer;
    timer.start();

    QProgressDialog pd3(this);
    pd3.setWindowTitle("Please Wait");
    pd3.setLabelText("Creating mosaic.....");
    pd3.setRange(0,0);
    pd3.setCancelButton(NULL);
    pd3.setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint);
    QFutureWatcher<void> watcher_mosaic(this);
    QFuture<void> mosaicThread =  QtConcurrent::run(this,findBestSub);
    QObject::connect(&watcher_mosaic,SIGNAL(finished()),&pd3,SLOT(reset()));
    QObject::connect(&pd3,SIGNAL(canceled()),&watcher_mosaic,SLOT(cancel()));
    watcher_mosaic.setFuture(mosaicThread);
    pd3.exec();
    watcher_mosaic.waitForFinished();
    mosaicThread.waitForFinished();

    int width = m_mosaicImg->width(),height = m_mosaicImg->height();
    while(width*height >400000)
    {
        width /=1.41;
        height /=1.41 ;
    }
    QImage tempimg = m_mosaicImg->scaled(QSize(width,height), Qt::KeepAspectRatio,Qt::SmoothTransformation);
    QPixmap pic(QPixmap::fromImage(tempimg));
    m_mosaic->setPixmap(pic);
    m_mosaic->resize(QSize(pic.width(),pic.height()));
    m_saveDialog->show();
    ui->zoomBig2->setVisible(true);
    ui->zoomSmall2->setVisible(true);
    qDebug() << "processing photos takes " << timer.elapsed() << "milliseconds";
}
void MainWindow::drawSubImg(QPoint pos,int num)
{
    QPainter painter(m_mosaicImg);

    QImage img = m_subImgCache.at(num);//,img2;


    painter.drawImage(pos,img);

}

int MainWindow::calculateDis2(CvScalar point1,CvScalar point2)
{
    return (pow(point1.val[0]-point2.val[0],2)
            +pow(point1.val[1]-point2.val[1],2)
            +pow(point1.val[2]-point2.val[2],2));
}

void MainWindow::findBestSub()
{
    QElapsedTimer timer;
    timer.start();
    int min = 255*255*3,x = 0,y = 0, tmin,tnum,re;

    for(int i = 0 ; i< m_baseInfoCache.size();i++)
    {
        tmin = min,tnum = -1;
        for(int j = 0; j < m_subInfoCache.size();j++)
        {
            re =calculateDis2(m_baseInfoCache.at(i),m_subInfoCache.at(j));
            if(re<tmin)
            {
                tnum = j;
                tmin = re;
            }
        }
        y = i/m_wnum;
        x = i%m_wnum;
        drawSubImg(QPoint(x*m_subLen,y*m_subLen),tnum);
    }
    qDebug() << "creating mosaic takes" << timer.elapsed() << "milliseconds";
}

void MainWindow::saveMosaic()
{
    QString filename = QFileDialog::getSaveFileName(this,
                                                    tr("Save Image"),
                                                    "",
                                                    tr("Images (*.jpg)"));
    if(filename.isEmpty())
    {
        return;
    }
    else
    {
        if(! ( m_mosaicImg->save(filename) ) )
        {
            QMessageBox::information(this,
                                     tr("Failed to save the mosaics"),
                                     tr("Failed to save the mosaics！"));
            return;
        }
        else
        {
            QMessageBox::information(this,
                                     tr("successfully  save the mosaics"),
                                     tr("successfully  save the mosaics！"));
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;
    delete  m_baseImg;
    delete m_mosaicImg;
    delete m_base;
    delete m_mosaic;
    delete m_mosaicImg;
}
void MainWindow::baseimgZoom(const qreal &zoom)
{
    QImage tempimg= m_baseImg2->scaled(m_baseImg2->width()*zoom,
                                       m_baseImg2->height()*zoom,
                                       Qt::KeepAspectRatio,Qt::SmoothTransformation);
    QPixmap pic(QPixmap::fromImage(tempimg));
    m_base->setPixmap(pic);
    m_base->resize(QSize(pic.width(),pic.height()));

}
void MainWindow::mosaicImgZoom(const qreal &zoom)
{
    QImage tempimg= m_mosaicImg->scaled(QSize(m_mosaicImg->width()*zoom,
                                              m_mosaicImg->height()*zoom),
                                        Qt::KeepAspectRatio,Qt::SmoothTransformation);
    QPixmap pic(QPixmap::fromImage(tempimg));
    m_mosaic->setPixmap(pic);
    m_mosaic->resize(QSize(pic.width(),pic.height()));

}
void MainWindow::on_zoomBig1_clicked()
{
    if(m_zoomBase>5)
        return;
    m_zoomBase *= 1.100;
    baseimgZoom(m_zoomBase);

}


void MainWindow::on_zoomSmall1_clicked()
{
    if(m_zoomBase<0.1)
        return;
    m_zoomBase *= 0.900;
    baseimgZoom(m_zoomBase);
}

void MainWindow::on_zoomBig2_clicked()
{
    if( m_zoomMos>5)
        return;
    m_zoomMos *= 1.100;
    mosaicImgZoom(m_zoomMos);
}

void MainWindow::on_zoomSmall2_clicked()
{
    if(m_zoomMos<0.1)
        return;
    m_zoomMos *= 0.900;
    mosaicImgZoom(m_zoomMos);
}
