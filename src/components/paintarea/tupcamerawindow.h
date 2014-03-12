#ifndef TUPCAMERAWINDOW_H
#define TUPCAMERAWINDOW_H

#include "tupvideosurface.h"

#include <QtGui>
#include <QCamera>
#include <QCameraImageCapture>

class TupCameraWindow: public QWidget, public VideoIF
{
    Q_OBJECT

    public:
        TupCameraWindow(QCamera *input, const QSize &camResolution, const QSize &displayResolution, 
                        QCameraImageCapture *imageCapture, const QString &path, QWidget *parent = 0);
        ~TupCameraWindow();

        void startCamera();
        void stopCamera();

        void paintEvent(QPaintEvent *event);
        void updateVideo();
        void reset();
        void drawGrid(bool flag);
        void drawActionSafeArea(bool flag);
        void showHistory(bool flag);
        void updateImagesOpacity(double opacity);
        void updateImagesDepth(int depth);
        void updateGridSpacing(int space);

    signals:
        void pictureHasBeenSelected(int id, const QString path);

    public slots:
        void takePicture(int counter);
   
    private slots: 
        void imageSavedFromCamera(int id, const QString path);
        void error(QCamera::Error error);

    private:
        struct Private;
        Private *const k;
};

#endif // TUPCAMERAWINDOW_H 
