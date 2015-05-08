/** ****************************************************************************
 * @file qsliderdeclarative.cpp
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

#include "qsliderdeclarative.h"

QSliderDeclarative::QSliderDeclarative(QDeclarativeItem *parent) :
    QDeclarativeItem(parent),
    slider(Qt::Horizontal)
{
    setFlag(QGraphicsItem::ItemHasNoContents, false);
    QObject::connect(&slider, SIGNAL(valueChanged(int)),
                     this, SIGNAL(valueChanged(int)));
}

/** ****************************************************************************
 * Function: gameOver()
 *   MainWindow gets informed by dotGameWidget that game is over.
*******************************************************************************/
void QSliderDeclarative::paint(QPainter *painter,
           const QStyleOptionGraphicsItem *option,
           QWidget *widget)
{
    //qDebug("QSliderDeclarative::paint: function called.");
    //painter->save();  // save existing painter settings
    slider.render(painter);
    //painter->restore();  // restore painter settings to how they were.
    return;
}

/** ****************************************************************************
 * Function: setRange()
 *   Wrapper function for QSlider object.
*******************************************************************************/
void QSliderDeclarative::setRange(int min, int max)
{

    slider.setRange(min,max);
    return;
}

/** ****************************************************************************
 * Function: setValue()
 *   Wrapper function for QSlider object.
*******************************************************************************/
void QSliderDeclarative::setValue(int newValue)
{
    slider.setValue(newValue);
    //emit valueChanged(newValue);
    return;
}
