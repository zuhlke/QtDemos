/** ****************************************************************************
 * @file gradienttextstatusbar.h
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

#ifndef GRADIENTTEXTSTATUSBAR_H
#define GRADIENTTEXTSTATUSBAR_H

#include <QtGui/QWidget>
#include <QtGui/QPainter>
#include <QtGui/QLabel>

/** ****************************************************************************
 * @class GradientTextStatusBar
 *   This GradientTextStatusBar is intended to provide the general appearance of
 *   two bits of text (left and right) along with a color gradient that appears
 *   like a line.  The gradient is shifted left and right to indicate status.
 *   This widget is intended to be placed in the Status Bar area of the MainWindow.
*******************************************************************************/
class GradientTextStatusBar : public QWidget
{
    Q_OBJECT
public:
    explicit GradientTextStatusBar(QWidget *parent = 0);
protected:
    virtual void paintEvent(QPaintEvent *event);
private:
    /* Private non-defined function prototypes disables compiler generation. */
    /// prevent copy operator
    GradientTextStatusBar(const GradientTextStatusBar&);
    /// prevent assignment operator
    GradientTextStatusBar& operator=(const GradientTextStatusBar&);

    QLinearGradient statusBarGradient;
    QString distString;
    QString timeString;
    float checkedErrorDistRatio;
signals:
    
public slots:
    void updateValues(const float errorDistRatio, const int elapsedTime);
};

#endif // GRADIENTTEXTSTATUSBAR_H
