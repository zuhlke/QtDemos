/** ****************************************************************************
 * @file mainwindow.h
 * @author Jeremy Hannon <hje@zuhlke.com>
 * @version 0.4
 * @date July 23rd, 2012
 * Created: June 19th, 2012
 *
 * @section LICENSE
 *     All rights reserved, Zuhlke Engineering Ltd.  Copyright 2012.
 *
 * @section DESCRIPTION
 * The DotGame is a Qt application involving a randomly walking dot that the
 * user tries to closely follow with their cursor to achieve a high m_score.
 *
 * The MainWindow contains the Dot Game in the center, a Config panel docked on
 * the right side to control the game, a status bar at bottom to show current
 * game status while playing, and can switch the main game over to a high m_score
 * screen.
 *
 ******************************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QtGui/QWidget>
#include <QtGui/QSpinBox>
#include <QtCore/QDateTime>
#include <QtCore/QTimer>
#include <QtCore/QTime>
#include <QtCore/QTextStream>
#include <QtGui/QStackedLayout>
#include "dot.h"
#include "dotwidget.h"
#include "scorescreen.h"
#include "gradienttextstatusbar.h"
#include "qmlapplicationviewer.h"

/** ****************************************************************************
 * @class MainWindow
 *   The MainWindow contains the Dot Game in the center, a Config panel docked on
 *   the right side to control the game, a status bar at bottom to show current
 *   game status while playing, and can switch the main game over to a high m_score
 *   screen.
*******************************************************************************/
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    /// Max number of high m_score entries to store and display
    static const int NUM_HIGH_SCORES_MAX = 10;
    /// HSV hue.  203=soft blue
    static const uint BACKGROUND_COLOR_DEFAULT = 203;

protected:

private:
    bool updateHighScoreListFromFile(QTextStream& scoreStream);
    bool writeScoreFile(QTextStream& scoreStream) const;
    void addHighScore(int index, uint newScore);
    /* Private non-defined function prototypes disables compiler generation. */
    MainWindow(const MainWindow&);                // prevent copy operator
    MainWindow& operator=(const MainWindow&);     // prevent assignment operator

    /// contains (dotGame&QML) and HighScore screen, one at a time
    QStackedLayout *m_centralTopLayoutPtr;
    /// contains dotGame and QML screens, shown simultaneously
    QStackedLayout *m_centralGameLayoutPtr;
    /// area where the dot is limited to.
    DotWidget *m_dotGameWidgetPtr;
    /// right-side Config control panel.
    QDockWidget *m_configDotDockPtr;
    /// main area High Score Screen, swappable with dot game
    ScoreScreen *m_scoreScreenPtr;
    /// QML inclusion as a C++ widget
    QmlApplicationViewer* m_qmlViewerPtr;
    QSpinBox *m_timeSpinBoxPtr;
    /// dynamic graph bar showing distance between mouse and dot location
    GradientTextStatusBar *m_followingDistStatusBarPtr;
    QList<HighScoreData> m_highScoreList;
public slots:
    void updateDistStatusBar(const float errorDistRatio, const uint elapsedGameTime);
    void setTimedGame();
    void setInfiniteGame();
    /// Switches MainWindow to show Highm_score screen.
    void gameOver(const gameOverReason reason);
    /// Game was at High m_score Screen, user clicked, so return to game screen.
    void returnToGame();
    /// QML screen has been clicked, transfer action to C++
    void qmlClicked();
};

#endif // MAINWINDOW_H
