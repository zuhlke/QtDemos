/** ****************************************************************************
 * @file mainwindow.cpp
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
 * The MainWindow contains the Dot Game in the center, a Config panel docked on
 * the right side to control the game, a status bar at bottom to show current
 * game status while playing, and can switch the main game over to a high m_score
 * screen.
 *
 ******************************************************************************/

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qmlapplicationviewer.h"
#include <QObject>
#include <QGraphicsObject>   // for rootObject()
#include <QtCore/QSize>
#include <QtCore/QFile>
#include <QtGui/QVBoxLayout>
#include <QtGui/QDockWidget>
#include <QtGui/QLabel>
#include <QtGui/QSlider>
#include <QtGui/QRadioButton>
#include <QtGui/QInputDialog>
#include <stdio.h>       // for snprintf()

//#include <QtDeclarative/QDeclarativeView>  // for qmlRegisterType
//#include "qsliderdeclarative.h"

/// For addressing the Central 'Game' StackedWidget pages
enum gameStackPageNumber {
    QML_CONTROL_PAGE = 0,           // corresponds to 'qmlViewerPtr'
    DOT_GAME_PAGE                   // corresponds to 'dotGameWidgetPtr'

    //    DOT_GAME_PAGE = 0,                   /// corresponds to 'dotGameWidgetPtr'
    //    QML_CONTROL_PAGE           /// corresponds to 'qmlViewerPtr'
};
/// For addressing the Central 'Top' StackedWidget pages
enum topStackPageNumber {
    GAME_QML_PAGE = 0,              // corresponds to 'centralGameLayoutPtr' containing DotGame&QML
    HIGH_SCORE_PAGE                 // corresponds to 'm_scoreScreenPtr'
};

/** ****************************************************************************
 * MainWindow Constructor
 *   @param parent Pointer to the parent window widget
 ******************************************************************************/
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_centralTopLayoutPtr(NULL), m_centralGameLayoutPtr(NULL),
    m_dotGameWidgetPtr(NULL), m_configDotDockPtr(NULL), m_scoreScreenPtr(NULL),
    m_qmlViewerPtr(NULL),
    m_timeSpinBoxPtr(NULL), m_followingDistStatusBarPtr(NULL)
{
    setWindowTitle(tr("Dot Game"));
    m_dotGameWidgetPtr = new DotWidget(this, size());

    // m_scoreScreen permanently holds a pointer to the highm_scoreList data.
    m_scoreScreenPtr = new ScoreScreen(this, &m_highScoreList);
    m_followingDistStatusBarPtr = new GradientTextStatusBar(this);
    statusBar()->addWidget(m_followingDistStatusBarPtr,1);
    m_dotGameWidgetPtr->setBackgroundColor(BACKGROUND_COLOR_DEFAULT);
    m_scoreScreenPtr->setBackgroundColor(BACKGROUND_COLOR_DEFAULT);

    //qmlRegisterType<QSliderDeclarative>("QtItems", 1, 0, "QSlider");

    m_qmlViewerPtr = new QmlApplicationViewer;
    m_qmlViewerPtr->setOrientation(QmlApplicationViewer::ScreenOrientationAuto);
    m_qmlViewerPtr->setMainQmlFile(QLatin1String("qml/dotgame/main.qml"));
    //QObject* rootObject = dynamic_cast<QObject*>(qmlViewerPtr->rootObject());
    QGraphicsObject* rootObject = m_qmlViewerPtr->rootObject();
    QObject::connect(m_dotGameWidgetPtr, SIGNAL(newDotLoc(QVariant)), rootObject, SLOT(updateDotLoc(QVariant)));
    QObject::connect(rootObject, SIGNAL(screenClicked()), m_dotGameWidgetPtr, SLOT(screenClicked()));
    QObject::connect(rootObject, SIGNAL(setTimeLimit(int)), m_dotGameWidgetPtr, SLOT(setTimeLimit(int)));

    m_qmlViewerPtr->showExpanded();

    /* Compose the Center of the MainWindow as the following:
     *      MainWindow
     *          | (show only one at a time)
     *          |-> High m_score Screen
     *          |-> Game&QML
     *              | (show ALL)
     *              |-> Dot Game Screen
     *              |-> QML Control Panel
     *  Note: make sure the addWidget order matches the enumerations */
    QWidget* centralWidgetPtr = new QWidget;
    m_centralGameLayoutPtr = new QStackedLayout;
    m_centralGameLayoutPtr->addWidget(m_qmlViewerPtr);
    m_centralGameLayoutPtr->addWidget(m_dotGameWidgetPtr);
    m_centralGameLayoutPtr->setStackingMode(QStackedLayout::StackAll);
    QWidget* centralGameWidgetPtr = new QWidget;
    centralGameWidgetPtr->setLayout(m_centralGameLayoutPtr);
    m_centralTopLayoutPtr = new QStackedLayout;
    m_centralTopLayoutPtr->addWidget(centralGameWidgetPtr);  // index:0 is the default active
    m_centralTopLayoutPtr->addWidget(m_scoreScreenPtr);
    m_centralTopLayoutPtr->setStackingMode(QStackedLayout::StackOne);
    centralWidgetPtr->setLayout(m_centralTopLayoutPtr);
    setCentralWidget(centralWidgetPtr);
    // DEBUG: For some reason, have to select another page, come back, to make all layers show correctly.
    //centralGameLayoutPtr->setCurrentIndex(QML_CONTROL_PAGE);
    m_centralGameLayoutPtr->setCurrentIndex(DOT_GAME_PAGE);
    m_centralGameLayoutPtr->setCurrentIndex(QML_CONTROL_PAGE);
    //    centralTopLayoutPtr->setCurrentIndex(HIGH_m_score_PAGE);
    m_centralTopLayoutPtr->setCurrentIndex(GAME_QML_PAGE);

    /* Set minimum window size so that High m_score Screen contents can be seen.
     *   However, when config window is added, it's too big. So set MainWindow size. */
    setMinimumSize(440,345);

    /* Build up the right-side Config Panel.  Not an object because no modification after creation. */
//    configDotDockPtr = new QDockWidget;
//    configDotDockPtr->setWindowTitle(tr("Config"));
//    QVBoxLayout* configLayoutPtr = new QVBoxLayout;
//    QLabel *speedLabelPtr = new QLabel(tr("&Speed:"), this);
//    QLabel *accelerationLabelPtr = new QLabel(tr("Speed-Up &Rate:"), this);
//    QLabel *diamLabelPtr = new QLabel(tr("Dot Si&ze:"), this);
//    QLabel *backgndColorLabelPtr = new QLabel(tr("Background &Color:"), this);
//    QSlider *m_dotSpeedSliderPtr = new QSlider(Qt::Horizontal);
//    QSlider *accelerationSliderPtr = new QSlider(Qt::Horizontal);
//    QSlider *dotDiamSliderPtr = new QSlider(Qt::Horizontal);
//    QSlider *backgndColorSliderPtr = new QSlider(Qt::Horizontal);
//    speedLabelPtr->setBuddy(m_dotSpeedSliderPtr);
//    diamLabelPtr->setBuddy(dotDiamSliderPtr);
//    backgndColorLabelPtr->setBuddy(backgndColorSliderPtr);
//    accelerationLabelPtr->setBuddy(accelerationSliderPtr);
//    // Speed slider values inverted so that bigger/slower number on left side.
//    m_dotSpeedSliderPtr->setRange(-DOT_SPEED_MAX, -DOT_SPEED_MIN);
//    m_dotSpeedSliderPtr->setValue(-DOT_SPEED_DEFAULT);
//    accelerationSliderPtr->setRange(DOT_ACCEL_MIN,DOT_ACCEL_MAX);
//    accelerationSliderPtr->setValue(DOT_ACCEL_DEFAULT);
//    dotDiamSliderPtr->setRange(DOT_DIAM_MIN, DOT_DIAM_MAX);
//    dotDiamSliderPtr->setValue(DOT_DIAM_DEFAULT);
//    // For linear color slider, use HSV hue encoding 0-360 degrees
//    backgndColorSliderPtr->setRange(0,360);
//    backgndColorSliderPtr->setValue(BACKGROUND_COLOR_DEFAULT);
//    QWidget* m_gameLengthBoxPtr = new QWidget();
//    QRadioButton* timedBtnPtr = new QRadioButton(tr("&Timed"), m_gameLengthBoxPtr);
//    timeSpinBoxPtr = new QSpinBox(this);
//    QRadioButton* infiniteBtnPtr = new QRadioButton(tr("&Infinite"), m_gameLengthBoxPtr);
//    QVBoxLayout *m_gameLengthVbox = new QVBoxLayout;
//    QHBoxLayout *gameTimeHbox = new QHBoxLayout;
//    timeSpinBoxPtr->setValue(GAME_LENGTH_DEFAULT_S);
//    gameTimeHbox->addWidget(timedBtnPtr);
//    gameTimeHbox->addWidget(timeSpinBoxPtr);
//    m_gameLengthVbox->addLayout(gameTimeHbox);
//    timedBtnPtr->setChecked(TRUE);
//    m_gameLengthVbox->addWidget(infiniteBtnPtr);
//    m_gameLengthVbox->addStretch(1);
//    m_gameLengthBoxPtr->setLayout(m_gameLengthVbox);
//    configLayoutPtr->addWidget(speedLabelPtr);
//    configLayoutPtr->addWidget(m_dotSpeedSliderPtr);
//    configLayoutPtr->addWidget(accelerationLabelPtr);
//    configLayoutPtr->addWidget(accelerationSliderPtr);
//    configLayoutPtr->addWidget(diamLabelPtr);
//    configLayoutPtr->addWidget(dotDiamSliderPtr);
//    configLayoutPtr->addWidget(backgndColorLabelPtr);
//    configLayoutPtr->addWidget(backgndColorSliderPtr);
//    configLayoutPtr->addWidget(m_gameLengthBoxPtr);
//    QWidget* dockWidg = new QWidget;
//    dockWidg->setLayout(configLayoutPtr);
//    configDotDockPtr->setWidget(dockWidg);
//    addDockWidget(Qt::RightDockWidgetArea,configDotDockPtr);

//    QObject::connect(m_dotSpeedSliderPtr, SIGNAL(valueChanged(int)),
//                     dotGameWidgetPtr, SLOT(setm_dotSpeedNeg(int)));
//    QObject::connect(dotGameWidgetPtr, SIGNAL(newm_dotSpeedNeg(int)),
//                     m_dotSpeedSliderPtr, SLOT(setValue(int)));
//    QObject::connect(accelerationSliderPtr, SIGNAL(valueChanged(int)),
//                     dotGameWidgetPtr, SLOT(setm_dotAcceleration(int)));
//    QObject::connect(dotDiamSliderPtr, SIGNAL(valueChanged(int)),
//                     dotGameWidgetPtr, SLOT(setm_dotSize(int)));
//    QObject::connect(backgndColorSliderPtr, SIGNAL(valueChanged(int)),
//                     dotGameWidgetPtr, SLOT(setBackgroundColor(int)));
//    QObject::connect(backgndColorSliderPtr, SIGNAL(valueChanged(int)),
//                     m_scoreScreenPtr, SLOT(setBackgroundColor(int)));
//    QObject::connect(timeSpinBoxPtr, SIGNAL(valueChanged(int)),
//                     dotGameWidgetPtr, SLOT(setTimeLimit(int)));
//    QObject::connect(timedBtnPtr, SIGNAL(clicked()),
//                     this, SLOT(setTimedGame()));
//    QObject::connect(infiniteBtnPtr, SIGNAL(clicked()),
//                     this, SLOT(setInfiniteGame()));



    QObject::connect(m_dotGameWidgetPtr, SIGNAL(newFollowDistanceRatio(float, uint)),
                     this, SLOT(updateDistStatusBar(float, uint)));
    QObject::connect(m_dotGameWidgetPtr, SIGNAL(gameOver(gameOverReason)),
                     this, SLOT(gameOver(gameOverReason)));
    QObject::connect(m_scoreScreenPtr, SIGNAL(scoreScreenClicked()),
                     this, SLOT(returnToGame()));

    show();             // update all window settings to adjust sizes
    m_dotGameWidgetPtr->initialize();  // re-center dot position

    return;
} // MainWindow ctor

/** MainWindow Destructor */
MainWindow::~MainWindow()
{
    // ONLY delete objects that are not in the window hierarchy
    return;
}


/** ****************************************************************************
 * Function: updateDistStatusBar()
 *  Using the Distance Ratio and m_elapsedGameTime, tell the status bar to redraw.
 *    @param errorDistRatio Generally, the ratio of the current mouse cursor
 *      distance from the dot, divided by the pixel diameter of the dot to be the
 *      number of circle diameters away. Normalize before sending to StatusBar.
 *    @param m_elapsedGameTime The number of seconds that the game has been running,
 *      minus any game pauses.
*******************************************************************************/
void MainWindow::updateDistStatusBar(const float errorDistRatio, const uint elapsedGameTime)
{
    float gameRatio = errorDistRatio / DotWidget::CURSOR_DISTANCE_GAMEOVER;
    m_followingDistStatusBarPtr->updateValues(gameRatio, elapsedGameTime);
    return;
}

/** ****************************************************************************
 * Function: setTimedGame()
 *  Set the game to have a time limit
*******************************************************************************/
void MainWindow::setTimedGame()
{
    m_dotGameWidgetPtr->setTimeLimit(m_timeSpinBoxPtr->value());
    return;
}

/** ****************************************************************************
 * Function: setInfiniteGame()
 *  Set the game to have no time limit
*******************************************************************************/
void MainWindow::setInfiniteGame()
{
    m_dotGameWidgetPtr->setTimeLimit(INFINITE);
    return;
}

/** ****************************************************************************
 * Function: restartGame()
 *    Game was at High m_score Screen, user clicked, so return to game screen.
*******************************************************************************/
void MainWindow::returnToGame()
{
    // Make the DotGame the main window again, including config panel.
    //addDockWidget(Qt::RightDockWidgetArea,configDotDockPtr);    // didn't work.
    //configDotDockPtr->show();
    m_centralTopLayoutPtr->setCurrentIndex(GAME_QML_PAGE);
    update();
    return;
}

/** ****************************************************************************
 * Function: gameOver()
 *   MainWindow gets informed by dotGameWidget that game is over.
*******************************************************************************/
void MainWindow::gameOver(const gameOverReason reason)
{
    Q_UNUSED(reason);
    // End Game activities (timers, dot motion, status bar updates, etc)
    updateDistStatusBar(m_dotGameWidgetPtr->getErrorRatio(),
                        m_dotGameWidgetPtr->getElapsedTime());
    uint newScore = m_dotGameWidgetPtr->getScore();
    //qDebug("MainWindow::gameOver: newScore:%i", newScore);

    // read the m_score file of previous High m_scores
    QString fileName = "dotGameScores.txt";
    QFile file(fileName);
    QTextStream stream(&file);
    if (m_highScoreList.size() == 0)  // if the file has not been read already.
    {
        if(!file.open(QIODevice::ReadWrite | QIODevice::Text))
        {
            qDebug("MainWindow::gameOver: Failed to open score File.");
            return;
        }
        updateHighScoreListFromFile(stream);
        file.close();
    }

    // Does the new m_score qualify for the High Score List?
    if (m_highScoreList.size() == 0)
    {
        addHighScore(0, newScore);
    }
    else
    {
        for (int i = 0; i < m_highScoreList.size(); ++i)
        {
            if (newScore > m_highScoreList[i].score)
            {
                // Made the High Score List!
                addHighScore(i, newScore);
                break;   // exit the for loop, don't touch the other m_scores.
            }
            else if ((i >= (m_highScoreList.size()-1)) &&
                     (m_highScoreList.size() < NUM_HIGH_SCORES_MAX))
            {
                /* newm_score does not beat any m_score in the list, but there is room
                 *   to add another m_score to the list. */
                addHighScore(i+1, newScore);
                break;   // exit the for loop
            }
        } // for each m_score in the list
    } // if listSize==0

    // save the new QList of scores back to the file. (every time)
    if(!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
    {
        qDebug("MainWindow::gameOver: Failed to open score File.");
        return;
    }
    writeScoreFile(stream);
    stream.flush();    // make sure that the stream's write buffer is written to the file.
    file.close();

    // draw the high m_score table, maybe just by changing what is shown/hidden.
    m_centralTopLayoutPtr->setCurrentIndex(HIGH_SCORE_PAGE);
    //configDotDockPtr->hide();

    return;
}

/** ****************************************************************************
 * Function: updateHighScoreListFromFile()
 *
*******************************************************************************/
bool MainWindow::updateHighScoreListFromFile(QTextStream &scoreStream)
{
    HighScoreData dataRow;
    scoreStream.skipWhiteSpace();
    while (!scoreStream.atEnd())
    {
        scoreStream >> dataRow.name >> dataRow.score >> dataRow.gameTimeLength
                    >> dataRow.dotDiam >> dataRow.maxSpeed;
        m_highScoreList.append(dataRow);
        scoreStream.skipWhiteSpace();
    }
    return true;
}

/** ****************************************************************************
 * Function: writeScoreFile()
 *
*******************************************************************************/
bool MainWindow::writeScoreFile(QTextStream &scoreStream) const
{
    HighScoreData dataRow = {"", 0, 0, 0, 0};
    foreach (dataRow, m_highScoreList)
    {
        if ((dataRow.name != "") && (dataRow.name != "\n"))    // verify that the next list entry is not empty
        {
            scoreStream << dataRow.name << " " << dataRow.score << " "
                        << dataRow.gameTimeLength << " " << dataRow.dotDiam
                        << " " << dataRow.maxSpeed << "\n";
        }
    }
    return true;
}

/** ****************************************************************************
 * Function: addHighm_score()
 *     Prompt the user for their name, then add an item to the list with their
 *     name and relevant data.  Chop off the last item in the list if necessary.
*******************************************************************************/
void MainWindow::addHighScore(int index, uint newScore)
{
    bool ok;
    QString nameText = QInputDialog::getText(this, tr("New High m_score!"),
                                             tr("Please Enter Your Name:"),
                                             QLineEdit::Normal,
                                             "name", &ok);
    if (ok && !nameText.isEmpty())
    {
        /* Entered name could contain spaces, tabs, newlines.
         *    Bad for reading/writing space-seperated file.  */
        nameText.replace(QRegExp("[ \t\n\r]"), "_");
        if (nameText.size() > MAX_NAME_STRING_LEN)
        {
            nameText.resize(MAX_NAME_STRING_LEN);
        }
        HighScoreData scoreData = {nameText,newScore,m_dotGameWidgetPtr->getElapsedTime(),
                                   m_dotGameWidgetPtr->getDotSize(),
                                   m_dotGameWidgetPtr->getDotSpeed()};
        m_highScoreList.insert(index,scoreData);
        while (m_highScoreList.size() > NUM_HIGH_SCORES_MAX)
        {
            m_highScoreList.removeLast();
        }
    }
    return;
}

/** ****************************************************************************
 * Function: qmlClicked()
 *
*******************************************************************************/
void MainWindow::qmlClicked()
{
    qDebug("MainWindow::qmlClicked");
    return;
}
