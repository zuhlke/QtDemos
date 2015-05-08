/** ****************************************************************************
 * @file qsliderdeclarative.h
 * @author Jeremy Hannon <hje@zuhlke.com>
 * @version 0.5
 * @date July 16th, 2012
 * Created: July 16th, 2012
 *
 * @section LICENSE
 *     All rights reserved, Zuhlke Engineering Ltd.  Copyright 2012.
 *
 * @section DESCRIPTION
 * The DotGame is a Qt application involving a randomly walking dot that the
 * user tries to closely follow with their cursor to achieve a high score.
 *
 * (local explanation goes here.)
 *
 ******************************************************************************/

#ifndef QSLIDERDECLARATIVE_H
#define QSLIDERDECLARATIVE_H

#include <QDeclarativeItem>
#include <QSlider>

class QSliderDeclarative : public QDeclarativeItem
{
    Q_OBJECT
public:
    explicit QSliderDeclarative(QDeclarativeItem *parent = 0);
    //virtual void paintEvent(QPaintEvent *event);
    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget = 0);
private:
    QSlider slider;
signals:
    void valueChanged(int value);
public slots:
    void setRange(int min, int max);
    void setValue(int newValue);
};

#endif // QSLIDERDECLARATIVE_H
