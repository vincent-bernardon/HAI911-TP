#ifndef TEXTUREDOCKWINDOW_H
#define TEXTUREDOCKWINDOW_H

#include "TextureViewer.h"

#include <QCheckBox>
#include <QDockWidget>
#include <QPushButton>
#include <QWidget>
#include <QListWidget>


class TextureDockWidget : public QDockWidget
{
    Q_OBJECT
    int sliderMax = 1000;
public:
    TextureDockWidget(QWidget * parent );

    QSlider *xHSlider;
    QSlider *yHSlider;
    QSlider *zHSlider;
    QPushButton *invertXPushButton;
    QPushButton *invertYPushButton;
    QPushButton *invertZPushButton;
    QCheckBox *displayXCut;
    QCheckBox *displayYCut;
    QCheckBox *displayZCut;

private slots:
    void xSliderChangedSlot(int i);
    void ySliderChangedSlot(int i);
    void zSliderChangedSlot(int i);
    void xInvertPlaneSlot();
    void yInvertPlaneSlot();
    void zInvertPlaneSlot();
    void xDisplaySlot(bool v);
    void yDisplaySlot(bool v);
    void zDisplaySlot(bool v);

public slots:
    void setMaxCutPlanes(int _xMax,int _yMax,int _zMax);

signals:
    void xValueChanged(float i);
    void yValueChanged(float i);
    void zValueChanged(float i);

    void xInvert();
    void yInvert();
    void zInvert();

    void xDisplay(bool v);
    void yDisplay(bool v);
    void zDisplay(bool v);

};

#endif // TEXTUREDOCKWINDOW_H
