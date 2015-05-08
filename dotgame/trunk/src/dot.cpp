/** ****************************************************************************
 * @file dot.cpp
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

#include "dot.h"
#include <time.h>             // to ask for system time()
#include <math.h>             // for sin(), cos(), pow(), sqrt()

/** ****************************************************************************
 * Dot Constructor
 *  @param parent Pointer to the parent window widget
 *  @param location Position of the center of the Dot relative to parent widget.
*******************************************************************************/
Dot::Dot(QWidget* parent, QPoint location) :
    QObject(parent), m_parentLocPtr(parent), m_curLoc(location), m_curMoveVect(0,0),
    m_moveDist(MOVE_DISTANCE_DEFAULT), m_motionWeight(MOTION_WEIGHT_DEFAULT),
    m_dotSize(DOT_DIAM_DEFAULT,DOT_DIAM_DEFAULT)
{
    if (m_parentLocPtr == NULL)
    {
        qDebug("dot::ctor: Must specify the dot's parent widget.");
        return;
    }
    srand(time(NULL));           // initialize random seed for motion vectors
    setm_dotSize(DOT_DIAM_DEFAULT);  // sets dot size and m_moveDist
    applyBoundaryCheck(m_curLoc);    // make sure Dot is placed within the window.
    //qDebug("Dot checked, now at (%i,%i)", m_curLoc.x(), m_curLoc.y());

    QLinearGradient gradient(QPointF(50, -20), QPointF(80, 20));
    gradient.setColorAt(0.0, Qt::white);
    gradient.setColorAt(1.0, QColor(0xa6, 0xce, 0x39));

    m_circleBrush = QBrush(gradient);
    m_circlePen = QPen(Qt::black);
    m_circlePen.setWidth(1);
    return;
}

/** ****************************************************************************
 * Function: updateLoc()
 *   Tells the dot to randomly generate a new location for itself.
*******************************************************************************/
void Dot::updateLoc()
{
    QVector2D randMoveVect(0,0);
    int tempRand = rand();
    int randIntX = (tempRand % (2*m_moveDist + 1)) - m_moveDist;      // distance (-m_moveDist<= tempX <= m_moveDist)
    randMoveVect.setX(randIntX);
    tempRand = tempRand % 2;
    if (tempRand == 0)
    {
        tempRand = -1;          // now tempRand is -1 or 1, the Y orientation.
    }
    //randMoveVect.setY(tempRand*(m_moveDist * sin(acos((float)randMoveVect.x()/(float)m_moveDist)) ));    // corresponding point on a circle
    randMoveVect.setY(tempRand*sqrt(pow(m_moveDist,2) - pow(randMoveVect.x(),2)));    // corresponding point on a circle
    /* To smooth out the random motion of the dot, do a weighted average with
       previous motion vector, so the dot moves in sort of curved lines. */
    m_curMoveVect = (m_curMoveVect * m_motionWeight) + (randMoveVect * (1-m_motionWeight));
    m_curMoveVect = (m_curMoveVect.normalized() * m_moveDist);
    QPoint offset = m_curMoveVect.toPoint();
    m_curLoc += offset;
    /* 1) Make sure that the dot doesn't move outside the window.
     *  2) If the dot hits the edge, point it back towards the center.
     *       We could rotate the vector, but inverting looks more natural,
     *       like a 'bounce'.  */
    const float invertWeight = -1.0;
    if (m_curLoc.x() < 0)
    {
        m_curLoc.setX(0);
        m_curMoveVect.setX(m_curMoveVect.x() * invertWeight);
    }
    else if (m_curLoc.x() > m_parentLocPtr->size().width())
    {
        m_curLoc.setX(m_parentLocPtr->size().width());
        m_curMoveVect.setX(m_curMoveVect.x() * invertWeight);
    }
    if (m_curLoc.y() < 0)
    {
        m_curLoc.setY(0);
        m_curMoveVect.setY(m_curMoveVect.y() * invertWeight);
    }
    else if (m_curLoc.y() > m_parentLocPtr->size().height())
    {
        m_curLoc.setY(m_parentLocPtr->size().height());
        m_curMoveVect.setY(m_curMoveVect.y() * invertWeight);
    }
    //qDebug("dot::update dist=%i randX=%.2f randY=%.2f newVectX=%.2f newVectY=%.2f m_curLocX=%i m_curLocY=%i",
    //       m_moveDist, randMoveVect.x(), randMoveVect.y(), m_curMoveVect.x(), m_curMoveVect.y(), m_curLoc.x(), m_curLoc.y());
    emit newDotLoc(m_curLoc);
    return;
} // function updateLoc()

/** ****************************************************************************
 * Function: paint()
 *   Draws the dot on the given 'painter' canvas
*******************************************************************************/
void Dot::paint(QPainter *painter, QPaintEvent *event) const
{
    Q_UNUSED(event);
    painter->setBrush(m_circleBrush);
    painter->setPen(m_circlePen);
    painter->drawEllipse(m_curLoc,m_dotSize.width(),m_dotSize.height());
    return;
}

/** ****************************************************************************
 * Function: setPos()
 *   Manually give the Dot a new position
*******************************************************************************/
void Dot::setPos(QPoint& newPos)
{
    applyBoundaryCheck(newPos);
    m_curLoc = newPos;
    emit newDotLoc(m_curLoc);
    return;
}

/** ****************************************************************************
 * Function: applyBoundaryCheck()
 *   Verify that the location is not outside the widget/window boundary.
 *   Update the passed-in location to be within the boundaries.  Doesn't
 *   actually modify the state of the Dot, so it's more flexible.
*******************************************************************************/
QPoint& Dot::applyBoundaryCheck(QPoint& tmpLoc) const
{
    if (tmpLoc.x() < 0)
        tmpLoc.setX(0);
    else if (tmpLoc.x() > m_parentLocPtr->size().width())
        tmpLoc.setX(m_parentLocPtr->size().width());
    if (tmpLoc.y() < 0)
        tmpLoc.setY(0);
    else if (tmpLoc.y() > m_parentLocPtr->size().height())
        tmpLoc.setY(m_parentLocPtr->size().height());
    return tmpLoc;
}

/** ****************************************************************************
 * Function: setm_dotSize()
 *     Dot size can be used to either adjust for screen size or to increase difficulty.
 *     For screen size, as the dot size is decreased, the distance moved should decrease.
 *     Linear scaling seemed to make the fast dots move faster, so use sqrt instead.
 *   @return Returns true if size was changed, false if value out of range.
*******************************************************************************/
bool Dot::setm_dotSize(uint newSize)
{
    if ((newSize >= DOT_DIAM_MIN) && (newSize <= DOT_DIAM_MAX))
    {
        m_dotSize.setWidth(newSize);
        m_dotSize.setHeight(newSize);
        m_moveDist = sqrt(newSize / 2);
        //qDebug("Dot::setm_dotSize: newSize=%i newm_moveDist=%i.", newSize, m_moveDist);
        return true;
    }
    else
    {
        return false;
    }
}

/** ****************************************************************************
 * Function: calcPosErrorRatio()
 *   Calculate current error distance and ratio with m_moveDist
*******************************************************************************/
float Dot::calcPosErrorRatio(const QPoint& mousePos) const
{
    QPoint errorPos = mousePos - m_curLoc;
    //uint errorDist = errorPos.manhattanLength();   // Less accurate but faster calculation
    float errorDist = sqrt(pow(errorPos.x(),2) + pow(errorPos.y(),2));  // more accurate
    float errorRatio = errorDist / m_moveDist;
//    qDebug("Dot::calcPosErrorRatio: DotLoc=(%i,%i) mousePos=(%i,%i) errorPos=(%i,%i) errorDist%.2f errorRatio=%.2f",
//           m_curLoc.x(), m_curLoc.y(), mousePos.x(), mousePos.y(), errorPos.x(), errorPos.y(), errorDist, errorRatio);
    return errorRatio;
}
