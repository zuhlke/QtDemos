/** ****************************************************************************
 * @file dot.h
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
 * The Dot class represents the dot that runs around the main window. It knows
 * its own motion vectors and how to draw itself, but only changes location
 * when instructed to (it doesn't have any timers of its own).
 *
 ******************************************************************************/

#ifndef DOT_H
#define DOT_H

#include <QtCore/QObject>
#include <QtCore/QPoint>
#include <QtCore/QSize>
#include <QtGui/QWidget>
#include <QtGui/QVector2D>
#include <QtGui/QPainter>
#include <QtGui/QBrush>
#include <QtGui/QFont>
#include <QtGui/QPen>

/** ****************************************************************************
 * @class Dot
 *   The Dot class represents the dot that runs around the main window. It knows
 *   its own motion vectors and how to draw itself, but only changes location
 *   when instructed to (it doesn't have any timers of its own).
*******************************************************************************/
class Dot : public QObject
{
    Q_OBJECT
public:
    explicit Dot(QWidget *parent, QPoint location);
    /// Repaint the Dot
    void paint(QPainter *painter, QPaintEvent *event) const;
    /// Manually give the Dot a new position
    void setPos(QPoint &newPos);
    inline const QPoint getPos() const;
    inline uint getDotSize() const;
    /// Verify that the position is not outside its window boundaries
    QPoint &applyBoundaryCheck(QPoint &tmpLoc) const;
    /// Calculate Position Error Ratio
    float calcPosErrorRatio(const QPoint &mousePos) const;

    /// Incremental distance the dot will move.  must NOT be zero.
    static const uint MOVE_DISTANCE_DEFAULT =  3;
    /// Minimum programmable dot speed
    static const uint DOT_SPEED_MIN         =  2;
    /// Default speed of the dot, in milliseconds
    static const uint DOT_SPEED_DEFAULT     = 40;
    /// Maximum programmable dot speed
    static const uint DOT_SPEED_MAX         = 50;
    /// Minimum Diameter of the drawn dot
    static const uint DOT_DIAM_MIN          =  8;
    /// Diameter of the drawn dot, in pixels
    static const uint DOT_DIAM_DEFAULT      = 20;
    /// Maximum Diameter of the drawn dot
    static const uint DOT_DIAM_MAX          = 50;
    /// Minimum Dot Acceleration
    static const uint DOT_ACCEL_MIN         =  0;
    /// Number of milliseconds to speed up the dot movement by.
    static const uint DOT_ACCEL_DEFAULT     =  1;
    /// Maximum Dot Acceleration
    static const uint DOT_ACCEL_MAX         =  5;
    /// length of time (in ms) between m_dotSpeed increases
    static const uint DOT_ACCELERATION_UPDATE_INTERVAL_MS = 2000;
    /** Dot motion is a weighted sum of current direction and new random vector.
        MOTION_WEIGHT should be 0.0<x<1.0 and represents the weight of the current vector component.*/
    static const float MOTION_WEIGHT_DEFAULT = 0.7;

signals:
    /// Manually tell the dot a new position
    void newDotLoc(const QPoint loc) const;
public slots:
    /// Tell the dot to randomly change its location
    void updateLoc();
    /// Change the dot's visual size, in pixel diameter
    bool setm_dotSize(uint newSize);
private:
    /* Private non-defined function prototypes disables compiler generation. */
    /// prevent copy operator
    Dot(const Dot&);
    /// prevent assignment operator
    Dot& operator=(const Dot&);

    // Movement-related members:
    /// Parent widget, for requesting window size
    QWidget* m_parentLocPtr;
    /// Current location of the dot
    QPoint m_curLoc;
    /// Current direction of motion
    QVector2D m_curMoveVect;
    /// Distance that dot should move when updated
    uint m_moveDist;
    /// Weight of the current vector component
    const float m_motionWeight;

    // visualizataion-related members:
    /// Size (W,H) of the dot to draw
    QSize m_dotSize;
    /// Brush for the fill color/pattern of the dot
    QBrush m_circleBrush;
    /// Line for the outside of the dot
    QPen m_circlePen;
};


/** ****************************************************************************
 * Function: getPos()
 *   What is the current position of the dot?
*******************************************************************************/
inline const QPoint Dot::getPos() const
{
    return m_curLoc;
}
/** ****************************************************************************
 * Function: getm_dotSize()
 *
*******************************************************************************/
inline uint Dot::getDotSize() const
{
    return m_dotSize.width();
}

#endif // DOT_H
