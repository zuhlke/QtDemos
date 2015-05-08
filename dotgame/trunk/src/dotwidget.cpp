/** ****************************************************************************
 * @file dotwidget.cpp
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
 * The DotWidget object is the main playing field for the game. It contains the
 * animated dot object, calculates distance and m_scores, and contains the main
 * game state machine.
 *
 ******************************************************************************/

#include "dotwidget.h"
#include <QtCore/QSize>
#include <QtGui/QResizeEvent>
#include <math.h>             // for pow(), sqrt()
#include <stdio.h>            // for itoa(), snprintf()

/** ****************************************************************************
 * DotWidget Constructor
 *  @param parent Pointer to the parent window widget
 *  @param winSize Current size od the DotWidget, so Dot is placed in the center
*******************************************************************************/
DotWidget::DotWidget(QWidget *parent, QSize winSize) :
    QWidget(parent), m_gameDotPtr(NULL), m_dotSpeed(Dot::DOT_SPEED_DEFAULT),
    m_dotSpeedUserSetting(Dot::DOT_SPEED_DEFAULT), m_dotAcceleration(Dot::DOT_ACCEL_DEFAULT),
    m_dotTimer(this), m_accelerationTimer(this), m_gameStatsTimer(this),
    m_secondTimerPtr(this), m_elapsedGameTime(0), m_gameLength(GAME_LENGTH_DEFAULT_S),
    m_gameState(PAUSED), m_lastGameOverReason(NO_GAME_OVER),
    m_runningAvgErrorRatio(RUNNING_AVERAGE_ERROR_DEFAULT), m_score(0.0),
    m_backgroundBrush(QBrush(QColor(Qt::blue), Qt::BDiagPattern))
{
    resize(winSize);
    QPoint dotLoc((winSize.width()/2),(winSize.height()/2));
    m_gameDotPtr = new Dot(this,dotLoc);
    // when the Dot changes its location, redraw the widget.
    //    use update() for scheduled redraw, or repaint() for immediate redraw.
    connect(m_gameDotPtr, SIGNAL(newDotLoc(const QPoint)), this, SLOT(update()));    // scheduled redraw
    connect(m_gameDotPtr, SIGNAL(newDotLoc(const QPoint)), this, SLOT(newDotPosQpnt(const QPoint)));    // inform higher levels

    m_textFontSmall.setPointSize(8);
    m_textFontLarge.setPointSize(16);
    setScoreText(0);     // initialize the m_scoreString.

    m_dotTimer.setInterval(Dot::DOT_SPEED_DEFAULT);
    m_gameStatsTimer.setInterval(GAME_UPDATE_INTERVAL_MS);
    m_accelerationTimer.setInterval(Dot::DOT_ACCELERATION_UPDATE_INTERVAL_MS);
    connect(&m_dotTimer, SIGNAL(timeout()), m_gameDotPtr, SLOT(updateLoc()));
    connect(&m_gameStatsTimer, SIGNAL(timeout()), this, SLOT(calcGameStats()));
    connect(&m_accelerationTimer, SIGNAL(timeout()), this, SLOT(increaseDotSpeed()));
    const int MSEC_IN_SEC = 1000;
    m_secondTimerPtr.setInterval(MSEC_IN_SEC);
    QObject::connect(&m_secondTimerPtr, SIGNAL(timeout()),
                     this, SLOT(updateElapsedGameTime()));
}

/** DotWidget Destructor */
DotWidget::~DotWidget()
{
    // ONLY delete objects that are not in the window hierarchy
    if (m_gameDotPtr != NULL)
        delete m_gameDotPtr;
}


/** ****************************************************************************
 * Function: initialize()
 *   Some parameters can't be set in the constructor.  Specifically, the dot
 *   should be in the middle of the window, but window size isn't known until
 *   the Widget is added to its higher-level window.
*******************************************************************************/
void DotWidget::initialize()
{
    QSize widgSize(this->size());
    QPoint newDotLoc((widgSize.width()/2),(widgSize.height()/2));
    m_gameDotPtr->setPos(newDotLoc);
    return;
}

/** ****************************************************************************
 * Function: paintEvent()
 *   Called by update() or repaint(), tells the Dot to redraw itself
*******************************************************************************/
void DotWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    //painter.fillRect(event->rect(), background);   // DEBUG: disable background to see the QML.
    painter.setRenderHint(QPainter::Antialiasing);
    m_gameDotPtr->paint(&painter, event);
    painter.setFont(m_textFontLarge);
    painter.drawText(0, size().height()/30, size().width(), size().height(),
                     Qt::AlignHCenter, m_scoreString);
    switch (m_gameState)
    {
    case RUNNING:
        break;  // don't print any extra text
    case PAUSED:
        painter.setFont(m_textFontLarge);
        // drawText(x position, y position, width, height, flags, text);
        painter.drawText(0, size().height()*2/3, size().width(),
                         size().height(), Qt::AlignHCenter, tr("Paused"));
        painter.setFont(m_textFontSmall);
        painter.drawText(0, size().height()*5/6, size().width(),
                         size().height(), Qt::AlignHCenter, tr("Click to Start/Resume"));
        painter.drawText(0, size().height()*9/10, size().width(),
                         size().height(), Qt::AlignHCenter, tr("Follow the Dot to get a High Score!"));
        break;
    case GAMEOVER:
        painter.setFont(m_textFontLarge);
        if (m_lastGameOverReason == TIME_LIMIT)
        {
            painter.drawText(0, size().height()*2/3, size().width(),
                             size().height(), Qt::AlignHCenter, tr("You Beat the Clock!!"));
        }
        else if (m_lastGameOverReason == DISTANCE)
        {
            painter.drawText(0, size().height()*2/3, size().width(),
                             size().height(), Qt::AlignHCenter, tr("Game Over"));
        }
        painter.setFont(m_textFontSmall);
        painter.drawText(0, size().height()*5/6, size().width(),
                         size().height(), Qt::AlignHCenter, tr("Click to Start a New Game"));
        break;
    default:
        qDebug("DotWidget::paintEvent: Unknown m_gameState!");
        break;
    }
    return;
} // function paintEvent

/** ****************************************************************************
 * Function: mousePressEvent()
 *   Called when user clicks in the window.  Use this to trigger dot movement.
*******************************************************************************/
void DotWidget::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    qDebug("DotWidget::mousePressEvent: action.");
    screenClicked();
    return;
} // function mousePressEvent

/** ****************************************************************************
 * Function: resizeEvent() reimplemented from QWidget
 *   Called after the window is resized, but before it is redrawn.
 *   Adjusts the position of the dot to have the same offset from center
 *   as before the window resizing.
*******************************************************************************/
void DotWidget::resizeEvent(QResizeEvent *event)
{
    //qDebug("DotWidget::resizeEvent: window size is now %i by %i", size().width(), size().height());
    if ((m_gameDotPtr != NULL) && (event->oldSize().width() > 0))
    {
        // adjust the position of the dot to retain relative offset from center.
        QPoint dotPos(m_gameDotPtr->getPos());
        dotPos.setX(dotPos.x()+((size().width() - event->oldSize().width())/2));
        dotPos.setY(dotPos.y()+((size().height() - event->oldSize().height())/2));
        m_gameDotPtr->applyBoundaryCheck(dotPos);   // affects the dotPos, not the actual gameDot yet
        m_gameDotPtr->setPos(dotPos);
        //qDebug("DotWidget::resizeEvent: width was %i now %i moved dot by %i", size().width(), event->oldSize().width(), adjX);
        //qDebug("DotWidget::resizeEvent: height was %i now %i moved dot by %i", size().height(), event->oldSize().height(), adjY);
    }
    return;
}

/** ****************************************************************************
 * Function: setm_dotSize()
 *
*******************************************************************************/
bool DotWidget::setDotSize(const int newSize)
{
    bool result = m_gameDotPtr->setm_dotSize(newSize);
    update();
    return result;
}

/** ****************************************************************************
 * Function: userSetDotSpeedNeg()
 *   @param newSpeedMs dot speed in milliseconds between position updates.
 *   Sets the timer interval for how often the dot moves.  During each game, the
 *   m_dotSpeed may increase. Use the user setting as the new default speed for
 *   the dot at the beginning of a new game.
*******************************************************************************/
bool DotWidget::setDotSpeedNeg(const int newSpeedMs)
{
    // Speed slider values were inverted so that the bigger/slower numbers were on the left side.
    uint newSpeedMsAbs = abs(newSpeedMs);
    if ((newSpeedMsAbs >= Dot::DOT_SPEED_MIN) && (newSpeedMsAbs <= Dot::DOT_SPEED_MAX))
    {
        m_dotTimer.setInterval(newSpeedMsAbs);
        m_dotSpeed = newSpeedMsAbs;
        if (m_gameState != RUNNING)
        {
            // While RUNNING, speed can change automatically.  Otherwise, user
            //    is changing the speed, so make this the new default.
            m_dotSpeedUserSetting = newSpeedMsAbs;
            //qDebug("DotWidget::setm_dotSpeed: m_dotSpeedUserSetting=%i", newSpeedMsAbs);
        }
        return true;
    }
    else
    {
        return false;
    }
}

/** ****************************************************************************
 * Function: setDotAcceleration()
 *   @param newAcceleration Rate at which the dot moves faster.
*******************************************************************************/
void DotWidget::setDotAcceleration(const int newAcceleration)
{
    //qDebug("DotWidget::setm_dotAcceleration: newAcceleration=%i", newAcceleration);
    if (newAcceleration >= 0)
    {
        m_dotAcceleration = newAcceleration;
    }
    if ((newAcceleration > 0) && (m_gameState == RUNNING))
    {
        m_accelerationTimer.start();
    }
    else
    {
        m_accelerationTimer.stop();
    }
    return;
}

/** ****************************************************************************
 * Function: increaseDotSpeed()
 *
*******************************************************************************/
void DotWidget::increaseDotSpeed()
{
    //qDebug("DotWidget::increaseDotSpeed: Oldspeed=%i accel=%i", m_dotSpeed, m_dotAcceleration);
    m_dotSpeed -= m_dotAcceleration;
    if (m_dotSpeed < Dot::DOT_SPEED_MIN)
    {
        m_dotSpeed = Dot::DOT_SPEED_MIN;
        m_accelerationTimer.stop();
    }
    emit newDotSpeedNeg(-m_dotSpeed);
    return;
}

/** ****************************************************************************
 * Function: calcGameStats()
 *     1) Calculate the error distance between mouse pointer and dot location.
 *     2) Calculate a running m_score
 *     3) Is the game over due to distance?
*******************************************************************************/
void DotWidget::calcGameStats()
{
    QPoint mousePosition = mapFromGlobal(QCursor::pos());
    float posErrorRatio = m_gameDotPtr->calcPosErrorRatio(mousePosition);
    m_runningAvgErrorRatio = (m_runningAvgErrorRatio * (ERROR_AVERAGING_RATIO - 1)
                         + posErrorRatio) / ERROR_AVERAGING_RATIO;
//    QPoint dotPos = m_gameDotPtr->getPos();
//    qDebug("DotWidget::calcGameStats: mousePos=(%i,%i) dotPos=%i,%i) errorRatio=%.2f runAverage=%.2f",
//           mousePosition.x(), mousePosition.y(), dotPos.x(), dotPos.y(), posErrorRatio, m_runningAvgErrorRatio);
    emit newFollowDistanceRatio(m_runningAvgErrorRatio, m_elapsedGameTime);
    float distm_scoreComp = 10 * (CURSOR_DISTANCE_GAMEOVER - m_runningAvgErrorRatio) /
            CURSOR_DISTANCE_GAMEOVER;
    float speedm_scoreComp = static_cast<float>(Dot::DOT_SPEED_MAX - m_dotSpeed) /
            (Dot::DOT_SPEED_MAX - Dot::DOT_SPEED_MIN);
    float compositem_scoreInc = pow(distm_scoreComp,3) * speedm_scoreComp / 40;
    m_score += compositem_scoreInc;
//    qDebug("DotWidget::calcGameStats: distm_score=%.2f speedm_score=%.2f comp=%.2f newm_score=%.2f",
//           distm_scoreComp, speedm_scoreComp, compositem_scoreInc, m_score);
    setScoreText(m_score);
    if (m_runningAvgErrorRatio >= CURSOR_DISTANCE_GAMEOVER)
    {
        //qDebug("DotWidget::calcGameStats: Distance Limit reached, GameOver");
        setGameOver(DISTANCE);
    }
    return;
}

/** ****************************************************************************
 * Function: setTimeLimit()
 *
*******************************************************************************/
bool DotWidget::setTimeLimit(const int timeLimit)
{
    qDebug("DotWidget::setTimeLimit: value=%i", timeLimit);
    if (timeLimit >= 0)
    {
        m_gameLength = timeLimit;
        return true;
    }
    else
    {
        qDebug("DotWidget::setTimeLimit: Value must be greater than zero.");
        return false;
    }
}

/** ****************************************************************************
 * Function: updatem_elapsedGameTime()
 *
*******************************************************************************/
void DotWidget::updateElapsedGameTime()
{
    if (m_gameState == RUNNING)
    {
        m_elapsedGameTime += 1;       // assumes the time increment is 1 second.
        if ((m_elapsedGameTime >= m_gameLength) && (m_gameLength != INFINITE))
        {
            //qDebug("DotWidget::updatem_elapsedGameTime: Time Limit reached, GameOver");
            setGameOver(TIME_LIMIT);
        }
    }
    // else do nothing, don't increment 'elapsedTime'.
    return;
}

/** ****************************************************************************
 * Function: setBackgroundColor()
 *
*******************************************************************************/
void DotWidget::setBackgroundColor(const int newHSVcolor)
{
    QColor tempHSV;
    tempHSV.setHsv(newHSVcolor,255,150);
    m_backgroundBrush.setColor(tempHSV);
    update();
    return;
}

/** ****************************************************************************
 * Function: setScoreText()
 *
*******************************************************************************/
void DotWidget::setScoreText(const uint newScore)
{
    //qDebug("DotWidget::setm_score: m_score=%5i", newm_score);
    snprintf(m_scoreString, SCORE_STRING_LENGTH, "m_score: %5i", newScore);
    return;
}

/** ****************************************************************************
 * Function: startGame()
 *
*******************************************************************************/
void DotWidget::startGame()
{
    resetGameStats();
    unpauseGame();
    return;
}

/** ****************************************************************************
 * Function: pauseGame()
 *
*******************************************************************************/
void DotWidget::pauseGame()
{
    setCursor(Qt::ArrowCursor);
    m_dotTimer.stop();
    m_gameStatsTimer.stop();
    m_accelerationTimer.stop();
    m_secondTimerPtr.stop();
    m_gameState = PAUSED;
    return;
}

/** ****************************************************************************
 * Function: unpauseGame()
 *
*******************************************************************************/
void DotWidget::unpauseGame()
{
    setCursor(Qt::CrossCursor);
    m_dotTimer.start();
    m_gameStatsTimer.start();
    if (m_dotAcceleration > 0)
    {
        m_accelerationTimer.start();
    }
    m_secondTimerPtr.start();
    m_gameState = RUNNING;
    return;
}

/** ****************************************************************************
 * Function: resetGameStats()
 *
*******************************************************************************/
void DotWidget::resetGameStats()
{
    m_score = 0;
    setScoreText(m_score);
    m_elapsedGameTime = 0;
    if (m_dotAcceleration > 0)
    {
        m_dotSpeed = m_dotSpeedUserSetting;
        emit newDotSpeedNeg(-m_dotSpeed);
    }
    m_runningAvgErrorRatio = RUNNING_AVERAGE_ERROR_DEFAULT;
    // just leave the dot in the same location for the next game.
    return;
}

/** ****************************************************************************
 * Function: setGameOver()
 *  @return Running=True, Paused=False
*******************************************************************************/
void DotWidget::setGameOver(const gameOverReason& reason)
{
    m_lastGameOverReason = reason;
    pauseGame();
    m_gameState = GAMEOVER;
    update();    // redraw to show final game state, GAMEOVER text
    emit gameOver(reason);   // tell MainWindow.
    return;
}

/** ****************************************************************************
 * Function: newDotPosQpnt()
 *  temporary function to call empty newDotPos in QML.
*******************************************************************************/
void DotWidget::newDotPosQpnt(const QPoint loc) const
{
    QVariant dotLocQvar(loc);
    emit newDotLoc(dotLocQvar);
    return;
}

/** ****************************************************************************
 * Function: screenClicked()
 *
*******************************************************************************/
void DotWidget::screenClicked()
{
    switch (m_gameState)
    {
    case RUNNING:
        pauseGame();
        break;
    case PAUSED:
        unpauseGame();
        break;
    case GAMEOVER:
        startGame();
        break;
    default:
        qDebug("DotWidget::mousePressEvent: Unknown gameState!");
        break;
    }
    update();    // redraw
    return;
}
