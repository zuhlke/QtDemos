/** ****************************************************************************
 * @file m_scorescreen.cpp
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

#include "scorescreen.h"
#include <QtGui/QPainter>
#include <QtGui/QPaintEvent>
#include <QtGui/QFont>
#include <stdio.h>            // for snprintf()

/** ****************************************************************************
 * m_scoreScreen Constructor
 *   @param parent Pointer to the parent window widget
 *   @param highm_scoreListPtr Pointer to the List holding all of the current high
 *      m_score values. As the list is updated, m_scoreScreen can redraw the info.
 ******************************************************************************/
ScoreScreen::ScoreScreen(QWidget *parent, const QList<HighScoreData>* const highScoreListPtr) :
    QWidget(parent),
    m_highScoreListPtr(highScoreListPtr), m_backgroundBrush(QBrush(QColor(Qt::blue)))
{
    m_parentWinPtr = parent;
    m_textFontScores.setPointSize(8);
    m_textFontScores.setFamily("Courier");        // use a fixed-width font
    m_textFontColumnTitles.setPointSize(9);
    m_textFontColumnTitles.setFamily("Courier");  // use a fixed-width font
    m_textFontLarge.setPointSize(16);
}

/** ****************************************************************************
 * Function: paintEvent()
 *   Called by update() or repaint(), tells the Dot to redraw itself
 ******************************************************************************/
void ScoreScreen::paintEvent(QPaintEvent *event)
{
    if ((*m_highScoreListPtr).size() > 0)
    {
        QPainter painter(this);
        painter.fillRect(event->rect(), m_backgroundBrush);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setFont(m_textFontLarge);
        painter.drawText(0, size().height()/30, size().width(), size().height(),
                         Qt::AlignHCenter, "High Scores");
        const size_t HIGH_SCORE_LINE_TEXT_SIZE = 96;
        char highScoreLineText[HIGH_SCORE_LINE_TEXT_SIZE] = "";
        painter.setFont(m_textFontColumnTitles);
        snprintf(highScoreLineText, HIGH_SCORE_LINE_TEXT_SIZE, "    %13s %10s %5s %5s %6s ",
                "", "", "", "", "Max  ");
        painter.drawText(0, (size().height()/15)*2, size().width(), size().height(),
                         Qt::AlignHCenter, highScoreLineText);
        snprintf(highScoreLineText, HIGH_SCORE_LINE_TEXT_SIZE, "    %13s %10s %5s %5s %6s ",
                "Name:", "Score:", "Time:", "Size:", "Speed:");
        painter.drawText(0, (size().height()/15)*3, size().width(), size().height(),
                         Qt::AlignHCenter, highScoreLineText);
        painter.setFont(m_textFontScores);
        for (int index = 0; index < m_highScoreListPtr->size(); index++)
        {
            snprintf(highScoreLineText, HIGH_SCORE_LINE_TEXT_SIZE, "%2i. %16s %8i %5i %6i %7i ",
                    (index+1), qPrintable((*m_highScoreListPtr)[index].name), (*m_highScoreListPtr)[index].score,
                    (*m_highScoreListPtr)[index].gameTimeLength, (*m_highScoreListPtr)[index].dotDiam,
                    (*m_highScoreListPtr)[index].maxSpeed);
            painter.drawText(0, (size().height()/15)*(4+index), size().width(), size().height(),
                             Qt::AlignHCenter, highScoreLineText);
        }
    }
    return;
} // function paintEvent

/** ****************************************************************************
 * Function: mousePressEvent()
 *   Called when user clicks in the window.
 ******************************************************************************/
void ScoreScreen::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    //qDebug("ScoreScreen::mousePressEvent");
    emit scoreScreenClicked();
    return;
}

/** ****************************************************************************
 * Function: resizeEvent() reimplemented from QWidget
 *   Called after the window is resized, but before it is redrawn.
 ******************************************************************************/
void ScoreScreen::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
//    qDebug("ScoreScreen::resizeEvent: newSize=(%i,%i) MainWinSize=(%i,%i)", rect().width(),
//           rect().height(), parentWinPtr->rect().width(), parentWinPtr->rect().height());
    return;
}

/** ****************************************************************************
 * Function: setBackgroundColor()
 *  Set the Background Color for the m_scoreScreen.
 ******************************************************************************/
void ScoreScreen::setBackgroundColor(const int newHSVcolor)
{
    QColor tempHSV;
    tempHSV.setHsv(newHSVcolor,255,150);
    m_backgroundBrush.setColor(tempHSV);
    update();
    return;
}
