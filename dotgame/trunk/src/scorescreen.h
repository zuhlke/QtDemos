/** ****************************************************************************
 * @file m_scorescreen.h
 * @author Jeremy Hannon <hje@zuhlke.com>
 * @version 0.4
 * @date July 11th, 2012
 * Created: June 28th, 2012
 *
 * @section LICENSE
 *     All rights reserved, Zuhlke Engineering Ltd.  Copyright 2012.
 *
 * @section DESCRIPTION
 * The DotGame is a Qt application involving a randomly walking dot that the
 * user tries to closely follow with their cursor to achieve a high m_score.
 *
 * This m_scoreScreen is used to display the top scoring players in the game.  It
 * is intended to occupy the central/main portion of the main window.
 *
 ******************************************************************************/

#ifndef m_scoreSCREEN_H
#define m_scoreSCREEN_H

#include <QWidget>
#include <QList>

/// High m_score Names are limited to 16 characters long
const int MAX_NAME_STRING_LEN = 16;

/** ****************************************************************************
 * @struct Highm_scoreData
 *   Contains the info about a single successful game in the High m_score table.
*******************************************************************************/
struct HighScoreData {
    /// Name of the Player, manually entered after the game
    QString name;
    /// Final game m_score
    uint score;
    /// How long was the game, in seconds?
    uint gameTimeLength;
    /// What was the size of the dot, in pixels?  Affects dot movement distance.
    uint dotDiam;
    /** What was the dot speed at the end of the game.  This is number of
     *      milliseconds between game refreshes.  Lower number is faster. */
    uint maxSpeed;
};

/** ****************************************************************************
 * @class m_scoreScreen
 *   This m_scoreScreen is used to display the top scoring players in the game.
 *   It is intended to occupy the central/main portion of the main window.
*******************************************************************************/
class ScoreScreen : public QWidget
{
    Q_OBJECT
public:
    explicit ScoreScreen(QWidget *parent = 0,
                         const QList<HighScoreData> *const highScoreListPtr = NULL);

    /// High m_score Names are limited to 16 characters long
    static const int MAX_NAME_STRING_LEN = 16;

protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void resizeEvent(QResizeEvent *event);
private:
    /* Private non-defined function prototypes disables compiler generation. */
    /// prevent copy operator
    ScoreScreen(const ScoreScreen&);
    /// prevent assignment operator
    ScoreScreen& operator=(const ScoreScreen&);

    /// Parent widget, for requesting window size
    QWidget *m_parentWinPtr;

    /// read-only pointer to the high m_score data
    const QList<HighScoreData>* const m_highScoreListPtr;

    /// Background color
    QBrush m_backgroundBrush;

    /// font for main "High m_scores" text
    QFont m_textFontLarge;

    /// font for small m_score table titles
    QFont m_textFontColumnTitles;

    /// font for small m_score text
    QFont m_textFontScores;

signals:
    /// The m_scoreScreen object has been mouse clicked.
    void scoreScreenClicked();

public slots:
    /// Set the Background Color for the m_scoreScreen.
    void setBackgroundColor(const int newHSVcolor);
};

#endif // m_scoreSCREEN_H
