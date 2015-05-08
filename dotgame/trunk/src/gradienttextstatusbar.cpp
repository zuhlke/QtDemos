/** ****************************************************************************
 * @file gradienttextstatusbar.cpp
 * @author Jeremy Hannon <hje@zuhlke.com>
 * @version 0.4
 * @date July 11th, 2012
 * Created: June 19th, 2012
 *
 * @section LICENSE
 *     All rights reserved, Zuhlke Engineering Ltd.  Copyright 2012.
 *
 * @section DESCRIPTION
 * The DotGame is a Qt application involving a randomly walking dot that the
 * user tries to closely follow with their cursor to achieve a high m_score.
 *
 * This GradientTextStatusBar is intended to provide the general appearance of
 * two bits of text (left and right) along with a color gradient that appears
 * like a line.  The gradient is shifted left and right to indicate status.
 * This widget is intended to be placed in the Status Bar area of the MainWindow.
 *
 ******************************************************************************/

#include "gradienttextstatusbar.h"
#include <QtCore/QString>
#include <stdio.h>             // for snprintf()

/** ****************************************************************************
 * GradientTextStatusBar Constructor
 *   @param parent Pointer to the parent window widget
 ******************************************************************************/
GradientTextStatusBar::GradientTextStatusBar(QWidget *parent) :
    QWidget(parent),
    statusBarGradient(40, 0, 70, 10),
    distString("0.00"),
    timeString("GameTime: 0"),
    checkedErrorDistRatio(0.0)
{
    statusBarGradient.setColorAt(0.0, Qt::green);
    statusBarGradient.setColorAt(1.0, Qt::black);
    updateValues(0.0, 0);
}

/** ****************************************************************************
 * Function: updateValues()
 *    Update the gradient positioning and printed text.
 *    @param errorDistRatio Ratio of current distance to the max,
 *          so ideally 0.0<ratio<1.0 although it may exceed 1.0 when the game ends.
 *    @param elapsedTime The number of seconds that the game has been running, minus any game pauses.
 *    Assume errorDistRatio should be between 0 and 1, representing error
 *    distance as a ratio of the max allowable error distance for the game.
*******************************************************************************/
void GradientTextStatusBar::updateValues(const float errorDistRatio, const int elapsedTime)
{
    //qDebug("GradientTextStatusBar::updateValue: runAverage=%.2f", errorDistRatio);
    checkedErrorDistRatio = errorDistRatio;
    if (checkedErrorDistRatio > 1.1)  // higher than 1.0 to make sure it overdraws the gradient.
    {
        checkedErrorDistRatio = 1.1;
    }
    else if (checkedErrorDistRatio < 0.0)
    {
        checkedErrorDistRatio = 0.0;
    }

    /* Sliding Gradient bar must be recalculated in 'paintEvent' instead of
     *   here so that it adjusts when the window is resized. */

    // adjust the color of the bar to be more green for low distances and redder for larger ratios
    const uint GOOD_COLOR_HSV = 130; // ~Green
    const uint BAD_COLOR_HSV = 0;  // ~Red
    int errorDistColorHsv = GOOD_COLOR_HSV -
            ((GOOD_COLOR_HSV - BAD_COLOR_HSV)*checkedErrorDistRatio);
    if (errorDistColorHsv < 0)
    {
        errorDistColorHsv = 0;
    }
    QColor tempHSV;
    tempHSV.setHsv(errorDistColorHsv,255,150);
    statusBarGradient.setColorAt(0.0,tempHSV);

    // Update the display text.
    const size_t TEMP_TEXT_SIZE = 32;
    char tempText[TEMP_TEXT_SIZE];
    if (checkedErrorDistRatio > 1.0)
    {
        snprintf(tempText, TEMP_TEXT_SIZE, "  Dist: %0.0f%%  Too Far!!", (100*checkedErrorDistRatio));
    }
    else if (checkedErrorDistRatio > 0.6)
    {
        snprintf(tempText, TEMP_TEXT_SIZE, "  Dist: %0.0f%%  Careful!", (100*checkedErrorDistRatio));
    }
    else
    {
        snprintf(tempText, TEMP_TEXT_SIZE, "  Dist: %0.0f%%", (100*checkedErrorDistRatio));
    }
    distString = tempText;
    snprintf(tempText, TEMP_TEXT_SIZE, "GameTime: %i  ", elapsedTime);
    timeString = tempText;

    update();     // redraw the status bar, via paintEvent()

    return;
} // function updateValues

/** ****************************************************************************
 * Function: paintEvent()
 *   Called by update() or repaint(), tells the status bar to redraw itself
*******************************************************************************/

void GradientTextStatusBar::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    // slide the gradient back and forth along the bar
    const QPoint GRAD_TOP_LEFT(-15,-10);     // points defining the width, initial position of the color gradient
    const QPoint GRAD_BOTTOM_RIGHT(15,10);
    QPointF stopPos((checkedErrorDistRatio * size().width()) + GRAD_BOTTOM_RIGHT.x(),
                    GRAD_BOTTOM_RIGHT.y());
    QPointF startPos = stopPos + QPoint(GRAD_TOP_LEFT.x() - GRAD_BOTTOM_RIGHT.x(),
                                        GRAD_TOP_LEFT.y() - GRAD_BOTTOM_RIGHT.y());
    statusBarGradient.setStart(startPos);
    statusBarGradient.setFinalStop(stopPos);

    // paint all of the status bar components
    QPainter painter(this);
    painter.fillRect(rect(), QBrush(statusBarGradient)); // Paint over the whole area
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(QPen(Qt::black));
    painter.drawText(rect(), Qt::AlignVCenter, distString);
    painter.setPen(QPen(Qt::white));
    painter.drawText(rect(), (Qt::AlignVCenter | Qt::AlignRight), timeString);
    return;
}
