#include "TextureDockWidget.h"
#include <QLabel>
#include <QLayout>
#include <QFileDialog>
#include <QGroupBox>
#include <QComboBox>

using namespace std;
TextureDockWidget::TextureDockWidget(QWidget * parent ):QDockWidget(parent)
{
    QWidget * contents = new QWidget();

    QVBoxLayout * contentLayout = new QVBoxLayout(contents);

    QGroupBox * groupBox = new QGroupBox("Cutting plane", parent);
    groupBox->setMaximumSize(QSize(16777215, 200));

    contentLayout->addWidget ( groupBox) ;

    QGridLayout * cuttingPlaneGridLayout = new QGridLayout(groupBox);
    xHSlider = new QSlider(groupBox);
    xHSlider->setOrientation(Qt::Horizontal);
    xHSlider->setMaximum(sliderMax);
    cuttingPlaneGridLayout->addWidget(xHSlider, 1, 0, 1, 1);


    yHSlider = new QSlider(groupBox);
    yHSlider->setOrientation(Qt::Horizontal);
    yHSlider->setMaximum(sliderMax);
    cuttingPlaneGridLayout->addWidget(yHSlider, 3, 0, 1, 1);


    zHSlider = new QSlider(groupBox);
    zHSlider->setOrientation(Qt::Horizontal);
    zHSlider->setMaximum(sliderMax);
    cuttingPlaneGridLayout->addWidget(zHSlider, 5, 0, 1, 1);


    QPushButton * invertXPushButton = new QPushButton("invert", groupBox);
    cuttingPlaneGridLayout->addWidget(invertXPushButton, 1, 1, 1, 1);

    QPushButton * invertYPushButton = new QPushButton("invert", groupBox);
    cuttingPlaneGridLayout->addWidget(invertYPushButton, 3, 1, 1, 1);

    QPushButton * invertZPushButton = new QPushButton("invert", groupBox);
    cuttingPlaneGridLayout->addWidget(invertZPushButton, 5, 1, 1, 1);

    QLabel * labelCutX = new QLabel("x cut position", groupBox);
    cuttingPlaneGridLayout->addWidget(labelCutX, 0, 0, 1, 1);

    QLabel * labelCutY = new QLabel("y cut position", groupBox);
    cuttingPlaneGridLayout->addWidget(labelCutY, 2, 0, 1, 1);

    QLabel * labelCutZ = new QLabel("z cut position", groupBox);
    cuttingPlaneGridLayout->addWidget(labelCutZ, 4, 0, 1, 1);

    QCheckBox * displayXCut = new QCheckBox("display", groupBox);
    cuttingPlaneGridLayout->addWidget(displayXCut, 0, 1, 1, 1);

    QCheckBox * displayYCut = new QCheckBox("display", groupBox);
    cuttingPlaneGridLayout->addWidget(displayYCut, 2, 1, 1, 1);

    QCheckBox * displayZCut = new QCheckBox("display", groupBox);
    cuttingPlaneGridLayout->addWidget(displayZCut, 4, 1, 1, 1);

    connect(xHSlider, &QSlider::valueChanged, this, &TextureDockWidget::xSliderChangedSlot);
    connect(yHSlider, &QSlider::valueChanged, this, &TextureDockWidget::ySliderChangedSlot);
    connect(zHSlider, &QSlider::valueChanged, this, &TextureDockWidget::zSliderChangedSlot);

    connect(invertXPushButton, &QPushButton::pressed, this, &TextureDockWidget::xInvertPlaneSlot);
    connect(invertYPushButton, &QPushButton::pressed, this, &TextureDockWidget::yInvertPlaneSlot);
    connect(invertZPushButton, &QPushButton::pressed, this, &TextureDockWidget::zInvertPlaneSlot);

    connect(displayXCut, &QCheckBox::stateChanged, this, &TextureDockWidget::xDisplaySlot);
    connect(displayYCut, &QCheckBox::stateChanged, this, &TextureDockWidget::yDisplaySlot);
    connect(displayZCut, &QCheckBox::stateChanged, this, &TextureDockWidget::zDisplaySlot);

    contentLayout->addStretch(0);
    this->setWidget(contents);
}

void TextureDockWidget::xSliderChangedSlot(int i) {emit xValueChanged((float)i/(float) sliderMax);}
void TextureDockWidget::ySliderChangedSlot(int i) {emit yValueChanged((float)i/(float) sliderMax);}
void TextureDockWidget::zSliderChangedSlot(int i) {emit zValueChanged((float)i/(float) sliderMax);}

void TextureDockWidget::xInvertPlaneSlot() {emit xInvert();}
void TextureDockWidget::yInvertPlaneSlot() {emit yInvert();}
void TextureDockWidget::zInvertPlaneSlot() {emit zInvert();}

void TextureDockWidget::xDisplaySlot(bool v) {emit xDisplay(v);}
void TextureDockWidget::yDisplaySlot(bool v) {emit yDisplay(v);}
void TextureDockWidget::zDisplaySlot(bool v) {emit zDisplay(v);}


void TextureDockWidget::setMaxCutPlanes( int x, int y , int z ){
        xHSlider->setRange(0,x);
        yHSlider->setRange(0,y);
        zHSlider->setRange(0,z);
}
