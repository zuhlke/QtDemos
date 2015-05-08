/** ****************************************************************************
 * @file dotwidget.h
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

#ifndef DOTWIDGET_H
#define DOTWIDGET_H

#include <QtGui/QWidget>
#include <QtGui/QBrush>
#include <QtCore/QTimer>
#include "dot.h"          // to create a dot object in the frame
#include <QVariant>

/// Possible game states for the Dot Game.
enum dotGameState {
    PAUSED = 0,
    RUNNING,
    GAMEOVER
};
/// Possible reason for the Dot Game ending.
enum gameOverReason {
    NO_GAME_OVER = 0,
    TIME_LIMIT,
    DISTANCE
};
/// Possible game time modes.  Either infinite, or a certain time in seconds.
enum gameTimeMode {
    INFINITE = 0
    // all values >0 are treated as actual times
};

/** ****************************************************************************
 * @class DotWidget
 *   The DotWidget object is the main playing field for the game. It contains the
 *   animated dot object, calculates distance and m_scores, and contains the main
 *   game state machine.
*******************************************************************************/
class DotWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DotWidget(QWidget *parent, QSize winSize);
    ~DotWidget();

    /// Adjust internal state once size/position is known.
    void initialize();

    inline dotGameState getGameState() const;
    inline uint getScore() const;
    inline uint getDotSize() const;
    inline uint getDotSpeed() const;
    inline float getErrorRatio() const;
    inline uint getElapsedTime() const;

    /// Maximum allowed distance between mouse cursor and the dot
    static const float CURSOR_DISTANCE_GAMEOVER = 30;   //10;
    /// Amount of time (seconds) for a timed game to last
    static const uint GAME_LENGTH_DEFAULT_S = 10;
    /// length of time (in ms) between game stat updates
    static const uint GAME_UPDATE_INTERVAL_MS = 80;
    /// Number of samples to average in the Position Error Ratio
    static const uint ERROR_AVERAGING_RATIO = 16;
    /// initial errorDistance value.  Non-zero to quickly stabilize the averaging.
    static const float RUNNING_AVERAGE_ERROR_DEFAULT = 3.0;


protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void resizeEvent(QResizeEvent *event);
private:
    void setScoreText(const uint newScore);

    /// Pause the game by stopping dot position updates and game timers
    void pauseGame();

    /// Unpause the game by restarting game timers
    void unpauseGame();

    /// Reset all dotGame timers, averages, and m_scores
    void resetGameStats();

    /* Private non-defined function prototypes disables compiler generation. */
    /// prevent copy operator
    DotWidget(const DotWidget&);
    /// prevent assignment operator
    DotWidget& operator=(const DotWidget&);

    /// Dot object in this window widget
    Dot* m_gameDotPtr;

    /// how many milliseconds between dot movements
    uint m_dotSpeed;

    /// What did the user set their default speed to?
    uint m_dotSpeedUserSetting;

    /// how fast the m_dotSpeed is increased
    uint m_dotAcceleration;

    /// Timer for Dot position updates (timer interval = speed)
    QTimer m_dotTimer;

    /// Timer for when the Dot Speed increases
    QTimer m_accelerationTimer;

    /// Timer for game stat calculations
    QTimer m_gameStatsTimer;

    /// for periodic update of the elapsed game time
    QTimer m_secondTimerPtr;

    /// Total game time, excluding paused time.  Valid range <24hours due to QTime.
    uint m_elapsedGameTime;

    /// Requested Game Length, in seconds.  Up to 9 hours.
    uint m_gameLength;

    /// Current state of the game
    dotGameState m_gameState;

    /// why did the last game end?
    gameOverReason m_lastGameOverReason;

    /// Running average of error ratio/distance
    float m_runningAvgErrorRatio;

    /// Cumulative game m_score
    float m_score;

    /// Game m_score as a string
    static const size_t SCORE_STRING_LENGTH = 32;
    char m_scoreString[SCORE_STRING_LENGTH];

    /// Background color
    QBrush m_backgroundBrush;

    /// font for small instructions
    QFont m_textFontSmall;

    /// font for main text
    QFont m_textFontLarge;

signals:
    /// A new ratio has been calculated = (cursor to dot distance / movement distance)
    void newFollowDistanceRatio(const float errorDistRatio, const uint m_elapsedGameTime) const;
    /// A new Dot update rate has been set.
    void newDotSpeedNeg(const int m_dotSpeedNeg) const;
    /// The Dot Game has ended, due to the reason included.
    void gameOver(const gameOverReason reason) const;
    void newDotLoc(const QVariant) const;
public slots:
    bool setDotSize(const int newSize);
    bool setDotSpeedNeg(const int newSpeedMs);
    void setDotAcceleration(const int newAcceleration);
    void increaseDotSpeed();
    void calcGameStats();
    bool setTimeLimit(const int timeLimit);
    void updateElapsedGameTime();
    void startGame();
    void setGameOver(const gameOverReason &reason);
    void setBackgroundColor(const int newHSVcolor);
    void newDotPosQpnt(const QPoint loc) const;
    void screenClicked();
};


/** ****************************************************************************
 * Function: geDotSpeed()
 *
*******************************************************************************/
inline uint DotWidget::getDotSpeed() const
{
    return m_dotSpeed;
}
/** ****************************************************************************
 * Function: getGameState()
 *
*******************************************************************************/
inline dotGameState DotWidget::getGameState() const
{
    return m_gameState;
}
/** ****************************************************************************
 * Function: getErrorRatio()
 *
*******************************************************************************/
inline float DotWidget::getErrorRatio() const
{
    return m_runningAvgErrorRatio;
}
/** ****************************************************************************
 * Function: getScore()
 *      What is the current m_score?
*******************************************************************************/
inline uint DotWidget::getScore() const
{
    return static_cast<uint>(m_score);
}
/** ****************************************************************************
 * Function: getElapsedTime()
 *      How long has the current game been running?
*******************************************************************************/
inline uint DotWidget::getElapsedTime() const
{
    return m_elapsedGameTime;
}
/** ****************************************************************************
 * Function: getDotSize()
 *      What is the diameter of the Dot, in pixels?
*******************************************************************************/
inline uint DotWidget::getDotSize() const
{
    return m_gameDotPtr->getDotSize();
}

#endif // DOTWIDGET_H

